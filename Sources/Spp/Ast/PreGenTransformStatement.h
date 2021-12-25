/**
 * @file Spp/Ast/PreGenTransformStatement.h
 * Contains the header of class Spp::Ast::PreGenTransformStatement.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_PREGENTRANSFORMSTATEMENT_H
#define SPP_AST_PREGENTRANSFORMSTATEMENT_H

namespace Spp::Ast
{

class PreGenTransformStatement : public Core::Data::Node,
                                 public Binding, public MapContaining<TiObject>,
                                 public Core::Data::Ast::MetaHaving, public Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(PreGenTransformStatement, Core::Data::Node, "Spp.Ast", "Spp", "alusus.org");
  IMPLEMENT_INTERFACES(
    Core::Data::Node, Binding, MapContaining<TiObject>,
    Core::Data::Ast::MetaHaving, Core::Data::Printable
  );
  OBJECT_FACTORY(PreGenTransformStatement);


  //============================================================================
  // Types

  public: typedef TioSharedPtr(*TransformFunc)(TiObject*);


  //============================================================================
  // Member Variables

  private: TiPtr transform;
  private: TioSharedPtr body;
  private: Bool transformed = false;


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(PreGenTransformStatement);

  IMPLEMENT_BINDING(Binding,
    (transform, TiPtr, VALUE, setTransform(value), &transform),
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Data::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>,
    (body, TiObject, SHARED_REF, setBody(value), body.get())
  );

  IMPLEMENT_AST_MAP_PRINTABLE(PreGenTransformStatement);


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(PreGenTransformStatement);

  IMPLEMENT_ATTR_CONSTRUCTOR(PreGenTransformStatement);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(PreGenTransformStatement);

  public: virtual ~PreGenTransformStatement()
  {
    DISOWN_SHAREDPTR(this->body);
  }


  //============================================================================
  // Member Functions

  public: void setTransform(TiPtr const &t)
  {
    this->transform = t;
  }
  public: void setTransform(TiPtr const *t)
  {
    this->transform = t == 0 ? 0 : t->get();
  }

  public: TiPtr const& getTransform() const
  {
    return this->transform;
  }

  public: TransformFunc getTransformFunc() const
  {
    return reinterpret_cast<TransformFunc>(this->transform.get());
  }

  public: void setTransformed(Bool t)
  {
    this->transformed = t;
  }
  public: Bool isTransformed() const
  {
    return this->transformed;
  }

  public: void setBody(TioSharedPtr const &b)
  {
    UPDATE_OWNED_SHAREDPTR(this->body, b);
  }
  private: void setBody(TiObject *b)
  {
    this->setBody(getSharedPtr(b));
  }

  public: TioSharedPtr const& getBody() const
  {
    return this->body;
  }

}; // class

} // namespace

#endif
