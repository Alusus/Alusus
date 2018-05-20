/**
 * @file Core/Data/Ast/Definition.h
 * Contains the header of class Core::Data::Ast::Definition.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_DEFINITION_H
#define CORE_DATA_AST_DEFINITION_H

namespace Core { namespace Data { namespace Ast
{

// TODO: DOC

class Definition : public Node,
                   public virtual Bindings, public virtual Basic::MapContainer<TiObject>, public virtual Metadata,
                   public virtual Clonable, public virtual Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Definition, Node, "Core.Data.Ast", "Core", "alusus.net");
  IMPLEMENT_INTERFACES(Node, Bindings, Basic::MapContainer<TiObject>, Metadata, Clonable, Printable);


  //============================================================================
  // Member Variables

  private: TiStr name;
  private: TioSharedPtr target;


  //============================================================================
  // Implementations

  IMPLEMENT_METADATA(Definition);

  IMPLEMENT_BINDINGS(Bindings,
    (name, TiStr, VALUE, setName(value), &name),
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContainer<TiObject>, (target, TiObject, setTarget(value), target.get()));


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Definition);

  IMPLEMENT_ATTR_CONSTRUCTOR(Definition);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(Definition);

  public: virtual ~Definition()
  {
    DISOWN_SHAREDPTR(this->target);
  }


  //============================================================================
  // Member Functions

  public: void setName(Char const *n)
  {
    this->name = n;
  }
  public: void setName(TiStr const *n)
  {
    this->name = n == 0 ? "" : n->get();
  }

  public: TiStr const& getName() const
  {
    return this->name;
  }

  public: void setTarget(TioSharedPtr const &t)
  {
    UPDATE_OWNED_SHAREDPTR(this->target, t);
  }
  private: void setTarget(TiObject *t)
  {
    this->setTarget(getSharedPtr(t));
  }

  public: TioSharedPtr const& getTarget() const
  {
    return this->target;
  }


  //============================================================================
  // Clonable Implementation

  public: virtual SharedPtr<TiObject> clone() const;


  //============================================================================
  // Printable Implementation

  public: virtual void print(OutStream &stream, Int indents=0) const;

}; // class

} } } // namespace

#endif
