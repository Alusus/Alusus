/**
 * @file Spp/Ast/ReferenceType.h
 * Contains the header of class Spp::Ast::ReferenceType.
 *
 * @copyright Copyright (C) 2024 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_REFERENCETYPE_H
#define SPP_AST_REFERENCETYPE_H

namespace Spp::Ast
{

//============================================================================
// Other Defs

ti_s_enum(ReferenceMode, TiInt, "Spp.Ast", "Spp", "alusus.org", EXPLICIT, TEMP_EXPLICIT, IMPLICIT, NO_DEREF);


//============================================================================
// ReferenceType class

class ReferenceType : public DataType
{
  //============================================================================
  // Type Info

  TYPE_INFO(ReferenceType, DataType, "Spp.Ast", "Spp", "alusus.org");
  OBJECT_FACTORY(ReferenceType);

  IMPLEMENT_AST_MAP_PRINTABLE(ReferenceType);


  //============================================================================
  // Member Variables

  private: ReferenceMode mode;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(DataType,
    (mode, ReferenceMode, VALUE, setMode(value), &this->mode)
  );


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(ReferenceType);

  IMPLEMENT_ATTR_CONSTRUCTOR(ReferenceType);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(ReferenceType);


  //============================================================================
  // Member Functions

  public: void setMode(ReferenceMode const &m)
  {
    this->mode = m;
  }
  public: void setMode(ReferenceMode const *m)
  {
    this->mode = m == 0 ? ReferenceMode::EXPLICIT : m->get();
  }

  public: ReferenceMode const& getMode() const
  {
    return this->mode;
  }


  public: Bool isImplicit() const
  {
    return this->mode == ReferenceMode::IMPLICIT;
  }

  public: Bool isAutoDeref() const
  {
    return this->mode != ReferenceMode::NO_DEREF;
  }

  public: Type* getContentType(Helper *helper) const;

  public: virtual TypeMatchStatus matchTargetType(
    Type const *type, Helper *helper, TypeMatchOptions opts = TypeMatchOptions::NONE
  ) const;

  public: virtual Bool isIdentical(Type const *type, Helper *helper) const;

}; // class

} // namespace

#endif
