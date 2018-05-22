/**
 * @file Spp/Ast/ArgPack.h
 * Contains the header of class Spp::Ast::ArgPack.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_ARGPACK_H
#define SPP_AST_ARGPACK_H

namespace Spp::Ast
{

// TODO: DOC

class ArgPack : public Core::Data::Node,
                public virtual Core::Basic::Binding, public virtual Core::Basic::MapContaining<TiObject>,
                public virtual Core::Data::Ast::Metadata,
                public virtual Core::Data::Clonable, public virtual Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(ArgPack, Core::Data::Node, "Spp.Ast", "Core", "alusus.net");
  IMPLEMENT_INTERFACES(Core::Data::Node, Core::Basic::Binding, Core::Basic::MapContaining<TiObject>,
                                         Core::Data::Ast::Metadata, Core::Data::Clonable, Core::Data::Printable);


  //============================================================================
  // Member Variables

  private: TioSharedPtr argType;
  private: TiWord min;
  private: TiWord max;


  //============================================================================
  // Implementations

  IMPLEMENT_METADATA(ArgPack);

  IMPLEMENT_BINDING(Binding,
    (min, TiWord, VALUE, setMin(value), &min),
    (max, TiWord, VALUE, setMax(value), &max),
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Data::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>, (argType, TiObject, setArgType(value), argType.get()));

  IMPLEMENT_AST_CLONABLE(ArgPack);

  IMPLEMENT_AST_MAP_PRINTABLE(ArgPack, << this->min.get() << STR(" ") << this->max.get());


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
