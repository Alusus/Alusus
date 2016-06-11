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


  //============================================================================
  // Overrides

  public: virtual SharedPtr<TiObject> clone() const
  {
    SharedPtr<ExpressionList> newList = std::make_shared<ExpressionList>();
    newList->setProdId(this->getProdId());
    for (Word i = 0; i < this->getCount(); ++i) {
      newList->add(this->get(i));
    }
    newList->setSourceLocation(this->getSourceLocation());
    return newList;
  }

  public: virtual void print(OutStream &stream, Int indents=0) const
  {
    stream << STR("ExpressionList") ;
    Word id = this->getProdId();
    if (id != UNKNOWN_ID) {
      stream << STR(" [") << IdGenerator::getSingleton()->getDesc(id) << STR("]");
    }
    for (Word i = 0; i < this->getCount(); ++i) {
      stream << STR("\n");
      printIndents(stream, indents+1);
      dumpData(stream, this->get(i), indents+1);
    }
  }

}; // class

} } } // namespace

#endif
