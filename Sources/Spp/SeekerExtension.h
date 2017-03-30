/**
 * @file Spp/SeekerExtension.h
 * Contains the header of class Spp::SeekerExtension.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_SEEKEREXTENSION_H
#define SPP_SEEKEREXTENSION_H

namespace Spp
{

class SeekerExtension : public ObjTiInterface
{
  //============================================================================
  // Type Info

  OBJ_INTERFACE_INFO(SeekerExtension, ObjTiInterface, "Spp", "Spp", "alusus.net");


  //============================================================================
  // Types

  public: struct Overrides
  {
    TiFunctionBase *foreachRef;
    TiFunctionBase *foreachByParamPassRef;
    TiFunctionBase *foreachByParamPass_routingRef;
    TiFunctionBase *foreachByParamPass_templateRef;
  };


  //============================================================================
  // Member Variables

  private: TiObject *owner;


  //============================================================================
  // Constructor

  public: SeekerExtension(TiObject *o) : owner(o)
  {
    initBindingCaches(this->owner, {
      &this->foreachByParamPass,
      &this->foreachByParamPass_routing,
      &this->foreachByParamPass_template
    });
  }


  //============================================================================
  // Member Functions

  /// @name ObjTiInterface Implementation
  /// @{

  public: virtual TiObject* getTiObject()
  {
    return this->owner;
  }

  public: virtual TiObject const* getTiObject() const
  {
    return this->owner;
  }

  /// @}

  /// @name Setup Functions
  /// @{

  public: static Overrides* extend(Core::Data::Seeker *seeker);
  public: static void unextend(Core::Data::Seeker *seeker, Overrides *overrides);

  /// @}

  /// @name Seek Functions
  /// @{


  public: METHOD_BINDING_CACHE(foreachByParamPass,
    void, (Data::Ast::ParamPass const*, TiObject*, Core::Data::Seeker::SeekForeachCallback)
  );
  public: METHOD_BINDING_CACHE(foreachByParamPass_routing,
    Core::Data::Seeker::SeekVerb, (Data::Ast::ParamPass const*, TiObject *, Core::Data::Seeker::SeekForeachCallback)
  );
  public: METHOD_BINDING_CACHE(foreachByParamPass_template,
    Core::Data::Seeker::SeekVerb, (TiObject*, Spp::Ast::Template*, Core::Data::Seeker::SeekForeachCallback)
  );

  private: static void _foreach(TiFunctionBase *base, TiObject *self,
                                TiObject const *ref, TiObject *target, Core::Data::Seeker::SeekForeachCallback cb);

  private: static void _foreachByParamPass(TiObject *self,
                                           Data::Ast::ParamPass const *paramPass, TiObject *data,
                                           Core::Data::Seeker::SeekForeachCallback cb);
  private: static Core::Data::Seeker::SeekVerb _foreachByParamPass_routing(TiObject *self,
                                                                           Data::Ast::ParamPass const *paramPass,
                                                                           TiObject *data,
                                                                           Core::Data::Seeker::SeekForeachCallback cb);
  private: static Core::Data::Seeker::SeekVerb _foreachByParamPass_template(TiObject *self,
                                                                            TiObject *param,
                                                                            Spp::Ast::Template *tmplt,
                                                                            Core::Data::Seeker::SeekForeachCallback cb);

  /// @}

}; // class

} // namespace

#endif
