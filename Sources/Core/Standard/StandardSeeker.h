/**
 * @file Core/Data/StandardSeeker.h
 * Contains the header of class Core::Data::StandardSeeker.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_STANDARD_STANDARDSEEKER_H
#define CORE_STANDARD_STANDARDSEEKER_H

namespace Core { namespace Standard
{

class StandardSeeker : public Data::Seeker
{
  //============================================================================
  // Type Info

  TYPE_INFO(StandardSeeker, Seeker, "Core.Standard", "Core", "alusus.net");


  //============================================================================
  // Constructors

  StandardSeeker()
  {
    this->initialize();
  }

  StandardSeeker(StandardSeeker *parent) : Seeker(parent)
  {
  }


  //============================================================================
  // Member Functions

  /// @name Initialization
  /// @{

  private: void initialize();

  /// @}

  /// @name Main Seek Functions
  /// @{

  private: void _set(TiObject const *ref, TiObject *target, SeekSetCallback cb);
  private: void _remove(TiObject const *ref, TiObject *target, SeekRemoveCallback cb);
  private: void _foreach(TiObject const *ref, TiObject *target, SeekForeachCallback cb);

  /// @}

  /// @name Identifier Seek Functions
  /// @{

  private: void setByIdentifier(Data::Ast::Identifier const *identifier, TiObject *data, SeekSetCallback cb);
  private: void removeByIdentifier(Data::Ast::Identifier const *identifier, TiObject *data, SeekRemoveCallback cb);
  private: void foreachByIdentifier(Data::Ast::Identifier const *identifer, TiObject *data, SeekForeachCallback cb);

  /// @}

  /// @name LinkOperator Seek Functions
  /// @{

  private: void setByLinkOperator(Data::Ast::LinkOperator const *link, TiObject *data, SeekSetCallback cb);
  private: SeekVerb setByLinkOperator_routing(Data::Ast::LinkOperator const *link, TiObject *data, SeekSetCallback cb);
  private: SeekVerb setByLinkOperator_scopeDotIdentifier(Data::Ast::Identifier const *identifier,
                                                         Data::Ast::Scope *scope, SeekSetCallback cb);
  private: SeekVerb setByLinkOperator_mapDotIdentifier(Data::Ast::Identifier const *identifier, Data::MapContainer *map,
                                                       SeekSetCallback cb);

  // TODO:

  private: void removeByLinkOperator(Data::Ast::LinkOperator *ref, TiObject *data, SeekRemoveCallback cb);
  private: SeekVerb removeByLinkOperator_routing(Data::Ast::LinkOperator *ref, TiObject *data, SeekRemoveCallback cb);
  private: SeekVerb removeByLinkOperator_scopeDotIdentifier(Data::Ast::LinkOperator *ref, TiObject *data,
                                                            SeekRemoveCallback cb);
  private: SeekVerb removeByLinkOperator_anyDotIdentifier(Data::Ast::LinkOperator *ref, TiObject *data,
                                                          SeekRemoveCallback cb);

  private: void foreachByLinkOperator(Data::Ast::LinkOperator *ref, TiObject *data, SeekForeachCallback cb);
  private: SeekVerb foreachByLinkOperator_routing(Data::Ast::LinkOperator *ref, TiObject *data, SeekForeachCallback cb);
  private: SeekVerb foreachByLinkOperator_scopeDotIdentifier(Data::Ast::LinkOperator *ref, TiObject *data,
                                                             SeekForeachCallback cb);
  private: SeekVerb foreachByLinkOperator_anyDotIdentifier(Data::Ast::LinkOperator *ref, TiObject *data,
                                                           SeekForeachCallback cb);

  /// @}

}; // class

} } // namespace

#endif
