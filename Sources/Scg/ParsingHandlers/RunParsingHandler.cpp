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

//==============================================================================
// Overloaded Abstract Functions

void RunParsingHandler::onProdEnd(Processing::Parser *machine, Processing::ParserState *state)
{
  SharedPtr<Standard::ParsedItem> item = state->getData().io_cast<Standard::ParsedItem>();

  static CodeGenerator generator;
  static Standard::ParsedDataBrowser nameBrowser;
  if (!nameBrowser.isInitialized()) nameBrowser.initialize(STR("Subject.Subject1>Subject.Parameter"));

  // Find the name of the module to execute.
  /*SharedPtr<Standard::ParsedToken> name = nameBrowser.getValue<Standard::ParsedToken>(item);
  SharedPtr<Data::Module> statementList;
  if (name != 0) {
    statementList = this->rootManager->getDefinitionsStore()->getValue(name->getText().c_str())
                    .io_cast<Data::Module>();
  }*/
  if (true /*statementList != 0*/) {
    // Execute a list of statements.
    try {
      LlvmContainer::Initialize();
      Program program;
      Data::Module *rootModule = this->rootManager->getDefinitionsRepository()->getLevelData(0).io_cast_get<Data::Module>();
      for (auto i = 0; i < rootModule->getCount(); i++) {
        auto statList = rootModule->get(i).io_cast<Data::Module>();
        Module *module = generator.GenerateModule(statList);
        program.AddModule(module);
      }
      program.Execute("main");
      LlvmContainer::Finalize();
    } catch (const Scg::Exception &e) {
      // TODO: Use the source code position once they are added to the module definition.
      //state->addBuildMsg(SharedPtr<Processing::BuildMsg>(new Processing::CustomBuildMsg(e.GetMessage().c_str(),
      //  statementList->getLine(), statementList->getColumn())));
      state->addBuildMsg(std::make_shared<Processing::CustomBuildMsg>(e.GetMessage().c_str(), 0, 0));
    } catch (Core::Basic::Exception &e) {
      // TODO: Use the source code position once they are added to the module definition.
      //state->addBuildMsg(SharedPtr<Processing::BuildMsg>(new Processing::CustomBuildMsg(e.getErrorMessage(),
      //  statementList->getLine(), statementList->getColumn())));
      state->addBuildMsg(std::make_shared<Processing::CustomBuildMsg>(e.getErrorMessage(), 0, 0));
    }
  } else {
      // Create a build message.
      Str message = "Couldn't find module: ";
      message += "Rafid"; //name->getText();
      state->addBuildMsg(std::make_shared<Processing::CustomBuildMsg>(message.c_str(),
                                                                      item->getLine(), item->getColumn()));
  }
  // Reset parsed data because we are done with the command.
  state->setData(SharedPtr<IdentifiableObject>(0));
}

} // namespace
