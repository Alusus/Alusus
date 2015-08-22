/**
 * @file Scg/LibraryGateway.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __LibraryGateway_h__
#define __LibraryGateway_h__

// Alusus Core header files
#include <core.h>

#include <ParsingHandlers/ModuleParsingHandler.h>
#include <ParsingHandlers/BuildParsingHandler.h>
#include <ParsingHandlers/RunParsingHandler.h>
#include <ParsingHandlers/DumpParsingHandler.h>
#include <ParsingHandlers/DefParsingHandler.h>

namespace Scg
{

class LibraryGateway : public Core::Standard::LibraryGateway
{
  //============================================================================
  // Type Info

  TYPE_INFO(LibraryGateway, Core::Standard::LibraryGateway, "Scg", "Scg", "alusus.net");


  //============================================================================
  // Member Variables

private:
  SharedPtr<ModuleParsingHandler> moduleHandler;
private:
  SharedPtr<BuildParsingHandler> buildHandler;
private:
  SharedPtr<RunParsingHandler> runHandler;
private:
  SharedPtr<DumpParsingHandler> dumpHandler;
private:
  SharedPtr<DefParsingHandler> defHandler;
private:
  SharedPtr<Core::Processing::GenericParsingHandler> handler;

private:
  Core::Basic::Int leadingCmdListPos;
private:
  Core::Basic::Int innerCmdListPos;
private:
  Core::Basic::Int tildeCmdListPos;


  //============================================================================
  // Constructor

public:
  LibraryGateway() : leadingCmdListPos(-1), innerCmdListPos(-1), tildeCmdListPos(-1)
  {
  }

public:
  virtual ~LibraryGateway()
  {
  }


  //============================================================================
  // Member Functions

public:
  virtual void initialize(Core::Standard::RootManager *manager);

public:
  virtual void uninitialize(Core::Standard::RootManager *manager);

private:
  Core::Data::SharedList* getLeadingCommandsList(Core::Data::GrammarRepository *grammarRepository);

private:
  Core::Data::SharedList* getInnerCommandsList(Core::Data::GrammarRepository *grammarRepository);

private:
  Core::Data::SharedList* getTildeCommandsList(Core::Data::GrammarRepository *grammarRepository);

}; // class

} // namespace

#endif

