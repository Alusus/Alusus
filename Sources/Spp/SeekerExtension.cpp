/**
 * @file Spp/SeekerExtension.cpp
 * Contains the implementation of class Spp::SeekerExtension.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
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

SeekerExtension::Overrides* SeekerExtension::extend(Core::Data::Seeker *seeker)
{
  auto extension = std::make_shared<SeekerExtension>(seeker);
  seeker->addDynamicInterface(extension);

  auto overrides = new Overrides();
  overrides->foreachRef = seeker->foreach.set(&SeekerExtension::_foreach).get();
  overrides->foreachByParamPassRef =
    extension->foreachByParamPass.set(&SeekerExtension::_foreachByParamPass).get();
  overrides->foreachByParamPass_routingRef =
    extension->foreachByParamPass_routing.set(&SeekerExtension::_foreachByParamPass_routing).get();
  overrides->foreachByParamPass_templateRef =
    extension->foreachByParamPass_template.set(&SeekerExtension::_foreachByParamPass_template).get();

  return overrides;
}


void SeekerExtension::unextend(Core::Data::Seeker *seeker, Overrides *overrides)
{
  auto extension = Core::Basic::ti_cast<SeekerExtension>(seeker);
  seeker->foreach.reset(overrides->foreachRef);
  extension->foreachByParamPass.reset(overrides->foreachByParamPassRef);
  extension->foreachByParamPass_routing.reset(overrides->foreachByParamPass_routingRef);
  extension->foreachByParamPass_template.reset(overrides->foreachByParamPass_templateRef);

  seeker->removeDynamicInterface<SeekerExtension>();
  delete overrides;
}


//==============================================================================
// Seek Functions

void SeekerExtension::_foreach(
  TiFunctionBase *base, TiObject *self, TiObject const *ref, TiObject *target,
  Core::Data::Seeker::ForeachCallback const &cb
) {
  if (ref->isA<Data::Ast::ParamPass>()) {
    PREPARE_SELF(seekerExtension, SeekerExtension);
    seekerExtension->foreachByParamPass(static_cast<Data::Ast::ParamPass const*>(ref), target, cb);
  } else {
    PREPARE_SELF(seeker, Core::Data::Seeker);
    seeker->foreach.useCallee(base)(ref, target, cb);
  }
}


void SeekerExtension::_foreachByParamPass(
  TiObject *self, Data::Ast::ParamPass const *paramPass, TiObject *data,
  Core::Data::Seeker::ForeachCallback const &cb
) {
  PREPARE_SELF(seeker, Core::Data::Seeker);
  PREPARE_SELF(seekerExtension, SeekerExtension);
  auto operand = paramPass->getOperand().get();
  seeker->doForeach(operand, data,
    [=](TiObject *newData, Core::Data::Notice*)->Core::Data::Seeker::Verb
    {
      return seekerExtension->foreachByParamPass_routing(paramPass, newData, cb);
    }
  );
}


Core::Data::Seeker::Verb SeekerExtension::_foreachByParamPass_routing(
  TiObject *self, Data::Ast::ParamPass const *paramPass, TiObject *data,
  Core::Data::Seeker::ForeachCallback const &cb
) {
  PREPARE_SELF(seekerExtension, SeekerExtension);
  if (paramPass->getType() == Core::Data::Ast::BracketType::SQUARE) {
    auto param = paramPass->getParam().get();
    if (data->isDerivedFrom<Ast::Template>()) {
      return seekerExtension->foreachByParamPass_template(param, static_cast<Ast::Template*>(data), cb);
    } else {
      throw EXCEPTION(InvalidArgumentException, STR("data"), STR("Unrecognized target data type."));
    }
  } else {
    throw EXCEPTION(InvalidArgumentException, STR("paramPass"), STR("Invalid bracket type."), paramPass->getType());
  }
}


Core::Data::Seeker::Verb SeekerExtension::_foreachByParamPass_template(
  TiObject *self, TiObject *param, Ast::Template *tmplt, Core::Data::Seeker::ForeachCallback const &cb
) {
  PREPARE_SELF(seeker, Core::Data::Seeker);
  auto instance = tmplt->getInstance(param, seeker).get();
  return cb(instance, 0);
}

} // namespace
