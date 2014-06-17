/**
 * @file Core/Data/GrammarPlant.h
 * Contains the header of class Core::Data::GrammarPlant.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_GRAMMAR_PLANT_H
#define DATA_GRAMMAR_PLANT_H

namespace Core { namespace Data
{

// TODO: DOC

class GrammarPlant
{
  //============================================================================
  // Member Variables

  protected: GrammarRepository repository;


  //============================================================================
  // Constructor & Destructor

  protected: GrammarPlant(Bool create=true)
  {
    if (create) this->createGrammar();
  }

  public: virtual ~GrammarPlant()
  {
  }


  //============================================================================
  // Member Functions

  /// Create the entire core grammar.
  protected: virtual void createGrammar();

  /// Create the list of char group definitions for the Core's grammar.
  protected: virtual void createCharGroupDefinitions() = 0;

  /// Create the list of token definitions for the Core's grammar.
  protected: virtual void createTokenDefinitions() = 0;

  /// Create the list of produciton definitions for the Core's grammar.
  protected: virtual void createProductionDefinitions() = 0;

  /// Generate lexer definitions for constant tokens used in a container tree.
  protected: void generateConstTokenDefinitions(SharedContainer *container);

  /// Generate lexer definitions for constant tokens used in a term tree.
  protected: void generateConstTokenDefinitions(Term *term);

  /// Generate lexer definitions for all strings found in the given hierarchy.
  protected: void generateConstTokensForStrings(IdentifiableObject *obj);

  // TODO: Replace this pure function with default implementation when the lexer is modified to use the Data
  //       namespace.
  protected: virtual Word addConstToken(Char const *text) = 0;

  public: GrammarRepository* getRepository()
  {
    return &this->repository;
  }

}; // class

} } // namespace

#endif
