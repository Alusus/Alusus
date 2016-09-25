/**
 * @file Spp/Ast/Type.h
 * Contains the header of class Spp::Ast::Type.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_TYPE_H
#define SPP_AST_TYPE_H

namespace Spp { namespace Ast
{

using namespace Core;

class Type : public Block
{
  //============================================================================
  // Type Info

  TYPE_INFO(Type, Block, "Spp.Ast", "Spp", "alusus.net");

  IMPLEMENT_AST_LIST_CLONABLE(Type);

  IMPLEMENT_AST_LIST_PRINTABLE(Type);


  //============================================================================
  // Constructors & Destructor

  public: Type()
  {
  }

  public: Type(Word pid, Core::Data::SourceLocation const &sl) : Block(pid, sl)
  {
  }

  public: Type(Word pid, std::initializer_list<SharedPtr<TiObject>> const &args) : Block(pid, args)
  {
  }

  public: Type(Word pid, Core::Data::SourceLocation const &sl,
               std::initializer_list<SharedPtr<TiObject>> const &args) :
    Block(pid, sl, args)
  {
  }

  public: virtual ~Type()
  {
  }

}; // class

} } // namespace

#endif
