/**
 * @file Spp/Ast/IntegerType.h
 * Contains the header of class Spp::Ast::IntegerType.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_INTEGERTYPE_H
#define SPP_AST_INTEGERTYPE_H

namespace Spp::Ast
{

class IntegerType : public DataType
{
  //============================================================================
  // Type Info

  TYPE_INFO(IntegerType, DataType, "Spp.Ast", "Spp", "alusus.net");

  IMPLEMENT_AST_MAP_CLONABLE(IntegerType);

  IMPLEMENT_AST_MAP_PRINTABLE(IntegerType);


  //============================================================================
  // Member Variables

  private: TiBool nullLiteral;

  private: TiBool withSign;

  private: mutable TioSharedPtr bitCountRef;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(DataType,
    (nullLiteral, TiBool, VALUE, setNullLiteral(value), &nullLiteral),
    (withSign, TiBool, VALUE, setSigned(value), &withSign)
  );


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(IntegerType);

  IMPLEMENT_ATTR_CONSTRUCTOR(IntegerType);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(IntegerType);


  //============================================================================
  // Member Functions

  public: void setNullLiteral(Bool nl)
  {
    this->nullLiteral = nl;
  }
  public: void setNullLiteral(TiBool const *nl)
  {
    this->setNullLiteral(nl == 0 ? false : nl->get());
  }

  public: Bool isNullLiteral() const
  {
    return this->nullLiteral.get();
  }

  public: void setSigned(Bool s)
  {
    this->withSign = s;
  }
  public: void setSigned(TiBool const *s)
  {
    this->setSigned(s == 0 ? false : s->get());
  }

  public: Bool isSigned() const
  {
    return this->withSign.get();
  }

  public: Word getBitCount(Helper *helper) const;

  public: virtual TypeMatchStatus matchTargetType(Type const *type, Helper *helper, ExecutionContext const *ec) const;

}; // class

} // namespace

#endif
