/**
 * @file Spp/Ast/PreGenTransformStatement.h
 * Contains the header of class Spp::Ast::PreGenTransformStatement.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
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

class PreGenTransformStatement : public Core::Ast::Node,
                                 public Binding, public MapContaining<Core::Ast::Node>,
                                 public Core::Ast::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(PreGenTransformStatement, Core::Ast::Node, "Spp.Ast", "Spp", "alusus.org");
  IMPLEMENT_INTERFACES(
    Core::Ast::Node, Binding, MapContaining<Core::Ast::Node>,
    Core::Ast::Printable
  );
  OBJECT_FACTORY(PreGenTransformStatement);


  //============================================================================
  // Types

  public: typedef SharedPtr<Core::Ast::Node>(*TransformFunc)(Core::Ast::Node*);


  //============================================================================
  // Member Variables

  private: TiPtr transform;
  private: SharedPtr<Core::Ast::Node> body;
  private: Bool transformed = false;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Binding,
    (transform, TiPtr, VALUE, setTransform(value), &transform),
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Ast::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<Core::Ast::Node>,
    (body, Core::Ast::Node, SHARED_REF, setBody(value), body.get())
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
    this->transform.set(t.get());
  }
  public: void setTransform(TiPtr const *t)
  {
    this->transform.set(t == 0 ? 0 : t->get());
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

  public: void setBody(SharedPtr<Core::Ast::Node> const &b)
  {
    UPDATE_OWNED_SHAREDPTR(this->body, b);
  }
  private: void setBody(Core::Ast::Node *b)
  {
    this->setBody(getSharedPtr(b));
  }

  public: SharedPtr<Core::Ast::Node> const& getBody() const
  {
    return this->body;
  }

}; // class

} // namespace

#endif
