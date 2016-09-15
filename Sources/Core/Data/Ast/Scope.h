/**
 * @file Core/Data/Ast/Scope.h
 * Contains the header of class Core::Data::Ast::Scope.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_SCOPE_H
#define CORE_DATA_AST_SCOPE_H

namespace Core { namespace Data { namespace Ast
{

// TODO: DOC

class Scope : public SharedMap,
              public virtual IdHolder, public virtual MetadataHolder, public virtual Clonable, public virtual Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Scope, SharedMap, "Core.Data.Ast", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_4(SharedMap, IdHolder, MetadataHolder, Clonable, Printable);

  IMPLEMENT_AST_MAP_CLONABLE(Scope);

  IMPLEMENT_AST_MAP_PRINTABLE(Scope);


  //============================================================================
  // Constructor / Destructor

  public: Scope() : SharedMap(true)
  {
  }

  public: Scope(Word pid, SourceLocation const &sl) :
    SharedMap(true), MetadataHolder(pid, sl)
  {
  }

  public: Scope(std::initializer_list<Argument<Char const*>> const &args) :
    SharedMap(true)
  {
    for (auto arg : args) {
      this->add(arg.id, arg.ioVal);
    }
  }

  public: Scope(Word pid, std::initializer_list<Argument<Char const*>> const &args) :
    Scope(args)
  {
    this->setProdId(pid);
  }

  public: Scope(Word pid, SourceLocation const &sl,
                const std::initializer_list<Argument<Char const*>> &args) :
    Scope(args)
  {
    this->setProdId(pid);
    this->setSourceLocation(sl);
  }

  public: virtual ~Scope()
  {
  }

  public: static SharedPtr<Scope> create()
  {
    return std::make_shared<Scope>();
  }

  public: static SharedPtr<Scope> creaet(Word pid, SourceLocation const &sl)
  {
    return std::make_shared<Scope>(pid, sl);
  }

  public: static SharedPtr<Scope> create(const std::initializer_list<Argument<Char const*>> &args)
  {
    return std::make_shared<Scope>(args);
  }

  public: static SharedPtr<Scope> create(Word pid, const std::initializer_list<Argument<Char const*>> &args)
  {
    return std::make_shared<Scope>(pid, args);
  }

  public: static SharedPtr<Scope> create(Word pid, SourceLocation const &sl,
                                          const std::initializer_list<Argument<Char const*>> &args)
  {
    return std::make_shared<Scope>(pid, sl, args);
  }

}; // class

} } } // namespace

#endif
