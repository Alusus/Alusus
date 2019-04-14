/**
 * @file Spp/RootScopeHandlerExtension.cpp
 * Contains the implementation of class Spp::RootScopeHandlerExtension.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp
{

//==============================================================================
// Initialization Functions

RootScopeHandlerExtension::Overrides* RootScopeHandlerExtension::extend(
  Core::Main::RootScopeHandler *handler, Core::Main::RootManager *rootManager,
  SharedPtr<Ast::Helper> const &astHelper, SharedPtr<CodeGen::MacroProcessor> const &macroProcessor,
  SharedPtr<CodeGen::Generator> const &generator, SharedPtr<LlvmCodeGen::TargetGenerator> const &targetGenerator
) {
  auto extension = std::make_shared<RootScopeHandlerExtension>(handler);
  handler->addDynamicInterface(extension);

  auto overrides = new Overrides();
  extension->rootStatementIndex = TiInt::create(0);
  extension->rootManagerBox = Box<Core::Main::RootManager*>::create(rootManager);
  extension->astHelper = astHelper;
  extension->macroProcessor = macroProcessor;
  extension->generator = generator;
  extension->targetGenerator = targetGenerator;
  overrides->addNewElementRef = handler->addNewElement.set(&RootScopeHandlerExtension::_addNewElement).get();
  overrides->executeElementRef = extension->executeElement.set(&RootScopeHandlerExtension::_executeElement).get();

  return overrides;
}


void RootScopeHandlerExtension::unextend(Core::Main::RootScopeHandler *handler, Overrides *overrides)
{
  auto extension = ti_cast<RootScopeHandlerExtension>(handler);
  handler->addNewElement.reset(overrides->addNewElementRef);
  extension->executeElement.reset(overrides->executeElementRef);
  extension->rootStatementIndex.remove();
  extension->rootManagerBox.remove();
  extension->astHelper.remove();
  extension->macroProcessor.remove();
  extension->generator.remove();
  extension->targetGenerator.remove();
  handler->removeDynamicInterface<RootScopeHandlerExtension>();
  delete overrides;
}


//==============================================================================
// Main Functions

void RootScopeHandlerExtension::_addNewElement(
  TiFunctionBase *base, TiObject *self, TioSharedPtr const &data, Core::Processing::ParserState *state
) {
  PREPARE_SELF(rootScopeHandler, Core::Main::RootScopeHandler);
  rootScopeHandler->addNewElement.useCallee(base)(data, state);
  if (data != 0 && !data->isDerivedFrom<Core::Data::Ast::Definition>()) {
    PREPARE_SELF(extension, RootScopeHandlerExtension);
    extension->executeElement(data, state);
  }
}


void RootScopeHandlerExtension::_executeElement(
  TiObject *self, TioSharedPtr const &element, Core::Processing::ParserState *state
) {
  // TODO:
  PREPARE_SELF(rootScopeHandlerExt, RootScopeHandlerExtension);

  auto root = rootScopeHandlerExt->rootManagerBox->get()->getRootScope().get();

  // Perform macro processing.
  rootScopeHandlerExt->macroProcessor->preparePass(state->getNoticeStore());
  Bool result = rootScopeHandlerExt->macroProcessor->runMacroPass(root);

  if (result) {
    // Build the IR code.
    rootScopeHandlerExt->targetGenerator->setNoticeStore(state->getNoticeStore());
    auto targetGeneration = ti_cast<CodeGen::TargetGeneration>(rootScopeHandlerExt->targetGenerator.get().get());
    rootScopeHandlerExt->generator->prepareBuild(state);
    auto generation = ti_cast<CodeGen::Generation>(rootScopeHandlerExt->generator.get().get());

    // Generate function type.
    TioSharedPtr tgVoidType;
    if (!rootScopeHandlerExt->targetGenerator->generateVoidType(tgVoidType)) {
      throw EXCEPTION(GenericException, S("Failed to generate LLVM void type."));
    }
    SharedMap<TiObject> argTypes;
    TioSharedPtr tgFuncType;
    if (!rootScopeHandlerExt->targetGenerator->generateFunctionType(&argTypes, tgVoidType.get(), false, tgFuncType)) {
      throw EXCEPTION(GenericException, S("Failed to generate function type for root scope execution."));
    }

    // Prepare a function name.
    Str funcName = Str(S("__rootstatement__")) + std::to_string(rootScopeHandlerExt->rootStatementIndex->get());
    rootScopeHandlerExt->rootStatementIndex->operator++();

    // Generate the function.
    TioSharedPtr tgFunc;
    if (!rootScopeHandlerExt->targetGenerator->generateFunctionDecl(funcName.c_str(), tgFuncType.get(), tgFunc)) {
      throw EXCEPTION(GenericException, S("Failed to generate function declaration for root scope statement."));
    }
    SharedList<TiObject> args;
    TioSharedPtr context;
    if (!rootScopeHandlerExt->targetGenerator->prepareFunctionBody(tgFunc.get(), tgFuncType.get(), &args, context)) {
      throw EXCEPTION(GenericException, S("Failed to generate function body for root scope statement."));
    }
    CodeGen::TerminalStatement terminal;
    result = generation->generateStatement(element.get(), targetGeneration, context.get(), terminal);
    if (!rootScopeHandlerExt->targetGenerator->finishFunctionBody(
      tgFunc.get(), tgFuncType.get(), &args, context.get()
    )) {
      throw EXCEPTION(GenericException, S("Failed to finalize function body for root scope statement."));
    }

    // Execute
    if (result) {
      rootScopeHandlerExt->targetGenerator->execute(funcName.c_str(), false, 0, 0);
    }
  }
}

} // namespace
