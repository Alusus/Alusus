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
                public Binding, public MapContaining<TiObject>,
                public MetaHaving, public Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Passage, Node, "Core.Data.Ast", "Core", "alusus.org");
  IMPLEMENT_INTERFACES(
    Node, Binding, MapContaining<TiObject>,
    MetaHaving, Printable
  );
  OBJECT_FACTORY(Passage);


  //============================================================================
  // Member Variables

  private: TiObject *target = 0;


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(Passage);

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Data::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>,
    (target, TiObject, PLAIN_REF, set(value), target)
  );

  IMPLEMENT_AST_MAP_PRINTABLE(Passage);


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Passage);

  IMPLEMENT_ATTR_CONSTRUCTOR(Passage);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(Passage);

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

}; // class

} // namespace

#endif
