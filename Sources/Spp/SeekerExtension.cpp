/**
 * @file Spp/SeekerExtension.cpp
 * Contains the implementation of class Spp::SeekerExtension.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp
{

using namespace std::placeholders;

//==============================================================================
// Initialization Functions

SeekerExtension* SeekerExtension::extend(Core::Data::Seeker *seeker)
{
  auto extension = new SeekerExtension();

  extension->foreachByParamPass.connect(seeker->getBindingMap());
  extension->foreachByParamPass_routing.connect(seeker->getBindingMap());
  extension->foreachByParamPass_template.connect(seeker->getBindingMap());

  extension->foreachRef = seeker->setFunction(
    seeker->foreach,
    std::function<
      void(TiFunctionBase*, Bindings*, TiObject const*, TiObject*, Core::Data::Seeker::SeekForeachCallback)
    >(std::bind(&SeekerExtension::_foreach, extension, _1, _2, _3, _4, _5))
  ).get();
  extension->foreachByParamPassRef = seeker->setFunction(
    extension->foreachByParamPass,
    std::function<
      void(Bindings*, Data::Ast::ParamPass const*, TiObject*, Core::Data::Seeker::SeekForeachCallback cb)
    >(std::bind(&SeekerExtension::_foreachByParamPass, extension, _1, _2, _3, _4))
  ).get();
  extension->foreachByParamPass_routingRef = seeker->setFunction(
    extension->foreachByParamPass_routing,
    std::function<
      Core::Data::Seeker::SeekVerb(Bindings*,
                                   Data::Ast::ParamPass const*,
                                   TiObject*,
                                   Core::Data::Seeker::SeekForeachCallback)
    >(std::bind(&SeekerExtension::_foreachByParamPass_routing, extension, _1, _2, _3, _4))
  ).get();
  extension->foreachByParamPass_templateRef = seeker->setFunction(
    extension->foreachByParamPass_template,
    std::function<
      Core::Data::Seeker::SeekVerb(Bindings*, TiObject*, Ast::Template*, Core::Data::Seeker::SeekForeachCallback)
    >(std::bind(&SeekerExtension::_foreachByParamPass_template, extension, _1, _2, _3, _4))
  ).get();

  return extension;
}


void SeekerExtension::unextend(Core::Data::Seeker *seeker, SeekerExtension *extension)
{
  seeker->resetFunction(seeker->foreach, extension->foreachRef);
  seeker->resetFunction(extension->foreachByParamPass, extension->foreachByParamPassRef);
  seeker->resetFunction(extension->foreachByParamPass_routing, extension->foreachByParamPass_routingRef);
  seeker->resetFunction(extension->foreachByParamPass_template, extension->foreachByParamPass_templateRef);
  delete extension;
}


//==============================================================================
// Seek Functions

void SeekerExtension::_foreach(SeekerExtension *extension, TiFunctionBase *base, Bindings *_self,
                               TiObject const *ref, TiObject *target, Core::Data::Seeker::SeekForeachCallback cb)
{
  PREPARE_SELF(Core::Data::Seeker);
  if (ref->isA<Data::Ast::ParamPass>()) {
    self->call<void>(extension->foreachByParamPass, static_cast<Data::Ast::ParamPass const*>(ref), target, cb);
  } else {
    self->call<void>(base, ref, target, cb);
  }
}


void SeekerExtension::_foreachByParamPass(SeekerExtension *extension, Bindings *_self,
                                          Data::Ast::ParamPass const *paramPass, TiObject *data,
                                          Core::Data::Seeker::SeekForeachCallback cb)
{
  PREPARE_SELF(Core::Data::Seeker);
  auto operand = paramPass->getOperand().get();
  self->doForeach(operand, data,
    [=](TiObject *newData)->Core::Data::Seeker::SeekVerb
    {
      return self->call<Core::Data::Seeker::SeekVerb>(extension->foreachByParamPass_routing, paramPass, newData, cb);
    }
  );
}


Core::Data::Seeker::SeekVerb SeekerExtension::_foreachByParamPass_routing(SeekerExtension *extension, Bindings *_self,
                                                                          Data::Ast::ParamPass const *paramPass,
                                                                          TiObject *data,
                                                                          Core::Data::Seeker::SeekForeachCallback cb)
{
  PREPARE_SELF(Core::Data::Seeker);
  if (paramPass->getType() == Core::Data::Ast::BracketType::SQUARE) {
    auto param = paramPass->getParam().get();
    if (data->isDerivedFrom<Ast::Template>()) {
      return self->call<Core::Data::Seeker::SeekVerb>(
        extension->foreachByParamPass_template, param, static_cast<Ast::Template*>(data), cb
      );
    } else {
      throw EXCEPTION(InvalidArgumentException, STR("data"), STR("Unrecognized target data type."));
    }
  } else {
    throw EXCEPTION(InvalidArgumentException, STR("paramPass"), STR("Invalid bracket type."), paramPass->getType());
  }
}


Core::Data::Seeker::SeekVerb SeekerExtension::_foreachByParamPass_template(SeekerExtension *extension, Bindings *_self,
                                                                           TiObject *param, Ast::Template *tmplt,
                                                                           Core::Data::Seeker::SeekForeachCallback cb)
{
  PREPARE_SELF(Core::Data::Seeker);
  auto instance = tmplt->getInstance(param, self).get();
  return cb(instance);
}

} // namespace
