/**
 * @file Spp/SeekerExtension.h
 * Contains the header of class Spp::SeekerExtension.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
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

class SeekerExtension
{
  //============================================================================
  // Member Functions

  /// @name Setup Functions
  /// @{

  public: static SeekerExtension* extend(Core::Data::Seeker *seeker);
  public: static void unextend(Core::Data::Seeker *seeker, SeekerExtension *extension);

  /// @}

  /// @name Seek Functions
  /// @{

  protected: TiFunctionBase *foreachRef;
  protected: TiFunctionBase *foreachByParamPassRef;
  protected: TiFunctionBase *foreachByParamPass_routingRef;
  protected: TiFunctionBase *foreachByParamPass_templateRef;

  public: BINDING_INDEX_CACHE(foreachByParamPass);
  public: BINDING_INDEX_CACHE(foreachByParamPass_routing);
  public: BINDING_INDEX_CACHE(foreachByParamPass_template);

  private: static void _foreach(SeekerExtension *extension, TiFunctionBase *base, RtBinding *_self,
                                TiObject const *ref, TiObject *target, Core::Data::Seeker::SeekForeachCallback cb);

  private: static void _foreachByParamPass(SeekerExtension *extension, RtBinding *_self,
                                           Data::Ast::ParamPass const *paramPass, TiObject *data,
                                           Core::Data::Seeker::SeekForeachCallback cb);
  private: static Core::Data::Seeker::SeekVerb _foreachByParamPass_routing(SeekerExtension *extension, RtBinding *_self,
                                                                           Data::Ast::ParamPass const *paramPass,
                                                                           TiObject *data,
                                                                           Core::Data::Seeker::SeekForeachCallback cb);
  private: static Core::Data::Seeker::SeekVerb _foreachByParamPass_template(SeekerExtension *extension,
                                                                            RtBinding *_self,
                                                                            TiObject *param,
                                                                            Spp::Ast::Template *tmplt,
                                                                            Core::Data::Seeker::SeekForeachCallback cb);

  /// @}

}; // class

} // namespace

#endif
