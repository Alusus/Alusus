/**
 * @file Core/Ast/Within.h
 * Contains the header of class Core::Ast::Within.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_AST_WITHIN_H
#define CORE_AST_WITHIN_H

namespace Core::Ast
{

/**
 * @brief Adds one or more statements to an existing scope.
 *
 * The subject is either a single statement or a List of statements, in which case the list gets expanded into the
 * target scope. If the qualifier is null the subject is added to the scope that contains the Within object, otherwise
 * it's added to the scope that the qualifier points to.
 */
class Within : public Node, public MapContaining<Node>
{
  //============================================================================
  // Type Info

  TYPE_INFO(Within, Node, "Core.Ast", "Core", "alusus.org");
  IMPLEMENT_INTERFACES(Node, MapContaining<Node>);
  OBJECT_FACTORY(Within);


  //============================================================================
  // Member Variables

  private: SharedPtr<Node> qualifier;
  private: SharedPtr<Node> subject;


  //============================================================================
  // Implementations

  IMPLEMENT_MAP_CONTAINING(MapContaining<Node>,
    (qualifier, Node, SHARED_REF, setQualifier(value), qualifier.get()),
    (subject, Node, SHARED_REF, setSubject(value), subject.get())
  );


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Within);

  IMPLEMENT_ATTR_CONSTRUCTOR(Within);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(Within);

  public: virtual ~Within()
  {
    DISOWN_SHAREDPTR(this->qualifier);
    DISOWN_SHAREDPTR(this->subject);
  }


  //============================================================================
  // Member Functions

  public: void setQualifier(SharedPtr<Node> const &q)
  {
    UPDATE_OWNED_SHAREDPTR(this->qualifier, q);
  }
  private: void setQualifier(Node *q)
  {
    this->setQualifier(getSharedPtr(q));
  }

  public: SharedPtr<Node> const& getQualifier() const
  {
    return this->qualifier;
  }

  public: void setSubject(SharedPtr<Node> const &s)
  {
    UPDATE_OWNED_SHAREDPTR(this->subject, s);
  }
  private: void setSubject(Node *s)
  {
    this->setSubject(getSharedPtr(s));
  }

  public: SharedPtr<Node> const& getSubject() const
  {
    return this->subject;
  }


  //============================================================================
  // Printing Functions

  public: virtual void print(OutStream &stream, Int indents=0) const
  {
    stream << S("Within");
    if (this->qualifier != 0) {
      stream << S(" ");
      Ast::dumpAst(stream, this->qualifier.get(), indents);
    }
    stream << S(": ");
    Ast::dumpAst(stream, this->subject.get(), indents);
    Word id = this->getProdId();
    if (id != UNKNOWN_ID) {
      stream << S(" [") << ID_GENERATOR->getDesc(id) << S("]");
    }
    Node::printModifiers(stream, indents);
    Node::printMetadata(stream, indents);
  }

}; // class

} // namespace

#endif
