/**
 * @file Core/Ast/Scope.h
 * Contains the header of class Core::Ast::Scope.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_AST_SCOPE_H
#define CORE_AST_SCOPE_H

namespace Core::Ast
{

class Scope : public List
{
  //============================================================================
  // Type Info

  TYPE_INFO(Scope, List, "Core.Ast", "Core", "alusus.org");
  OBJECT_FACTORY(Scope);


  //============================================================================
  // Memver Variables

  private: SubsetIndex bridgesIndex;

  /**
   * @brief An index of the definitions in this scope, grouped by name.
   * Each list holds the definitions of a single name in the order in which
   * they appear in the scope, since multiple definitions can share the same
   * name (like overloaded functions). Emptied lists are kept so that pointers
   * to them remain valid while the scope gets modified.
   */
  private: std::unordered_map<Str, std::vector<Definition*>> definitionsIndex;


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

  protected: virtual void onWillUpdate(Int index);

  protected: virtual void onUpdated(Int index);

  protected: virtual void onWillRemove(Int index);

  protected: virtual void onRemoved(Int index);

  /// @}

  /// @name Definition Retrieval Functions
  /// @{

  /**
   * @brief Get the definitions of the given name, in the order in which they appear in the scope.
   * @return A pointer to the list of definitions, or null if the scope has
   *         never had a definition of that name. The list can be empty if all
   *         definitions of that name were removed.
   */
  public: std::vector<Definition*> const* findDefinitions(Str const &name) const
  {
    auto iter = this->definitionsIndex.find(name);
    return iter == this->definitionsIndex.end() ? 0 : &iter->second;
  }

  /// Get the index of the given definition within the scope, or -1 if it's not in this scope.
  public: Int findDefinitionIndex(Definition const *def) const;

  /// Inform the scope that one of its definitions has been renamed.
  public: void onDefinitionRenamed(Definition *def, Str const &oldName);

  private: void addToDefinitionsIndex(Int index);

  private: void removeFromDefinitionsIndex(Definition *def, Str const &name);

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
