/**
 * @file Core/Ast/Alias.h
 * Contains the header of class Core::Ast::Alias.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_AST_ALIAS_H
#define CORE_AST_ALIAS_H

namespace Core::Ast
{

class Alias : public Node,
              public Binding, public MapContaining<Node>
{
  //============================================================================
  // Type Info

  TYPE_INFO(Alias, Node, "Core.Ast", "Core", "alusus.org");
  IMPLEMENT_INTERFACES(Node, Binding, MapContaining<Node>);
  OBJECT_FACTORY(Alias);


  //============================================================================
  // Member Variables

  private: SharedPtr<Node> reference;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<Node>,
    (reference, Node, SHARED_REF, setReference(value), reference.get())
  );


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Alias);

  IMPLEMENT_ATTR_CONSTRUCTOR(Alias);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(Alias);

  public: virtual ~Alias()
  {
    DISOWN_SHAREDPTR(this->reference);
  }


  //============================================================================
  // Member Functions

  public: void setReference(SharedPtr<Node> const &r)
  {
    UPDATE_OWNED_SHAREDPTR(this->reference, r);
  }
  private: void setReference(Node *r)
  {
    this->setReference(getSharedPtr(r));
  }

  public: SharedPtr<Node> const& getReference() const
  {
    return this->reference;
  }


  //============================================================================
  // Printing Functions

  public: virtual void print(OutStream &stream, Int indents=0) const
  {
    stream << S("Alias");
    stream << S(": ");
    Ast::dumpAst(stream, this->reference.get(), indents);
    Word id = this->getProdId();
    if (id != UNKNOWN_ID) {
      stream << S(" [") << ID_GENERATOR->getDesc(id) << S("]");
    }
  }

}; // class

} // namespace

#endif
