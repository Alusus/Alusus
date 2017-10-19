/**
 * @file Core/Data/Ast/GenericCommand.h
 * Contains the header of class Core::Data::Ast::GenericCommand.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_GENERICCOMMAND_H
#define CORE_DATA_AST_GENERICCOMMAND_H

namespace Core { namespace Data { namespace Ast
{

class GenericCommand : public Node,
                       public virtual Bindings, public virtual MapContainer, public virtual Metadata,
                       public virtual Clonable, public virtual Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(GenericCommand, Node, "Core.Data.Ast", "Core", "alusus.net");
  IMPLEMENT_INTERFACES(Node, Bindings, MapContainer, Metadata, Clonable, Printable);


  //============================================================================
  // Member Variables

  protected: TiStr type;
  protected: SharedPtr<List> args;
  protected: SharedPtr<List> modifiers;


  //============================================================================
  // Implementations

  IMPLEMENT_METADATA(GenericCommand);

  IMPLEMENT_BINDINGS(Bindings,
                     (type, TiStr, VALUE, setType(value), &type),
                     (prodId, TiWord, VALUE, setProdId(value), &prodId),
                     (sourceLocation, SourceLocation, VALUE, setSourceLocation(value), &sourceLocation));

  IMPLEMENT_MAP_CONTAINER(MapContainer, (List, args), (List, modifiers));

  IMPLEMENT_AST_MAP_CLONABLE(GenericCommand);

  IMPLEMENT_AST_MAP_PRINTABLE(GenericCommand, << this->type.get());


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(GenericCommand);

  IMPLEMENT_ATTR_CONSTRUCTOR(GenericCommand);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(GenericCommand);

  public: virtual ~GenericCommand()
  {
    DISOWN_SHAREDPTR(this->args);
    DISOWN_SHAREDPTR(this->modifiers);
  }


  //============================================================================
  // Member Functions

  public: void setType(Char const *t)
  {
    this->type = t;
  }
  public: void setType(TiStr const *t)
  {
    this->type = t == 0 ? "" : t->get();
  }

  public: TiStr const& getType() const
  {
    return this->type;
  }

  public: void setArgs(SharedPtr<List> const &a)
  {
    UPDATE_OWNED_SHAREDPTR(this->args, a);
  }

  public: void addArg(TioSharedPtr const &arg)
  {
    if (this->args == 0) {
      this->args = List::create({}, { arg });
    } else {
      this->args->add(arg);
    }
  }

  public: SharedPtr<List> const& getArgs() const
  {
    return this->args;
  }

  public: void setModifiers(SharedPtr<List> const &m)
  {
    UPDATE_OWNED_SHAREDPTR(this->modifiers, m);
  }

  public: void addModifier(TioSharedPtr const &modifier)
  {
    if (this->modifiers == 0) {
      this->modifiers = List::create({}, { modifier });
    } else {
      this->modifiers->add(modifier);
    }
  }

  public: SharedPtr<List> const& getModifiers() const
  {
    return this->modifiers;
  }

}; // class

} } } // namespace

#endif
