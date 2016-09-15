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

  IMPLEMENT_AST_MAP_CLONABLE(Type);


  //============================================================================
  // Constructors & Destructor

  public: Type()
  {
  }

  public: Type(Word pid, Core::Data::SourceLocation const &sl) : Block(pid, sl)
  {
  }

  public: Type(std::initializer_list<Argument<Char const*>> const &args) : Block(args)
  {
  }

  public: Type(Word pid, std::initializer_list<Argument<Char const*>> const &args) : Block(pid, args)
  {
  }

  public: Type(Word pid, Core::Data::SourceLocation const &sl,
               const std::initializer_list<Argument<Char const*>> &args) :
    Block(pid, sl, args)
  {
  }

  public: virtual ~Type()
  {
  }


  //============================================================================
  // Printable Implementation

  public: virtual void print(OutStream &stream, Int indents=0) const;

}; // class

} } // namespace

#endif
