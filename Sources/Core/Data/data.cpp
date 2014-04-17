/**
 * @file Core/Data/data.cpp
 * Contains the global implementations of Data namespace's declarations.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Data
{

//============================================================================
// Global Functions

/**
 * Call the unsetIndexes method of DataOwner interface, if implemented by
 * the object.
 */
void unsetIndexes(IdentifiableObject *obj, Int from, Int to)
{
  if (obj == 0) {
    throw InvalidArgumentException(STR("obj"), STR("Core::Data::unsetIndexes"),
                                   STR("Obj is null."));
  }
  DataOwner *mt = obj->getInterface<DataOwner>();
  if (mt != 0) mt->unsetIndexes(from, to);
}


Module* findAssociatedLexerModule(Module *module, Provider *provider)
{
  GrammarModule *grammarModule = io_cast<GrammarModule>(module);
  // Check if we need to switch the lexer module.
  Reference *lmr = 0;
  while (lmr == 0 && grammarModule != 0) {
    lmr = grammarModule->getLexerModuleRef().get();
    grammarModule = grammarModule->getParent();
  }
  if (lmr == 0) return 0;
  GrammarHelper helper(provider);
  Module *dummyModule;
  IdentifiableObject *lmo;
  helper.getValue(lmr, grammarModule, lmo, dummyModule);
  Module *lm = io_cast<Module>(lmo);
  if (lm == 0) {
    throw GeneralException(STR("The module has an invalid lexer module reference."),
                           STR("Core::Data::DataContext::findAssociatedLexerModule"));
  }
  return lm;
}

} } // namespace
