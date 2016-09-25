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

class Scope : public StatementList
{
  //============================================================================
  // Type Info

  TYPE_INFO(Scope, StatementList, "Core.Data.Ast", "Core", "alusus.net");

  IMPLEMENT_AST_LIST_CLONABLE(Scope);

  IMPLEMENT_AST_LIST_PRINTABLE(Scope);


  //============================================================================
  // Constructor / Destructor

  public: Scope()
  {
  }

  public: Scope(Word pid, SourceLocation const &sl) : StatementList(pid, sl)
  {
  }

  public: Scope(Word pid, const std::initializer_list<SharedPtr<TiObject>> &args) : StatementList(pid, args)
  {
  }

  public: Scope(Word pid, SourceLocation const &sl,
                const std::initializer_list<SharedPtr<TiObject>> &args) :
    StatementList(pid, sl, args)
  {
  }

  public: virtual ~Scope()
  {
  }

  public: static SharedPtr<Scope> create()
  {
    return std::make_shared<Scope>();
  }

  public: static SharedPtr<Scope> create(Word pid, SourceLocation const &sl)
  {
    return std::make_shared<Scope>(pid, sl);
  }

  public: static SharedPtr<Scope> create(Word pid, const std::initializer_list<SharedPtr<TiObject>> &args)
  {
    return std::make_shared<Scope>(pid, args);
  }

  public: static SharedPtr<Scope> create(Word pid, SourceLocation const &sl,
                                         const std::initializer_list<SharedPtr<TiObject>> &args)
  {
    return std::make_shared<Scope>(pid, sl, args);
  }

}; // class

} } } // namespace

#endif
