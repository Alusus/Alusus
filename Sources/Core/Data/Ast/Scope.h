/**
 * @file Core/Data/Ast/Scope.h
 * Contains the header of class Core::Data::Ast::Scope.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_SCOPE_H
#define CORE_DATA_AST_SCOPE_H

namespace Core::Data::Ast
{

class Scope : public List
{
  //============================================================================
  // Type Info

  TYPE_INFO(Scope, List, "Core.Data.Ast", "Core", "alusus.org");
  OBJECT_FACTORY(Scope);


  //============================================================================
  // Memver Variables

  private: SubsetIndex bridgesIndex;


  //============================================================================
  // Implementations

  IMPLEMENT_AST_LIST_PRINTABLE(Scope);


  //============================================================================
  // Constructor / Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Scope);

  IMPLEMENT_ATTR_CONSTRUCTOR(Scope);

  IMPLEMENT_ATTR_LIST_CONSTRUCTOR(Scope);


  //============================================================================
  // Member Functions

  /// @name Inheritted Functions
  /// @{

  protected: virtual void onAdded(Int index);

  protected: virtual void onUpdated(Int index);

  protected: virtual void onRemoved(Int index);

  /// @}

  /// @name Bridge Retrieval Functions
  /// @{

  public: Word getBridgeCount() const
  {
    return this->bridgesIndex.getSize();
  }

  public: Bridge* getBridge(Int index) const;

  /// @}

}; // class

} // namespace

#endif
