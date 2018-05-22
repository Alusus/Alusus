/**
 * @file Core/Data/Ast/Alias.h
 * Contains the header of class Core::Data::Ast::Alias.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_ALIAS_H
#define CORE_DATA_AST_ALIAS_H

namespace Core { namespace Data { namespace Ast
{

class Alias : public Node,
              public virtual Binding, public virtual Basic::MapContaining<TiObject>, public virtual Metadata,
              public virtual Clonable, public virtual Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Alias, Node, "Core.Data.Ast", "Core", "alusus.net");
  IMPLEMENT_INTERFACES(Node, Binding, Basic::MapContaining<TiObject>, Metadata, Clonable, Printable);


  //============================================================================
  // Member Variables

  private: TioSharedPtr reference;


  //============================================================================
  // Implementations

  IMPLEMENT_METADATA(Alias);

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>, (reference, TiObject, setReference(value), reference.get()));

  IMPLEMENT_AST_LIST_CLONABLE(Scope);


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

  public: void setReference(TioSharedPtr const &r)
  {
    UPDATE_OWNED_SHAREDPTR(this->reference, r);
  }
  private: void setReference(TiObject *r)
  {
    this->setReference(getSharedPtr(r));
  }

  public: TioSharedPtr const& getReference() const
  {
    return this->reference;
  }


  //============================================================================
  // Printable Implementation

  public: virtual void print(OutStream &stream, Int indents=0) const
  {
    stream << STR("Alias");
    stream << STR(": ");
    Core::Data::dumpData(stream, this->reference.get(), indents);
    Word id = this->getProdId();
    if (id != UNKNOWN_ID) {
      stream << STR(" [") << IdGenerator::getSingleton()->getDesc(id) << STR("]");
    }
  }

}; // class

} } } // namespace

#endif
