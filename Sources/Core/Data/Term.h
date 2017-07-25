/**
 * @file Core/Data/Term.h
 * Contains the header of class Core::Data::Term.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_TERM_H
#define CORE_DATA_TERM_H

namespace Core { namespace Data
{

/**
 * @brief The base of grammar term classes.
 * @ingroup data_terms
 *
 * The base class of all grammar term classes. Term classes are used to
 * construct a grammar formulas.
 */
class Term : public Node
{
  //============================================================================
  // Type Info

  TYPE_INFO(Term, Node, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  /**
     * @brief Flags for parsing and code generation features.
     *
     * These flags helps the parser and parsing handler determine different
     * features associated with this term.
     */
  protected: SharedPtr<Node> flags;


  //============================================================================
  // Constructor / Destructor

  protected: Term(SharedPtr<Node> const &f=SharedPtr<Node>()) : flags(f)
  {
  }

  protected: Term(Int f) : flags(std::make_shared<Integer>(f))
  {
  }

  public: virtual ~Term()
  {
  }


  //============================================================================
  // Member Functions

  public: void setFlags(SharedPtr<Node> const &f)
  {
    if (f != 0 && !f->isA<Integer>() && !f->isDerivedFrom<Reference>()) {
      throw EXCEPTION(InvalidArgumentException, STR("f"), STR("Must be of type Integer or Reference."));
    }
    UPDATE_OWNED_SHAREDPTR(this->flags, f);
  }

  public: SharedPtr<Node> const& getFlags() const
  {
    return this->flags;
  }

}; // class

} } // namespace

#endif
