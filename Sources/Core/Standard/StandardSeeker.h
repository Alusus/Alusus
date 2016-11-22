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
  using SeekVerb = Data::Seeker::SeekVerb;

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

  private: static void _set(Seeker *self, TiObject const *ref, TiObject *target, SeekSetCallback cb);
  private: static void _remove(Seeker *self, TiObject const *ref, TiObject *target, SeekRemoveCallback cb);
  private: static void _foreach(Seeker *self, TiObject const *ref, TiObject *target, SeekForeachCallback cb);

  /// @}

  /// @name Identifier Seek Functions
  /// @{

  private: void setByIdentifier(Data::Ast::Identifier const *identifier, TiObject *data, SeekSetCallback cb);
  private: void setByIdentifier_SharedRepository(Data::Ast::Identifier const *identifier,
                                                 Data::SharedRepository *repo, SeekSetCallback cb);
  private: void setByIdentifier_Ast(Data::Ast::Identifier const *identifier, TiObject *data, SeekSetCallback cb);

  private: void removeByIdentifier(Data::Ast::Identifier const *identifier, TiObject *data, SeekRemoveCallback cb);
  private: void removeByIdentifier_SharedRepository(Data::Ast::Identifier const *identifier,
                                                    Data::SharedRepository *repo, SeekRemoveCallback cb);
  private: void removeByIdentifier_Ast(Data::Ast::Identifier const *identifier, TiObject *data, SeekRemoveCallback cb);

  private: void foreachByIdentifier(Data::Ast::Identifier const *identifier, TiObject *data, SeekForeachCallback cb);
  private: void foreachByIdentifier_SharedRepository(Data::Ast::Identifier const *identifier,
                                                     Data::SharedRepository *repo, SeekForeachCallback cb);
  private: void foreachByIdentifier_Ast(Data::Ast::Identifier const *identifier, TiObject *data,
                                        SeekForeachCallback cb);

  /// @}

  /// @name LinkOperator Seek Functions
  /// @{

  private: void setByLinkOperator(Data::Ast::LinkOperator const *link, TiObject *data, SeekSetCallback cb);
  private: SeekVerb setByLinkOperator_routing(Data::Ast::LinkOperator const *link, TiObject *data, SeekSetCallback cb);
  private: SeekVerb setByLinkOperator_scopeDotIdentifier(Data::Ast::Identifier const *identifier,
                                                         Data::Ast::Scope *scope, SeekSetCallback cb);
  private: SeekVerb setByLinkOperator_mapDotIdentifier(Data::Ast::Identifier const *identifier, Data::MapContainer *map,
                                                       SeekSetCallback cb);

  private: void removeByLinkOperator(Data::Ast::LinkOperator const *link, TiObject *data, SeekRemoveCallback cb);
  private: SeekVerb removeByLinkOperator_routing(Data::Ast::LinkOperator const *link, TiObject *data,
                                                 SeekRemoveCallback cb);
  private: SeekVerb removeByLinkOperator_scopeDotIdentifier(Data::Ast::Identifier const *identifier,
                                                            Data::Ast::Scope *scope, SeekRemoveCallback cb);
  private: SeekVerb removeByLinkOperator_mapDotIdentifier(Data::Ast::Identifier const *identifier,
                                                          Data::MapContainer *map, SeekRemoveCallback cb);

  private: void foreachByLinkOperator(Data::Ast::LinkOperator const *link, TiObject *data, SeekForeachCallback cb);
  private: SeekVerb foreachByLinkOperator_routing(Data::Ast::LinkOperator const *link, TiObject *data,
                                                  SeekForeachCallback cb);
  private: SeekVerb foreachByLinkOperator_scopeDotIdentifier(Data::Ast::Identifier *identifier,
                                                             Data::Ast::Scope *scope, SeekForeachCallback cb);
  private: SeekVerb foreachByLinkOperator_mapDotIdentifier(Data::Ast::Identifier const *identifier,
                                                           Data::MapContainer *map, SeekForeachCallback cb);

  /// @}

}; // class

} } // namespace

#endif
