/**
 * @file Spp/CodeGen/AstProcessor.cpp
 * Contains the implementation of class Spp::CodeGen::AstProcessor.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::CodeGen
{

//==============================================================================
// Initialization Functions

void AstProcessor::initBindingCaches()
{
  Basic::initBindingCaches(this, {
    &this->runPass,
    &this->process,
    &this->processMemberFunction,
    &this->processMacro,
    &this->applyMacroArgs,
    &this->applyMacroArgsIteration,
    &this->applyMacroArgsIteration_identifier,
    &this->applyMacroArgsIteration_stringLiteral,
    &this->applyMacroArgsIteration_tiStr,
    &this->applyMacroArgsIteration_other,
    &this->applyMacroArgsIteration_binding,
    &this->applyMacroArgsIteration_containing,
    &this->applyMacroArgsIteration_dynContaining,
    &this->applyMacroArgsIteration_dynMapContaining
  });
}


void AstProcessor::initBindings()
{
  this->runPass = &AstProcessor::_runPass;
  this->process = &AstProcessor::_process;
  this->processMemberFunction = &AstProcessor::_processMemberFunction;
  this->processMacro = &AstProcessor::_processMacro;
  this->applyMacroArgs = &AstProcessor::_applyMacroArgs;
  this->applyMacroArgsIteration = &AstProcessor::_applyMacroArgsIteration;
  this->applyMacroArgsIteration_identifier = &AstProcessor::_applyMacroArgsIteration_identifier;
  this->applyMacroArgsIteration_stringLiteral = &AstProcessor::_applyMacroArgsIteration_stringLiteral;
  this->applyMacroArgsIteration_tiStr = &AstProcessor::_applyMacroArgsIteration_tiStr;
  this->applyMacroArgsIteration_other = &AstProcessor::_applyMacroArgsIteration_other;
  this->applyMacroArgsIteration_binding = &AstProcessor::_applyMacroArgsIteration_binding;
  this->applyMacroArgsIteration_containing = &AstProcessor::_applyMacroArgsIteration_containing;
  this->applyMacroArgsIteration_dynContaining = &AstProcessor::_applyMacroArgsIteration_dynContaining;
  this->applyMacroArgsIteration_dynMapContaining = &AstProcessor::_applyMacroArgsIteration_dynMapContaining;
}


//==============================================================================
// Main Functions

void AstProcessor::preparePass(Core::Notices::Store *noticeStore)
{
  this->setNoticeStore(noticeStore);
  this->getAstHelper()->prepare(noticeStore);
}


Bool AstProcessor::_runPass(TiObject *self, Core::Data::Ast::Scope *root)
{
  PREPARE_SELF(astProcessor, AstProcessor);
  VALIDATE_NOT_NULL(root);

  return astProcessor->process(root);
}


Bool AstProcessor::_process(TiObject *self, TiObject *owner)
{
  PREPARE_SELF(astProcessor, AstProcessor);
  VALIDATE_NOT_NULL(owner);

  if (owner == 0 || owner->isDerivedFrom<Core::Data::Grammar::Module>()) return true;

  auto container = ti_cast<Containing<TiObject>>(owner);
  if (container == 0) return true;

  Bool result = true;
  for (Int i = 0; i < container->getElementCount(); ++i) {
    auto child = container->getElement(i);
    if (child == 0) continue;
    if (child->isDerivedFrom<Core::Data::Ast::ParamPass>()) {
      auto paramPass = static_cast<Core::Data::Ast::ParamPass*>(child);
      if (paramPass->getType() == Core::Data::Ast::BracketType::SQUARE) {
        // Extract args.
        PlainList<TiObject> argsList;
        auto param = paramPass->getParam().get();
        Containing<TiObject> *args = &argsList;
        if (param) {
          if (param->isDerivedFrom<Core::Data::Ast::List>()) {
            args = static_cast<Core::Data::Ast::List*>(param);
          } else {
            argsList.add(param);
          }
        }

        // Find matching macro.
        Ast::Macro *macro = 0;
        astProcessor->astHelper->getSeeker()->foreach(paramPass->getOperand().get(), owner,
          [=, &macro] (TiObject *obj, Core::Notices::Notice *ntc)->Core::Data::Seeker::Verb
          {
            if (ntc != 0) {
              return Core::Data::Seeker::Verb::MOVE;
            }

            auto m = ti_cast<Ast::Macro>(obj);
            if (m != 0 && m->matchCall(args, astProcessor->astHelper)) {
              macro = m;
              return Core::Data::Seeker::Verb::STOP;
            } else {
              return Core::Data::Seeker::Verb::MOVE;
            }
          }, 0
        );

        if (macro != 0) {
          auto sl = paramPass->findSourceLocation();
          astProcessor->noticeStore->pushPrefixSourceLocation(sl.get());
          if (astProcessor->processMacro(macro, args, owner, i, sl.get())) --i;
          else result = false;
          astProcessor->noticeStore->popPrefixSourceLocation(
            Core::Data::getSourceLocationRecordCount(sl.get())
          );
        }
        continue;
      }
    } else if (child->isDerivedFrom<Ast::Function>()) {
      if (astProcessor->astHelper->getDefinitionDomain(child) == Ast::DefinitionDomain::OBJECT) {
        if (!astProcessor->processMemberFunction(static_cast<Ast::Function*>(child))) result = false;
      }
    }
    if (!astProcessor->process(child)) result = false;
  }
  return result;
}


Bool AstProcessor::_processMemberFunction(TiObject *self, Spp::Ast::Function *func)
{
  PREPARE_SELF(astProcessor, AstProcessor);
  auto def = ti_cast<Core::Data::Ast::Definition>(func->getOwner());
  if (def == 0) {
    throw EXCEPTION(GenericException, S("Could not find definition of a member function."));
  }
  auto funcType = func->getType().get();
  if (funcType == 0) {
    throw EXCEPTION(GenericException, S("Function is missing a function type."));
  }
  if (funcType->getArgTypes() == 0) {
    funcType->setArgTypes(Core::Data::Ast::Map::create());
  }
  // Make sure we don't have an arg named `this`.
  if (funcType->getArgTypes()->findIndex(S("this")) != -1) {
    astProcessor->noticeStore->add(
      std::make_shared<Spp::Notices::ThisRedefinedNotice>(func->findSourceLocation())
    );
    return false;
  }
  // Prepare this type.
  auto thisType = Core::Data::Ast::ParamPass::create({
    {S("sourceLocation"), func->findSourceLocation()},
    {S("type"), Core::Data::Ast::BracketType(Core::Data::Ast::BracketType::SQUARE)}
  }, {
    {S("operand"), Core::Data::Ast::Identifier::create({
      {S("sourceLocation"), func->findSourceLocation()},
      {S("value"), TiStr(S("ref"))}
    })},
    {S("param"), Spp::Ast::ThisTypeRef::create()}
  });
  // Add this arg.
  funcType->getArgTypes()->insert(0, S("this"), thisType);
  // Mark the function as shared.
  def->addModifier(Core::Data::Ast::Identifier::create({
    {S("value"), TiStr(S("shared"))}
  }));
  return true;
}


Bool AstProcessor::_processMacro(
  TiObject *self, Spp::Ast::Macro *macro, Containing<TiObject> *args, TiObject *owner, TiInt indexInOwner,
  Core::Data::SourceLocation *sl
) {
  PREPARE_SELF(astProcessor, AstProcessor);
  VALIDATE_NOT_NULL(macro, args, owner);

  // Apply macro args.
  TioSharedPtr macroInstance;
  if (!astProcessor->applyMacroArgs(macro, args, sl, macroInstance)) return false;
  if (macroInstance == 0) {
    astProcessor->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidMacroNotice>(macro->findSourceLocation())
    );
    return false;
  }
  if (macroInstance->isDerivedFrom<Core::Data::Ast::Scope>()) {
    auto scope = macroInstance.s_cast_get<Core::Data::Ast::Scope>();
    if (scope->getCount() == 1) {
      auto child = scope->get(0);
      macroInstance = child;
    }
  }

  // Replace macro reference with the clone.
  if (owner->isDerivedFrom<Core::Data::Ast::Scope>() && macroInstance->isDerivedFrom<Core::Data::Ast::Scope>()) {
    // Merge the two scopes.
    auto ownerScope = static_cast<Core::Data::Ast::Scope*>(owner);
    auto instanceScope = macroInstance.s_cast_get<Core::Data::Ast::Scope>();
    ownerScope->remove(indexInOwner);
    for (Int i = 0; i < instanceScope->getCount(); ++i) {
      ownerScope->insert(indexInOwner + i, instanceScope->get(i));
    }
  } else {
    // Replace the instance.
    auto ownerContainer = ti_cast<Containing<TiObject>>(owner);
    if (ownerContainer == 0) {
      throw EXCEPTION(InvalidArgumentException, S("owner"), S("Invalid owner type."));
    }
    ownerContainer->setElement(indexInOwner, macroInstance.get());
  }

  return true;
}


Bool AstProcessor::_applyMacroArgs(
  TiObject *self, Spp::Ast::Macro *macro, Containing<TiObject> *args, Core::Data::SourceLocation *sl,
  TioSharedPtr &result
) {
  PREPARE_SELF(astProcessor, AstProcessor);
  VALIDATE_NOT_NULL(macro, args);

  if (macro->getArgCount() != args->getElementCount()) {
    throw EXCEPTION(GenericException, S("Invalid args passed to macro."));
  }

  return astProcessor->applyMacroArgsIteration(macro->getBody().get(), macro->getArgTypes().get(), args, sl, result);
}


Bool AstProcessor::_applyMacroArgsIteration(
  TiObject *self, TiObject *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
  Core::Data::SourceLocation *sl, TioSharedPtr &result
) {
  PREPARE_SELF(astProcessor, AstProcessor);

  if (obj->isDerivedFrom<Core::Data::Ast::Identifier>()) {
    auto identifier = static_cast<Core::Data::Ast::Identifier*>(obj);
    return astProcessor->applyMacroArgsIteration_identifier(identifier, argTypes, args, sl, result);
  }

  if (obj->isDerivedFrom<Core::Data::Ast::StringLiteral>()) {
    auto stringLiteral = static_cast<Core::Data::Ast::StringLiteral*>(obj);
    return astProcessor->applyMacroArgsIteration_stringLiteral(stringLiteral, argTypes, args, sl, result);
  }

  if (obj->isDerivedFrom<TiStr>()) {
    auto str = static_cast<TiStr*>(obj);
    return astProcessor->applyMacroArgsIteration_tiStr(str, argTypes, args, sl, result);
  }

  // It's not a replacable identifier, so we'll proceed with cloning the tree.
  return astProcessor->applyMacroArgsIteration_other(obj, argTypes, args, sl, result);
}


Bool AstProcessor::_applyMacroArgsIteration_identifier(
  TiObject *self, Core::Data::Ast::Identifier *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
  Core::Data::SourceLocation *sl, TioSharedPtr &result
) {
  PREPARE_SELF(astProcessor, AstProcessor);
  Char var[1000];
  Word prefixSize = 0;
  Char const *suffix = 0;
  astProcessor->parseStringTemplate(obj->getValue().get(), var, 1000, prefixSize, suffix);
  auto index = argTypes->findIndex(var);
  if (index != -1) {
    auto arg = args->getElement(index);
    if (prefixSize != 0 || suffix != 0) {
      // We have an identifier string template, so we need the matching arg to be an identifier as well.
      if (arg->isDerivedFrom<Core::Data::Ast::Identifier>()) {
        Char newVar[1000];
        astProcessor->generateStringFromTemplate(
          obj->getValue().get(), prefixSize, static_cast<Core::Data::Ast::Identifier*>(arg)->getValue().get(), suffix,
          newVar, 1000
        );
        result = Core::Data::Ast::Identifier::create({
          { S("value"), TiStr(newVar) },
          { S("sourceLocation"), obj->getSourceLocation() }
        });
        Core::Data::Ast::addSourceLocation(result.get(), sl);
      } else {
        astProcessor->noticeStore->add(
          std::make_shared<Spp::Notices::InvalidMacroArgNotice>(Core::Data::Ast::findSourceLocation(arg))
        );
        return false;
      }
    } else {
      // We don't have an identifier string template, so we'll just copy the arg as is.
      result = Core::Data::Ast::clone(getSharedPtr(args->getElement(index)), sl);
    }
  } else {
    result = Core::Data::Ast::clone(getSharedPtr(obj), sl);
  }
  return true;
}


Bool AstProcessor::_applyMacroArgsIteration_stringLiteral(
  TiObject *self, Core::Data::Ast::StringLiteral *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
  Core::Data::SourceLocation *sl, TioSharedPtr &result
) {
  PREPARE_SELF(astProcessor, AstProcessor);
  Char var[1000];
  Word prefixSize = 0;
  Char const *suffix = 0;
  astProcessor->parseStringTemplate(obj->getValue().get(), var, 1000, prefixSize, suffix, S("{{"), S("}}"));
  auto index = argTypes->findIndex(var);
  if (index != -1) {
    auto arg = args->getElement(index);
    if (prefixSize != 0 || suffix != 0) {
      // We have an identifier string template, so we need the matching arg to be an identifier as well.
      if (arg->isDerivedFrom<Core::Data::Ast::Identifier>() || arg->isDerivedFrom<Core::Data::Ast::StringLiteral>()) {
        Char newVar[1000];
        astProcessor->generateStringFromTemplate(
          obj->getValue().get(), prefixSize, static_cast<Core::Data::Ast::Text*>(arg)->getValue().get(), suffix,
          newVar, 1000
        );
        result = Core::Data::Ast::StringLiteral::create({
          { S("value"), TiStr(newVar) },
          { S("sourceLocation"), obj->getSourceLocation() }
        });
        Core::Data::Ast::addSourceLocation(result.get(), sl);
      } else {
        astProcessor->noticeStore->add(
          std::make_shared<Spp::Notices::InvalidMacroArgNotice>(Core::Data::Ast::findSourceLocation(arg))
        );
        return false;
      }
    } else {
      // We don't have an identifier string template, so we'll just copy the arg as is.
      result = Core::Data::Ast::clone(getSharedPtr(args->getElement(index)), sl);
    }
  } else {
    result = Core::Data::Ast::clone(getSharedPtr(obj), sl);
  }
  return true;
}


Bool AstProcessor::_applyMacroArgsIteration_tiStr(
  TiObject *self, TiStr *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
  Core::Data::SourceLocation *sl, TioSharedPtr &result
) {
  PREPARE_SELF(astProcessor, AstProcessor);
  Char var[1000];
  Word prefixSize = 0;
  Char const *suffix = 0;
  astProcessor->parseStringTemplate(obj->get(), var, 1000, prefixSize, suffix);
  auto index = argTypes->findIndex(var);
  if (index != -1) {
    auto arg = ti_cast<Core::Data::Ast::Identifier>(args->getElement(index));
    if (arg != 0) {
      Char newVar[1000];
      astProcessor->generateStringFromTemplate(obj->get(), prefixSize, arg->getValue().get(), suffix, newVar, 1000);
      result = TiStr::create(newVar);
    } else {
      astProcessor->noticeStore->add(
        std::make_shared<Spp::Notices::InvalidMacroArgNotice>(Core::Data::Ast::findSourceLocation(arg))
      );
      return false;
    }
  } else {
    result = TiStr::create(obj->get());
  }
  return true;
}


Bool AstProcessor::_applyMacroArgsIteration_other(
  TiObject *self, TiObject *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
  Core::Data::SourceLocation *sl, TioSharedPtr &result
) {
  PREPARE_SELF(astProcessor, AstProcessor);

  if (!obj->isDerivedFrom<Core::Data::Node>()) {
    // We won't clone basic data types.
    result = getSharedPtr(obj);
    return true;
  }

  auto factory = obj->getMyTypeInfo()->getFactory();
  if (!factory) {
    throw EXCEPTION(GenericException, S("A Node derived class is missing a type factory."));
  }
  result = factory->createShared();
  if (sl != 0) Core::Data::Ast::addSourceLocation(result.get(), sl);

  auto srcBinding = ti_cast<Binding>(obj);
  auto binding = result.ti_cast_get<Binding>();
  if (binding != 0) {
    if (!astProcessor->applyMacroArgsIteration_binding(srcBinding, argTypes, args, sl, binding)) return false;
  }

  auto srcDynMapContainer = ti_cast<DynamicMapContaining<TiObject>>(obj);
  auto dynMapContainer = result.ti_cast_get<DynamicMapContaining<TiObject>>();
  if (dynMapContainer != 0) {
    if (!astProcessor->applyMacroArgsIteration_dynMapContaining(
      srcDynMapContainer, argTypes, args, sl, dynMapContainer
    )) return false;
  }

  auto srcDynContainer = ti_cast<DynamicContaining<TiObject>>(obj);
  auto dynContainer = result.ti_cast_get<DynamicContaining<TiObject>>();
  if (dynContainer != 0) {
    if (!astProcessor->applyMacroArgsIteration_dynContaining(
      srcDynContainer, argTypes, args, sl, dynContainer
    )) return false;
  }

  auto srcContainer = ti_cast<Containing<TiObject>>(obj);
  auto container = result.ti_cast_get<Containing<TiObject>>();
  if (dynMapContainer == 0 && dynContainer == 0 && container != 0) {
    if (!astProcessor->applyMacroArgsIteration_containing(
      srcContainer, argTypes, args, sl, container
    )) return false;
  }

  return true;
}


Bool AstProcessor::_applyMacroArgsIteration_binding(
  TiObject *self, Binding *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
  Core::Data::SourceLocation *sl, Binding *destObj
) {
  PREPARE_SELF(astProcessor, AstProcessor);
  for (Int i = 0; i < obj->getMemberCount(); ++i) {
    if (obj->getMemberHoldMode(i) == HoldMode::VALUE && obj->getMemberNeededType(i) == TiStr::getTypeInfo()) {
      // replace string, if possible.
      Char var[1000];
      Word prefixSize = 0;
      Char const *suffix = 0;
      astProcessor->parseStringTemplate(obj->refMember<TiStr>(i).get(), var, 1000, prefixSize, suffix);
      Int index = argTypes->findElementIndex(var);
      if (index != -1) {
        auto identifier = ti_cast<Core::Data::Ast::Identifier>(args->getElement(index));
        if (identifier == 0) {
          auto elementSl = Core::Data::Ast::findSourceLocation(args->getElement(index));
          astProcessor->noticeStore->add(
            std::make_shared<Spp::Notices::InvalidMacroArgNotice>(elementSl == 0 ? getSharedPtr(sl) : elementSl)
          );
          return false;
        }
        Char newVar[1000];
        astProcessor->generateStringFromTemplate(
          obj->refMember<TiStr>(i).get(), prefixSize, identifier->getValue().get(), suffix, newVar, 1000
        );
        destObj->refMember<TiStr>(i) = newVar;
      } else {
        destObj->setMember(i, obj->getMember(i));
      }
    } else if (obj->getMemberHoldMode(i) == HoldMode::SHARED_REF) {
      // Clone the member.
      TioSharedPtr newChild;
      TiObject *child = obj->getMember(i);
      if (child != 0) {
        if (!astProcessor->applyMacroArgsIteration(child, argTypes, args, sl, newChild)) return false;
        destObj->setMember(i, newChild.get());
      }
    } else {
      destObj->setMember(i, obj->getMember(i));
    }
  }
  return true;
}


Bool AstProcessor::_applyMacroArgsIteration_containing(
  TiObject *self, Containing<TiObject> *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
  Core::Data::SourceLocation *sl, Containing<TiObject> *destObj
) {
  PREPARE_SELF(astProcessor, AstProcessor);
  for (Int i = 0; i < obj->getElementCount(); ++i) {
    if (obj->getElementHoldMode(i) == HoldMode::SHARED_REF) {
      TioSharedPtr newChild;
      TiObject *child = obj->getElement(i);
      if (child != 0) {
        if (!astProcessor->applyMacroArgsIteration(child, argTypes, args, sl, newChild)) return false;
      }
      destObj->setElement(i, newChild.get());
    } else {
      destObj->setElement(i, obj->getElement(i));
    }
  }
  return true;
}


Bool AstProcessor::_applyMacroArgsIteration_dynContaining(
  TiObject *self, DynamicContaining<TiObject> *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
  Core::Data::SourceLocation *sl, DynamicContaining<TiObject> *destObj
) {
  PREPARE_SELF(astProcessor, AstProcessor);
  for (Int i = 0; i < obj->getElementCount(); ++i) {
    if (obj->getElementHoldMode(i) == HoldMode::SHARED_REF) {
      TioSharedPtr newChild;
      TiObject *child = obj->getElement(i);
      if (child != 0) {
        if (!astProcessor->applyMacroArgsIteration(child, argTypes, args, sl, newChild)) return false;
      }
      destObj->addElement(newChild.get());
    } else {
      destObj->addElement(obj->getElement(i));
    }
  }
  return true;
}


Bool AstProcessor::_applyMacroArgsIteration_dynMapContaining(
  TiObject *self, DynamicMapContaining<TiObject> *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
  Core::Data::SourceLocation *sl, DynamicMapContaining<TiObject> *destObj
) {
  PREPARE_SELF(astProcessor, AstProcessor);
  for (Int i = 0; i < obj->getElementCount(); ++i) {
    // Generate the new key value.
    Char key[1000];
    Char newKey[1000];
    Word prefixSize = 0;
    Char const *suffix = 0;
    astProcessor->parseStringTemplate(obj->getElementKey(i).c_str(), key, 1000, prefixSize, suffix);
    auto index = argTypes->findElementIndex(key);
    if (index != -1) {
      // The key can be replaced with the new key.
      auto arg = ti_cast<Core::Data::Ast::Identifier>(args->getElement(index));
      if (arg != 0) {
        astProcessor->generateStringFromTemplate(
          obj->getElementKey(i).c_str(), prefixSize, arg->getValue().get(), suffix, newKey, 1000
        );
        // Make sure the new key isn't already used.
        if (obj->findElementIndex(newKey) != -1) {
          astProcessor->noticeStore->add(
            std::make_shared<Spp::Notices::InvalidMacroArgNotice>(arg->findSourceLocation())
          );
          return false;
        }
        auto value = obj->getElement(index);
        obj->insertElement(index, newKey, value);
        obj->removeElement(index + 1);
      } else {
        astProcessor->noticeStore->add(
          std::make_shared<Spp::Notices::InvalidMacroArgNotice>(Core::Data::Ast::findSourceLocation(arg))
        );
        return false;
      }
    } else {
      copyStr(obj->getElementKey(i).c_str(), newKey);
    }
    // Generate the value.
    if (obj->getElementHoldMode(i) == HoldMode::SHARED_REF) {
      TioSharedPtr newChild;
      TiObject *child = obj->getElement(i);
      if (child != 0) {
        if (!astProcessor->applyMacroArgsIteration(child, argTypes, args, sl, newChild)) return false;
      }
      destObj->addElement(newKey, newChild.get());
    } else {
      destObj->addElement(newKey, obj->getElement(i));
    }
  }
  return true;
}


//==============================================================================
// Helper Functions

void AstProcessor::parseStringTemplate(
  Char const *str, Char *var, Word varBufSize, Word &prefixSize, Char const *&suffix,
  Char const *varOpening, Char const *varClosing
) {
  auto varStart = SBSTR(str).findPos(varOpening) + 2;
  if (varStart >= 2) {
    auto varSize = SBSTR(str + varStart).findPos(varClosing);
    if (varSize > 0) {
      SBSTR(var).assign(str + varStart, varSize, varBufSize);
      prefixSize = varStart - 2;
      suffix = str + varStart + varSize + 2;
      return;
    }
  }
  SBSTR(var).assign(str, varBufSize);
  prefixSize = 0;
  suffix = 0;
}


void AstProcessor::generateStringFromTemplate(
  Char const *prefix, Word prefixSize, Char const *var, Char const *suffix, Char *output, Word outputBufSize
) {
  SbStr &result = SBSTR(output);
  result.assign(S(""), outputBufSize);
  if (prefixSize > 0) result.append(prefix, prefixSize, outputBufSize);
  result.append(var, outputBufSize);
  if (suffix != 0) result.append(suffix, outputBufSize);
}

} // namespace
