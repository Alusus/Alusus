/**
 * @file Core/Data/ParsingGrammarContext.cpp
 * Contains the implementation of class Core::Data::ParsingGrammarContext.
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
// Module Functions

void ParsingGrammarContext::switchCurrentModule(Module *m)
{
  if (m == this->currentModule) return;
  // Validation.
  if (m == 0) {
    throw InvalidArgumentException(STR("m"), STR("Core::Data::ParsingGrammarContext::switchCurrentModule"),
                                   STR("Cannot be null."));
  }
  if (this->rootModule == 0) {
    throw GeneralException(STR("Root module must be set before calling this function."),
                           STR("Core::Data::ParsingGrammarContext::switchCurrentModule"));
  }
  // Switch the modules.
  //Module *oldm = this->currentModule;
  this->currentModule = m;
  // Inform the registered parsers.
  //this->parserModuleSwitchNotifier.emit(oldm, this->currentModule);
}


Module* ParsingGrammarContext::findAssociatedLexerModule(Module *module) const
{
  GrammarModule *grammarModule = io_cast<GrammarModule>(module);
  // Check if we need to switch the lexer module.
  Reference *lmr = 0;
  while (lmr == 0 && grammarModule != 0) {
    lmr = grammarModule->getLexerModuleRef().get();
    grammarModule = grammarModule->getParent();
  }
  if (lmr == 0) return 0;
  GrammarHelper helper(static_cast<const Provider*>(this));
  Module *dummyModule;
  IdentifiableObject *lmo;
  helper.getValue(lmr, grammarModule, lmo, dummyModule);
  Module *lm = io_cast<Module>(lmo);
  if (lm == 0) {
    throw GeneralException(STR("The module has an invalid lexer module reference."),
                           STR("Core::Data::ParsingGrammarContext::findAssociatedLexerModule"));
  }
  return lm;
}


//==============================================================================
// Data Access Functions

IdentifiableObject* ParsingGrammarContext::getStartingParent(ReferenceScope scope) const
{
  if (scope == ReferenceScope::ARGS) {
    if (this->currentArgumentList == 0) {
      throw GeneralException(STR("No argument list currently set."),
                             STR("Core::Data::ParsingGrammarContext::getStartingParent"));
    }
    return this->currentArgumentList;
  } else if (scope == ReferenceScope::STACK) {
    if (this->variableStack == 0) {
      throw GeneralException(STR("No variable stack currently set."),
                             STR("Core::Data::ParsingGrammarContext::getStartingParent"));
    }
    return this->variableStack;
  } else if (scope == ReferenceScope::MODULE || scope == ReferenceScope::UNKNOWN) {
    if (this->currentModule == 0) {
      throw GeneralException(STR("No module is provided for module reference."),
                             STR("Core::Data::ParsingGrammarContext::getStartingParent"));
    }
    return this->currentModule;
  } else if (scope == ReferenceScope::PMODULE) {
    if (this->currentModule == 0 || !this->currentModule->isDerivedFrom<GrammarModule>() ||
        static_cast<GrammarModule*>(this->currentModule)->getParent() == 0) {
      throw GeneralException(STR("No parent module is provided for pmodule reference."),
                             STR("Core::Data::ParsingGrammarContext::getStartingParent"));
    }
    return static_cast<GrammarModule*>(this->currentModule)->getParent();
  } else if (scope == ReferenceScope::ROOT) {
    if (this->rootModule == 0) {
      throw GeneralException(STR("Root module is not yet set."),
                             STR("Core::Data::ParsingGrammarContext::getStartingParent"));
    }
    return this->rootModule;
  } else {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::ParsingGrammarContext::getStartingParent"),
                                   STR("Uknown reference type."), scope.val);
  }
}


IdentifiableObject* ParsingGrammarContext::getStartingParent(const Char *&qualifier) const
{
  if (qualifier == 0) {
    throw InvalidArgumentException(STR("qualifier"),
                                   STR("Core::Data::ParsingGrammarContext::getStartingParent"),
                                   STR("Should not be 0."));
  }
  return this->getStartingParent(ReferenceParser::parseQualifierScope(qualifier));
}

} } // namespace
