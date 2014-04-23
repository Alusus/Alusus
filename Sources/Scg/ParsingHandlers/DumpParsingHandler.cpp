/**
 * @file Scg/ParsingHandlers/DumpParsingHandler.cpp
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
#include <ParsingHandlers/DumpParsingHandler.h>

namespace Scg
{

using namespace Core;

//==============================================================================
// Helper Functions

static std::ostream & Cout = std::cout;

void printIndents(int indents)
{
    for (Int i=0; i < indents; ++i) {
        Cout << STR(" ");
    }
}

void debugPrintParsedData(const SharedPtr<IdentifiableObject> &ptr, int indents=0, Bool start_indent=true)
{
    if (start_indent) printIndents(indents);
    if (ptr == 0) {
        Cout << STR("NULL:\n");
        return;
    }

    // Is this a default data type?
    if (ptr->isDerivedFrom<Standard::ParsedItem>()) {
        // Print the production name.
        Word id = ptr.io_cast_get<Standard::ParsedItem>()->getProdId();
        if (id != UNKNOWN_ID) {
            Cout << Data::IdGenerator::getSingleton()->getDesc(id) << STR(" -- ");
        }
    } else {
        // Unkown datat type not even derived form ParsedItem.
        Cout << ptr->getMyTypeInfo()->getUniqueName() << STR(" -- ");
    }
    // Print the data itself.
    Data::MapSharedContainer *mapContainer;
    Data::ListSharedContainer *listContainer;
    if (ptr->isDerivedFrom<Standard::ParsedList>()) {
        Cout << STR("[LIST]:\n");
        for (Word i = 0; i < ptr.s_cast_get<Standard::ParsedList>()->getElementCount(); ++i) {
            debugPrintParsedData(ptr.s_cast_get<Standard::ParsedList>()->getElement(i), indents+1);
        }
    } else if (ptr->isDerivedFrom<Standard::ParsedRoute>()) {
        Cout << STR("[ROUTE]: ");
        Cout << ptr.s_cast_get<Standard::ParsedRoute>()->getRoute() << STR("\n");
        debugPrintParsedData(ptr.s_cast_get<Standard::ParsedRoute>()->getData(), indents+1);
    } else if (ptr->isDerivedFrom<Standard::ParsedToken>()) {
        Cout << STR("[TOKEN]: ");
        // Print the token type.
        Int id = ptr.s_cast_get<Standard::ParsedToken>()->getId();
        Cout << Data::IdGenerator::getSingleton()->getDesc(id);
        // Print the token text.
        Cout << STR(" (\"") << ptr.s_cast_get<Standard::ParsedToken>()->getText() << STR("\")\n");
    } else if ((listContainer = ptr->getInterface<Data::ListSharedContainer>()) != 0) {
        Cout << STR("[LIST]:\n");
        for (Word i = 0; i < listContainer->getCount(); ++i) {
            debugPrintParsedData(listContainer->get(i), indents+1);
        }
    } else if ((mapContainer = ptr->getInterface<Data::MapSharedContainer>()) != 0) {
        Cout << STR("[MAP]:\n");
        for (Word i = 0; i < mapContainer->getCount(); ++i) {
            printIndents(indents+1);
            Cout << mapContainer->getKey(i) << STR(": ");
            debugPrintParsedData(mapContainer->get(i), indents+1, false);
        }
    } else {
        // A default parsed item but not one of the three known types.
        Cout << STR("[UNKNOWN TYPE]\n");
    }
}


//==============================================================================
// Overloaded Abstract Functions

void DumpParsingHandler::onProdEnd(Parser::StateMachine *machine, Parser::State *state)
{
  SharedPtr<Standard::ParsedItem> item = state->getData().io_cast<Standard::ParsedItem>();

  static Standard::ParsedDataBrowser nameBrowser;
  if (!nameBrowser.isInitialized()) nameBrowser.initialize(STR("Subject.Subject1>Subject.Parameter"));

  // Find the name of the module to execute.
  SharedPtr<Standard::ParsedToken> name = nameBrowser.getValue<Standard::ParsedToken>(item);
  SharedPtr<IdentifiableObject> def;
  if (name != 0) {
    def = this->rootManager->getDefinitionsStore()->getSharedValue(name->getText().c_str());
  }
  if (def!= 0) {
    Cout << STR("------------------ Parsed Data Dump ------------------\n");
    debugPrintParsedData(def);
    Cout << STR("------------------------------------------------------\n");
  } else {
      // Create a build msg.
      Str message = "Couldn't find module: ";
      message += name->getText();
      state->addBuildMsg(std::make_shared<Common::CustomBuildMsg>(message.c_str(),
                                                                      item->getLine(), item->getColumn()));
  }
  // Reset parsed data because we are done with the command.
  state->setData(SharedPtr<IdentifiableObject>(0));
}

} // namespace
