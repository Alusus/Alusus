/**
 * @file Core/Ast/Passage.h
 * Contains the header of class Core::Ast::Passage.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_AST_PASSAGE_H
#define CORE_AST_PASSAGE_H

namespace Core::Ast
{

class Passage : public Node,
                public Binding
{
  //============================================================================
  // Type Info

  TYPE_INFO(Passage, Node, "Core.Ast", "Core", "alusus.org");
  IMPLEMENT_INTERFACES(
    Node, Binding
  );
  OBJECT_FACTORY(Passage);


  //============================================================================
  // Member Variables

  private: Node *target = 0;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Binding,
    (target, Node, PLAIN_REF, set(value), target),
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Passage);

  IMPLEMENT_ATTR_CONSTRUCTOR(Passage);

  public: Passage(Node *t)
  {
    this->set(t);
  }

  public: static SharedPtr<Passage> create(Node *t)
  {
    return newSrdObj<Passage>(t);
  }

  public: virtual ~Passage()
  {
  }


  //============================================================================
  // Member Functions

  public: void set(Node *t)
  {
    this->target = t;
  }

  public: Node* get() const
  {
    return this->target;
  }


  //============================================================================
  // Printing Functions

  public: virtual void print(OutStream &stream, Int indents=0) const
  {
    stream << S("Passage");
    Word id = this->getProdId();
    stream << S(": ") << this->get();
    if (id != UNKNOWN_ID) {
      stream << S(" [") << ID_GENERATOR->getDesc(id) << S("]");
    }
  }

}; // class

} // namespace

#endif
