/**
 * @file Core/Data/Ast/ExpressionList.h
 * Contains the header of class Core::Data::Ast::ExpressionList.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_EXPRESSIONLIST_H
#define CORE_DATA_AST_EXPRESSIONLIST_H

namespace Core { namespace Data { namespace Ast
{

class ExpressionList : public List
{
  //============================================================================
  // Type Info

  TYPE_INFO(ExpressionList, List, "Core.Data.Ast", "Core", "alusus.net");

  IMPLEMENT_AST_LIST_CLONABLE(ExpressionList);

  IMPLEMENT_AST_LIST_PRINTABLE(ExpressionList);


  //============================================================================
  // Constructor / Destructor

  public: ExpressionList()
  {
  }

  public: ExpressionList(Word pid, SourceLocation const &sl) : List(pid, sl)
  {
  }

  public: ExpressionList(Word pid, const std::initializer_list<SharedPtr<TiObject>> &args) : List(pid, args)
  {
  }

  public: ExpressionList(Word pid, SourceLocation const &sl,
                         const std::initializer_list<SharedPtr<TiObject>> &args) :
    List(pid, sl, args)
  {
  }

  public: virtual ~ExpressionList()
  {
  }

  public: static SharedPtr<ExpressionList> create()
  {
    return std::make_shared<ExpressionList>();
  }

  public: static SharedPtr<ExpressionList> create(Word pid, SourceLocation const &sl)
  {
    return std::make_shared<ExpressionList>(pid, sl);
  }

  public: static SharedPtr<ExpressionList> create(Word pid,
                                                  const std::initializer_list<SharedPtr<TiObject>> &args)
  {
    return std::make_shared<ExpressionList>(pid, args);
  }

  public: static SharedPtr<ExpressionList> create(Word pid, SourceLocation const &sl,
                                                  const std::initializer_list<SharedPtr<TiObject>> &args)
  {
    return std::make_shared<ExpressionList>(pid, sl, args);
  }

}; // class

} } } // namespace

#endif
