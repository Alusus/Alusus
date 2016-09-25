/**
 * @file Core/Data/Ast/Definition.h
 * Contains the header of class Core::Data::Ast::Definition.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
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
                   public virtual MapContainer, public virtual MetadataHolder,
                   public virtual Clonable, public virtual Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Definition, Node, "Core.Data.Ast", "Core", "alusus.net");
  IMPLEMENT_INTERFACES(Node, MapContainer, MetadataHolder, Clonable, Printable);


  //============================================================================
  // Member Variables

  private: Str name;
  private: TioSharedPtr target;

  IMPLEMENT_MAP_CONTAINER((TiObject, target));


  //============================================================================
  // Constructors & Destructor

  public: Definition()
  {
  }

  public: Definition(Word pid, SourceLocation const &sl) :
    MetadataHolder(pid, sl)
  {
  }

  public: Definition(Word pid, Char const *n, TioSharedPtr const &t) :
    MetadataHolder(pid), name(n), target(t)
  {
    OWN_SHAREDPTR(this->target);
  }

  public: Definition(Word pid, SourceLocation const &sl, Char const *n, TioSharedPtr const &t) :
    MetadataHolder(pid, sl), name(n), target(t)
  {
    OWN_SHAREDPTR(this->target);
  }

  public: virtual ~Definition()
  {
    DISOWN_SHAREDPTR(this->target);
  }

  public: static SharedPtr<Definition> create()
  {
    return std::make_shared<Definition>();
  }

  public: static SharedPtr<Definition> create(Word pid, SourceLocation const &sl)
  {
    return std::make_shared<Definition>(pid, sl);
  }

  public: static SharedPtr<Definition> create(Word pid, Char const *n, TioSharedPtr const &t)
  {
    return std::make_shared<Definition>(pid, n, t);
  }

  public: static SharedPtr<Definition> create(Word pid, SourceLocation const &sl, Char const *n, TioSharedPtr const &t)
  {
    return std::make_shared<Definition>(pid, sl, n, t);
  }


  //============================================================================
  // Member Functions

  public: void setName(Char const *n)
  {
    this->name = n;
  }

  public: Str const& getName() const
  {
    return this->name;
  }

  public: void setTarget(TioSharedPtr const &t)
  {
    UPDATE_OWNED_SHAREDPTR(this->target, t);
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
