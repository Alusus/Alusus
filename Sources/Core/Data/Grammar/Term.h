/**
 * @file Core/Data/Grammar/Term.h
 * Contains the header of class Core::Data::Grammar::Term.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_DATA_GRAMMAR_TERM_H
#define CORE_DATA_GRAMMAR_TERM_H

namespace Core::Data::Grammar
{

/**
 * @brief The base of grammar term classes.
 * @ingroup core_data_grammar
 *
 * The base class of all grammar term classes. Term classes are used to
 * construct a grammar formulas.
 */
class Term : public Node, public Binding
{
  //============================================================================
  // Type Info

  TYPE_INFO(Term, Node, "Core.Data.Grammar", "Core", "alusus.org", (
    INHERITANCE_INTERFACES(Binding)
  ));


  //============================================================================
  // Member Variables

  /**
   * @brief Flags for parsing and code generation features.
   *
   * These flags helps the parser and parsing handler determine different
   * features associated with this term.
   */
  protected: TioSharedPtr flags;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Binding,
    (flags, TiObject, SHARED_REF, setFlags(value), flags.get())
  );


  //============================================================================
  // Constructor / Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Term);

  IMPLEMENT_ATTR_CONSTRUCTOR(Term);

  public: virtual ~Term()
  {
    RESET_OWNED_SHAREDPTR(this->flags);
  }


  //============================================================================
  // Member Functions

  public: void setFlags(TioSharedPtr const &f)
  {
    if (f != 0 && !f->isA<TiInt>() && !f->isDerivedFrom<Reference>()) {
      throw EXCEPTION(InvalidArgumentException, S("f"), S("Must be of type TiInt or Reference."));
    }
    UPDATE_OWNED_SHAREDPTR(this->flags, f);
  }

  private: void setFlags(TiObject *f)
  {
    this->setFlags(getSharedPtr(f));
  }

  public: TioSharedPtr const& getFlags() const
  {
    return this->flags;
  }

}; // class

} // namespace

#endif
