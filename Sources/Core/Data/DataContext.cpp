/**
 * @file Core/Data/DataContext.cpp
 * Contains the implementation of class Core::Data::DataContext.
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

void DataContext::switchCurrentModule(Module *m)
{
  if (m == this->currentModule) return;
  // Validation.
  if (m == 0) {
    throw InvalidArgumentException(STR("m"), STR("Core::Data::DataContext::switchCurrentModule"),
                                   STR("Cannot be null."));
  }
  if (this->rootModule == 0) {
    throw GeneralException(STR("Root module must be set before calling this function."),
                           STR("Core::Data::DataContext::switchCurrentModule"));
  }
  // Switch the modules.
  //Module *oldm = this->currentModule;
  this->currentModule = m;

  // Trigger signal.
  //this->moduleSwitchNotifier.emit(oldm, this->currentModule);
}


//==============================================================================
// Data Access Functions

IdentifiableObject* DataContext::getStartingParent(ReferenceScope scope) const
{
  if (scope == ReferenceScope::ARGS) {
    if (this->currentArgumentList == 0) {
      throw GeneralException(STR("No argument list currently set."),
                             STR("Core::Data::DataContext::getStartingParent"));
    }
    return this->currentArgumentList;
  } else if (scope == ReferenceScope::STACK) {
    if (this->variableStack == 0) {
      throw GeneralException(STR("No variable stack currently set."),
                             STR("Core::Data::DataContext::getStartingParent"));
    }
    return this->variableStack;
  } else if (scope == ReferenceScope::MODULE || scope == ReferenceScope::UNKNOWN) {
    if (this->currentModule == 0) {
      throw GeneralException(STR("No module is provided for module reference."),
                             STR("Core::Data::DataContext::getStartingParent"));
    }
    return this->currentModule;
  } else if (scope == ReferenceScope::PMODULE) {
    if (this->currentModule == 0 || !this->currentModule->isDerivedFrom<GrammarModule>() ||
        static_cast<GrammarModule*>(this->currentModule)->getParent() == 0) {
      throw GeneralException(STR("No parent module is provided for pmodule reference."),
                             STR("Core::Data::DataContext::getStartingParent"));
    }
    return static_cast<GrammarModule*>(this->currentModule)->getParent();
  } else if (scope == ReferenceScope::ROOT) {
    if (this->rootModule == 0) {
      throw GeneralException(STR("Root module is not yet set."),
                             STR("Core::Data::DataContext::getStartingParent"));
    }
    return this->rootModule;
  } else {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::DataContext::getStartingParent"),
                                   STR("Uknown reference type."), scope.val);
  }
}


IdentifiableObject* DataContext::getStartingParent(const Char *&qualifier) const
{
  if (qualifier == 0) {
    throw InvalidArgumentException(STR("qualifier"),
                                   STR("Core::Data::DataContext::getStartingParent"),
                                   STR("Should not be 0."));
  }
  return this->getStartingParent(ReferenceParser::parseQualifierScope(qualifier));
}


IdentifiableObject* DataContext::tryGetStartingParent(ReferenceScope scope) const
{
  if (scope == ReferenceScope::ARGS) {
    return this->currentArgumentList;
  } else if (scope == ReferenceScope::STACK) {
    return this->variableStack;
  } else if (scope == ReferenceScope::MODULE || scope == ReferenceScope::UNKNOWN) {
    return this->currentModule;
  } else if (scope == ReferenceScope::PMODULE) {
    if (this->currentModule == 0 || !this->currentModule->isDerivedFrom<GrammarModule>() ||
        static_cast<GrammarModule*>(this->currentModule)->getParent() == 0) {
      return 0;
    }
    return static_cast<GrammarModule*>(this->currentModule)->getParent();
  } else if (scope == ReferenceScope::ROOT) {
    return this->rootModule;
  } else {
    return 0;
  }
}


IdentifiableObject* DataContext::tryGetStartingParent(const Char *&qualifier) const
{
  if (qualifier == 0) {
    throw InvalidArgumentException(STR("qualifier"),
                                   STR("Core::Data::DataContext::tryGetStartingParent"),
                                   STR("Should not be 0."));
  }
  return this->tryGetStartingParent(ReferenceParser::parseQualifierScope(qualifier));
}

} } // namespace
