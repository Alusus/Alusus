/**
 * @file Scg/ParsingHandlers/RunParsingHandler.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <core.h>   // Alusus core header files.
#include <llvm/IR/IRBuilder.h>
#include <scg.h>
#include <CodeGeneration/CodeGenerator.h>
#include <ParsingHandlers/RunParsingHandler.h>

namespace Scg
{

using namespace Core;
using namespace Core::Data;

//==============================================================================
// Overloaded Abstract Functions

void RunParsingHandler::onProdEnd(Processing::Parser *parser, Processing::ParserState *state)
{
  SharedPtr<IdentifiableObject> item = state->getData();

  static CodeGenerator generator;
  static ReferenceSeeker seeker;

  // Set the build msg store.
  generator.setBuildMsgStore(state->getBuildMsgStore());

  // Set the aliases dictionary.
  generator.setAliasDictionary(static_cast<SharedMap*>(state->getDataStack()->tryGet(this->aliasDictionaryRef.get())));

  static SharedPtr<Reference> nameReference = REF_PARSER->parseQualifier(
        STR("1~where(prodId=Subject.Subject1)."
            "{find prodId=Subject.Parameter, 0}"),
        ReferenceUsageCriteria::MULTI_DATA);

  // Find the name of the module to execute.
  auto name = io_cast<PrtToken>(seeker.tryGet(nameReference.get(), item.get()));

  /*SharedPtr<Module> statementList;
  if (name != 0) {
  statementList = this->rootManager->getDefinitionsStore()->getValue(name->getText().c_str())
  .io_cast<Module>();
  }*/
  if (true /*statementList != 0*/) {
    // Execute a list of statements.
    try {
      Word prevBuildMsgCount = state->getBuildMsgStore()->getCount();
      LlvmContainer::initialize();
      Program program;
      program.setBuildMsgStore(state->getBuildMsgStore());
      auto *rootModule = this->rootManager->getDefinitionsRepository()->getLevelData(0).io_cast_get<Data::Module>();

      for (auto i = 0; i < rootModule->getCount(); i++) {
        auto statList = rootModule->getShared(i).io_cast<Data::Module>();

        if (statList == 0) continue;

        Module *module = generator.generateModule(name->getText(), statList);
        program.addModule(module);
      }

      auto str = name->getText() + "_main";

      if (state->getBuildMsgStore()->getCount() == prevBuildMsgCount) {
        // No errors were found, so proceed to execute.
        program.execute(str.c_str());
      } else {
        // Some errors were found, so we'll not execute.
        state->addBuildMsg(std::make_shared<Processing::CustomBuildMsg>(STR("Couldn't execute module due to build errors"),
                           STR("SCG1001"), 1, name->getSourceLocation(),
                           name->getText().c_str()));
      }

      LlvmContainer::finalize();
    } catch (Core::Basic::Exception &e) {
      // TODO: Use the source code position once they are added to the module definition.
      //state->addBuildMsg(SharedPtr<Processing::BuildMsg>(new Processing::CustomBuildMsg(e.getErrorMessage(),
      //  statementList->getLine(), statementList->getColumn())));
      state->addBuildMsg(std::make_shared<Processing::CustomBuildMsg>(e.getErrorMessage().c_str(),
                         name->getSourceLocation()));
    }
  } else {
    // Create a build message.
    Str message = "Couldn't find module: ";
    message += name->getText();
    auto metadata = item.ii_cast_get<ParsingMetadataHolder>();

    if (metadata != nullptr) {
      state->addBuildMsg(std::make_shared<Processing::CustomBuildMsg>(message.c_str(),
                         metadata->getSourceLocation()));
    } else {
      state->addBuildMsg(std::make_shared<Processing::CustomBuildMsg>(message.c_str(), name->getSourceLocation()));
    }
  }

  // Reset parsed data because we are done with the command.
  state->setData(SharedPtr<IdentifiableObject>(0));
}

} // namespace
