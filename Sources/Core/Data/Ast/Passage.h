/**
 * @file Core/Data/Ast/Passage.h
 * Contains the header of class Core::Data::Ast::Passage.
 *
 * @copyright Copyright (C) 2024 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_PASSAGE_H
#define CORE_DATA_AST_PASSAGE_H

namespace Core::Data::Ast
{

class Passage : public Node,
                public Binding, public MetaHaving, public Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Passage, Node, "Core.Data.Ast", "Core", "alusus.org");
  IMPLEMENT_INTERFACES(
    Node, Binding, MetaHaving, Printable
  );
  OBJECT_FACTORY(Passage);


  //============================================================================
  // Member Variables

  private: TiObject *target = 0;


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(Passage);

  IMPLEMENT_BINDING(Binding,
    (target, TiObject, PLAIN_REF, set(value), target),
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Data::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Passage);

  IMPLEMENT_ATTR_CONSTRUCTOR(Passage);

  public: Passage(TiObject *t)
  {
    this->set(t);
  }

  public: static SharedPtr<Passage> create(TiObject *t)
  {
    return newSrdObj<Passage>(t);
  }

  public: virtual ~Passage()
  {
  }


  //============================================================================
  // Member Functions

  public: void set(TiObject *t)
  {
    this->target = t;
  }

  public: TiObject* get() const
  {
    return this->target;
  }


  //============================================================================
  // Printable Implementation

  public: virtual void print(OutStream &stream, Int indents=0) const
  {
    stream << S("Passage");
    Word id = this->getProdId();
    stream << S(": ") << this->get();
    if (id != UNKNOWN_ID) {
      stream << S(" [") << IdGenerator::getSingleton()->getDesc(id) << S("]");
    }
  }

}; // class

} // namespace

#endif
