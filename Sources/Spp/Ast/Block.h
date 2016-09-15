/**
 * @file Spp/Ast/Block.h
 * Contains the header of class Spp::Ast::Block.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_BLOCK_H
#define SPP_AST_BLOCK_H

namespace Spp { namespace Ast
{

using namespace Core;

class Block : public Core::Data::Ast::Scope
{
  //============================================================================
  // Type Info

  TYPE_INFO(Block, Core::Data::Ast::Scope, "Spp.Ast", "Spp", "alusus.net");

  IMPLEMENT_AST_MAP_CLONABLE(Block);


  //============================================================================
  // Constructors & Destructor

  public: Block()
  {
    this->prepareStatementsElement();
  }

  public: Block(Word pid, Core::Data::SourceLocation const &sl) : Scope(pid, sl)
  {
    this->prepareStatementsElement();
  }

  public: Block(std::initializer_list<Argument<Char const*>> const &args) :
    Scope(args)
  {
    this->prepareStatementsElement();
  }

  public: Block(Word pid, std::initializer_list<Argument<Char const*>> const &args) :
    Block(args)
  {
    this->setProdId(pid);
  }

  public: Block(Word pid, Core::Data::SourceLocation const &sl,
                const std::initializer_list<Argument<Char const*>> &args) :
    Block(args)
  {
    this->setProdId(pid);
    this->setSourceLocation(sl);
  }

  public: virtual ~Block()
  {
  }


  //============================================================================
  // Member Functions

  private: void prepareStatementsElement();

  public: void setStatements(TioSharedPtr const &c)
  {
    this->set(0, c);
  }

  public: TioSharedPtr const& getStatements() const
  {
    return this->getShared(0);
  }


  //============================================================================
  // Printable Implementation

  public: virtual void print(OutStream &stream, Int indents=0) const;

}; // class

} } // namespace

#endif
