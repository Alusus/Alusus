/**
 * @file Spp/Ast/ArgPack.h
 * Contains the header of class Spp::Ast::ArgPack.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_ARGPACK_H
#define SPP_AST_ARGPACK_H

namespace Spp::Ast
{

// TODO: DOC

class ArgPack : public Core::Ast::Node,
                public Binding, public MapContaining<Core::Ast::Node>,
                public Core::Ast::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(ArgPack, Core::Ast::Node, "Spp.Ast", "Core", "alusus.org");
  IMPLEMENT_INTERFACES(
    Core::Ast::Node, Binding, MapContaining<Core::Ast::Node>,
    Core::Ast::Printable
  );
  OBJECT_FACTORY(ArgPack);


  //============================================================================
  // Member Variables

  private: SharedPtr<Core::Ast::Node> argType;
  private: TiWord min;
  private: TiWord max;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Binding,
    (min, TiWord, VALUE, setMin(value), &min),
    (max, TiWord, VALUE, setMax(value), &max),
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Ast::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<Core::Ast::Node>,
    (argType, Core::Ast::Node, SHARED_REF, setArgType(value), argType.get())
  );

  IMPLEMENT_AST_MAP_PRINTABLE(ArgPack, << this->min.get() << S(" ") << this->max.get());


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(ArgPack);

  IMPLEMENT_ATTR_CONSTRUCTOR(ArgPack);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(ArgPack);

  public: virtual ~ArgPack()
  {
    DISOWN_SHAREDPTR(this->argType);
  }


  //============================================================================
  // Member Functions

  public: void setArgType(SharedPtr<Core::Ast::Node> const &t)
  {
    UPDATE_OWNED_SHAREDPTR(this->argType, t);
  }
  private: void setArgType(Core::Ast::Node *t)
  {
    this->setArgType(getSharedPtr(t));
  }

  public: SharedPtr<Core::Ast::Node> const& getArgType() const
  {
    return this->argType;
  }

  public: void setMin(Word m)
  {
    this->min = m;
  }
  public: void setMin(TiWord const *m)
  {
    this->min = m == 0 ? 0 : m->get();
  }

  public: TiWord const& getMin() const
  {
    return this->min;
  }

  public: void setMax(Word m)
  {
    this->max = m;
  }
  public: void setMax(TiWord const *m)
  {
    this->max = m == 0 ? 0 : m->get();
  }

  public: TiWord const& getMax() const
  {
    return this->max;
  }

}; // class

} // namespace

#endif
