/**
 * @file Core/Parser/ProdLevel.h
 * Contains the header of class Core::Parser::ProdLevel.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef PARSER_PRODUCTION_LEVEL_H
#define PARSER_PRODUCTION_LEVEL_H

namespace Core { namespace Parser
{

// TODO: DOC

class ProdLevel
{
  //============================================================================
  // Friend Classes

  friend class State;


  //============================================================================
  // Member Variables

  private: Data::Module *module;

  private: Data::SymbolDefinition *prod;

  private: Int termStackIndex;


  //============================================================================
  // Constructors / Destructor

  public: ProdLevel() : module(0), prod(0), termStackIndex(-1)
  {
  }

  public: ProdLevel(const ProdLevel &level) : module(level.module),
    prod(level.prod),
    termStackIndex(level.termStackIndex)
  {
  }

  public: ~ProdLevel()
  {
  }


  //============================================================================
  // Member Functions

  protected: void setModule(Data::Module *m)
  {
    this->module = m;
  }

  public: Data::Module* getModule() const
  {
    return this->module;
  }

  protected: void setProd(Data::SymbolDefinition *p)
  {
    this->prod = p;
  }

  public: Data::SymbolDefinition* getProd() const
  {
    return this->prod;
  }

  protected: void setTermStackIndex(Int i)
  {
    this->termStackIndex = i;
  }

  public: Int getTermStackIndex() const
  {
    return this->termStackIndex;
  }

}; // class

} } // namespace

#endif
