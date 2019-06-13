/**
 * @file Spp/Ast/ArgPack.h
 * Contains the header of class Spp::Ast::ArgPack.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_ARGPACK_H
#define SPP_AST_ARGPACK_H

namespace Spp::Ast
{

// TODO: DOC

class ArgPack : public Core::Data::Node,
                public Binding, public MapContaining<TiObject>,
                public Core::Data::Ast::MetaHaving,
                public Core::Data::Clonable, public Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(ArgPack, Core::Data::Node, "Spp.Ast", "Core", "alusus.org");
  IMPLEMENT_INTERFACES(
    Core::Data::Node, Binding, MapContaining<TiObject>,
    Core::Data::Ast::MetaHaving, Core::Data::Clonable, Core::Data::Printable
  );
  OBJECT_FACTORY(ArgPack);


  //============================================================================
  // Member Variables

  private: TioSharedPtr argType;
  private: TiWord min;
  private: TiWord max;


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(ArgPack);

  IMPLEMENT_BINDING(Binding,
    (min, TiWord, VALUE, setMin(value), &min),
    (max, TiWord, VALUE, setMax(value), &max),
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Data::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>,
    (argType, TiObject, SHARED_REF, setArgType(value), argType.get())
  );

  IMPLEMENT_AST_CLONABLE(ArgPack);

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

  public: void setArgType(TioSharedPtr const &t)
  {
    UPDATE_OWNED_SHAREDPTR(this->argType, t);
  }
  private: void setArgType(TiObject *t)
  {
    this->setArgType(getSharedPtr(t));
  }

  public: TioSharedPtr const& getArgType() const
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
