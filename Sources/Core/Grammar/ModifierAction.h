/**
 * @file Core/Grammar/ModifierAction.h
 * Contains the header of class Core::Grammar::ModifierAction and its subclasses.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_GRAMMAR_MODIFIERACTION_H
#define CORE_GRAMMAR_MODIFIERACTION_H

namespace Core::Grammar
{

/**
 * @brief The base of the actions that can be requested for an incoming modifier keyword.
 * @ingroup core_data_grammar
 *
 * SymbolDefinition::modifierActions maps a modifier's keyword to an instance of one of this class's
 * subclasses, which GenericParsingHandler::onIncomingModifier uses to decide what to do with a modifier
 * bearing that keyword.
 */
class ModifierAction : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(ModifierAction, TiObject, "Core.Grammar", "Core", "alusus.org");

}; // class


/**
 * @brief Requests translating a modifier's keyword to another keyword.
 * @ingroup core_data_grammar
 *
 * The modifier is translated in place, then reprocessed as if it was written with the translated keyword
 * to begin with.
 */
class TranslateModifierAction : public ModifierAction
{
  //============================================================================
  // Type Info

  TYPE_INFO(TranslateModifierAction, ModifierAction, "Core.Grammar", "Core", "alusus.org");


  //============================================================================
  // Member Variables

  private: Str keyword;


  //============================================================================
  // Constructor

  public: TranslateModifierAction(Char const *k) : keyword(k)
  {
  }


  //============================================================================
  // Member Functions

  public: Str const& getKeyword() const
  {
    return this->keyword;
  }

}; // class


/**
 * @brief Requests accepting the modifier and storing it as a modifier on the target Node.
 * @ingroup core_data_grammar
 */
class KeepModifierAction : public ModifierAction
{
  //============================================================================
  // Type Info

  TYPE_INFO(KeepModifierAction, ModifierAction, "Core.Grammar", "Core", "alusus.org");

}; // class


/**
 * @brief Requests accepting the modifier and storing its params on the target Node's customData.
 * @ingroup core_data_grammar
 *
 * The modifier's params (whatever appeared between its `[]` brackets) get stored, under the given key,
 * in the customData of the Node the modifier applies to. If the modifier has no params an IntegerLiteral
 * with a value of "1" is stored instead.
 */
class StoreModifierAction : public ModifierAction
{
  //============================================================================
  // Type Info

  TYPE_INFO(StoreModifierAction, ModifierAction, "Core.Grammar", "Core", "alusus.org");


  //============================================================================
  // Member Variables

  private: Str key;


  //============================================================================
  // Constructor

  public: StoreModifierAction(Char const *k) : key(k)
  {
  }


  //============================================================================
  // Member Functions

  public: Str const& getKey() const
  {
    return this->key;
  }

}; // class

} // namespace

#endif
