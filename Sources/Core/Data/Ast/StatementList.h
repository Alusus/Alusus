/**
 * @file Core/Data/Ast/StatementList.h
 * Contains the header of class Core::Data::Ast::StatementList.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_STATEMENTLIST_H
#define CORE_DATA_AST_STATEMENTLIST_H

namespace Core { namespace Data { namespace Ast
{

class StatementList : public List
{
  //============================================================================
  // Type Info

  TYPE_INFO(StatementList, List, "Core.Data.Ast", "Core", "alusus.net");

  IMPLEMENT_AST_LIST_CLONABLE(StatementList);

  IMPLEMENT_AST_LIST_PRINTABLE(StatementList);


  //============================================================================
  // Constructor / Destructor

  public: StatementList()
  {
  }

  public: StatementList(Word pid, SourceLocation const &sl) : List(pid, sl)
  {
  }

  public: StatementList(Word pid, const std::initializer_list<SharedPtr<TiObject>> &args) : List(pid, args)
  {
  }

  public: StatementList(Word pid, SourceLocation const &sl,
                        const std::initializer_list<SharedPtr<TiObject>> &args) :
    List(pid, sl, args)
  {
  }

  public: virtual ~StatementList()
  {
  }

  public: static SharedPtr<StatementList> create()
  {
    return std::make_shared<StatementList>();
  }

  public: static SharedPtr<StatementList> create(Word pid, SourceLocation const &sl)
  {
    return std::make_shared<StatementList>(pid, sl);
  }

  public: static SharedPtr<StatementList> create(Word pid,
                                                 const std::initializer_list<SharedPtr<TiObject>> &args)
  {
    return std::make_shared<StatementList>(pid, args);
  }

  public: static SharedPtr<StatementList> create(Word pid, SourceLocation const &sl,
                                                 const std::initializer_list<SharedPtr<TiObject>> &args)
  {
    return std::make_shared<StatementList>(pid, sl, args);
  }

}; // class

} } } // namespace

#endif
