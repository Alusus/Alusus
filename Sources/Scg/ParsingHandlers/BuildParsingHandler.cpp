/**
 * @file Scg/ParsingHandlers/BuildParsingHandler.cpp
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
#include <ParsingHandlers/BuildParsingHandler.h>

namespace Scg
{

using namespace Core;
using namespace Core::Data;

static std::ostream & Cout = std::cout;

//==============================================================================
// Overloaded Abstract Functions

void BuildParsingHandler::onProdEnd(Processing::Parser *parser, Processing::ParserState *state)
{
  SharedPtr<IdentifiableObject> item = state->getData();

  static CodeGenerator generator;
  static ReferenceSeeker seeker;

  // Set the aliases dictionary.
  generator.SetAliasDictionary(static_cast<SharedMap*>(state->getDataStack()->tryGet(this->aliasDictionaryRef.get())));

  static SharedPtr<Reference> nameReference = ReferenceParser::parseQualifier(
    STR("self~where(prodId=Subject.Subject1).{find prodId=Subject.Parameter, 0}"),
    ReferenceUsageCriteria::MULTI_DATA);

  // Find the name of the module to execute.
  auto name = io_cast<ParsedToken>(seeker.tryGet(nameReference.get(), item.get()));
  /*SharedPtr<Module> statementList;
  if (name != 0) {
    statementList = this->rootManager->getDefinitionsStore()->getValue(name->getText().c_str())
                    .io_cast<Module>();
  }*/
  if (true /*statementList != 0*/) {
    // Execute a list of statements.
    try {
      LlvmContainer::Initialize();
      Program program;
      auto *rootModule = this->rootManager->getDefinitionsRepository()->getLevelData(0).io_cast_get<Data::Module>();
      for (auto i = 0; i < rootModule->getCount(); i++) {
        auto statList = rootModule->getShared(i).io_cast<Data::Module>();
        if (statList == 0) continue;
        Module *module = generator.GenerateModule(name->getText(), statList);
        program.AddModule(module);
      }
      Cout << STR("---------------------- IR Code -----------------------\n");
      Cout << program.Compile();
      Cout << STR("------------------------------------------------------\n");
      LlvmContainer::Finalize();
    } catch (const Scg::Exception &e) {
      // TODO: Use the source code position once they are added to the module definition.
      //state->addBuildMsg(SharedPtr<Processing::BuildMsg>(new Processing::CustomBuildMsg(e.GetMessage().c_str(),
      //  statementList->getLine(), statementList->getColumn())));
      state->addBuildMsg(SharedPtr<Processing::BuildMsg>(new Processing::CustomBuildMsg(e.GetMessage().c_str(), 0, 0)));
    } catch (Core::Basic::Exception &e) {
      // TODO: Use the source code position once they are added to the module definition.
      //state->addBuildMsg(SharedPtr<Processing::BuildMsg>(new Processing::CustomBuildMsg(e.getErrorMessage(),
      //  statementList->getLine(), statementList->getColumn())));
      state->addBuildMsg(SharedPtr<Processing::BuildMsg>(new Processing::CustomBuildMsg(e.getErrorMessage(), 0, 0)));
    }
  } else {
      // Create a build message.
      Str message = "Couldn't find module: ";
      message += name->getText();
      auto metadata = item.ii_cast_get<ParsingMetadataHolder>();
      if (metadata != nullptr) {
        state->addBuildMsg(std::make_shared<Processing::CustomBuildMsg>(message.c_str(),
          metadata->getLine(), metadata->getColumn()));
      } else {
        state->addBuildMsg(std::make_shared<Processing::CustomBuildMsg>(message.c_str(), -1, -1));
      }
  }
  // Reset parsed data because we are done with the command.
  state->setData(SharedPtr<IdentifiableObject>(0));
}

} // namespace
