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
#include <ParsingHandlers/DumpParsingHandler.h>

namespace Scg
{

using namespace Core;
using namespace Core::Data;

//==============================================================================
// Helper Functions

static std::ostream & Cout = std::cout;

void printIndents(int indents)
{
    for (Int i=0; i < indents; ++i) {
        Cout << STR(" ");
    }
}

void debugPrintParsedData(IdentifiableObject *ptr, int indents=0, Bool start_indent=true)
{
    if (start_indent) printIndents(indents);
    if (ptr == 0) {
        Cout << STR("NULL:\n");
        return;
    }

    // Is this a default data type?
    ParsingMetadataHolder *metadata;
    if ((metadata = ptr->getInterface<ParsingMetadataHolder>()) != 0) {
        // Print the production name.
        Word id = metadata->getProdId();
        if (id != UNKNOWN_ID) {
            Cout << IdGenerator::getSingleton()->getDesc(id) << STR(" -- ");
        }
    } else {
        // Unkown data type not even implementing ParsingMetadataHolder.
        Cout << ptr->getMyTypeInfo()->getUniqueName() << STR(" -- ");
    }
    // Print the data itself.
    MapContainer *mapContainer;
    ListContainer *listContainer;
    if (ptr->isDerivedFrom<ParsedList>()) {
        Cout << STR("[LIST]:\n");
        for (Word i = 0; i < static_cast<ParsedList*>(ptr)->getCount(); ++i) {
            debugPrintParsedData(static_cast<ParsedList*>(ptr)->get(i), indents+1);
        }
    } else if (ptr->isDerivedFrom<ParsedRoute>()) {
        Cout << STR("[ROUTE]: ");
        Cout << static_cast<ParsedRoute*>(ptr)->getRoute() << STR("\n");
        debugPrintParsedData(static_cast<ParsedRoute*>(ptr)->getData().get(), indents+1);
    } else if (ptr->isDerivedFrom<ParsedToken>()) {
        Cout << STR("[TOKEN]: ");
        // Print the token type.
        Int id = static_cast<ParsedToken*>(ptr)->getId();
        Cout << IdGenerator::getSingleton()->getDesc(id);
        // Print the token text.
        Cout << STR(" (\"") << static_cast<ParsedToken*>(ptr)->getText() << STR("\")\n");
    } else if ((listContainer = ptr->getInterface<ListContainer>()) != 0) {
        Cout << STR("[LIST]:\n");
        for (Word i = 0; i < listContainer->getCount(); ++i) {
            debugPrintParsedData(listContainer->get(i), indents+1);
        }
    } else if ((mapContainer = ptr->getInterface<MapContainer>()) != 0) {
        Cout << STR("[MAP]:\n");
        for (Word i = 0; i < mapContainer->getCount(); ++i) {
            printIndents(indents+1);
            Cout << mapContainer->getKey(i).c_str() << STR(": ");
            debugPrintParsedData(mapContainer->get(i), indents+1, false);
        }
    } else {
        // A default parsed item but not one of the three known types.
        Cout << STR("[UNKNOWN TYPE]\n");
    }
}


//==============================================================================
// Overloaded Abstract Functions

void DumpParsingHandler::onProdEnd(Processing::Parser *parser, Processing::ParserState *state)
{
  SharedPtr<IdentifiableObject> item = state->getData();

  static ReferenceSeeker seeker;
  static SharedPtr<Reference> nameReference = ReferenceParser::parseQualifier(
    STR("self~where(prodId=Subject.Subject1).{find prodId=Subject.Parameter, 0}"),
    ReferenceUsageCriteria::MULTI_DATA);

  // Find the name of the module to execute.
  auto name = io_cast<ParsedToken>(seeker.tryGet(nameReference.get(), item.get()));
  IdentifiableObject *def = 0;
  if (name != 0) {
    def = this->rootManager->getDefinitionsRepository()->get(name->getText().c_str());
  }
  if (def!= 0) {
    Cout << STR("------------------ Parsed Data Dump ------------------\n");
    debugPrintParsedData(def);
    Cout << STR("------------------------------------------------------\n");
  } else {
      // Create a build msg.
      Str message = "Couldn't find module: ";
      message += name->getText();
      ParsingMetadataHolder *metadata = item->getInterface<ParsingMetadataHolder>();
      if (metadata != 0) {
        state->addBuildMsg(std::make_shared<Processing::CustomBuildMsg>(message.c_str(),
          metadata->getLine(), metadata->getColumn()));
      } else {
        state->addBuildMsg(std::make_shared<Processing::CustomBuildMsg>(message.c_str(),
          -1, -1));
      }
  }
  // Reset parsed data because we are done with the command.
  state->setData(SharedPtr<IdentifiableObject>(0));
}

} // namespace
