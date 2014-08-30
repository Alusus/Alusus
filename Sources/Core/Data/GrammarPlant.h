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

#ifndef DATA_GRAMMARPLANT_H
#define DATA_GRAMMARPLANT_H

namespace Core { namespace Data
{

// TODO: DOC

class GrammarPlant
{
  //============================================================================
  // Member Variables

  protected: GrammarRepository repository;
  protected: Str constTokenPrefix;


  //============================================================================
  // Constructor & Destructor

  protected: GrammarPlant()
  {
  }

  public: virtual ~GrammarPlant()
  {
  }


  //============================================================================
  // Member Functions

  /// Search the entire grammar for const token to generate.
  protected: void generateConstTokenDefinitions()
  {
    this->generateConstTokenDefinitions(this->repository.getRoot()->getInterface<SharedContainer>());
  }

  /// Generate lexer definitions for constant tokens used in a container tree.
  protected: void generateConstTokenDefinitions(SharedContainer *container);

  /// Generate lexer definitions for constant tokens used in a term tree.
  protected: void generateConstTokenDefinitions(Term *term);

  /// Generate lexer definitions for all strings found in the given hierarchy.
  protected: void generateConstTokensForStrings(IdentifiableObject *obj);

  /**
   * @brief Add a const token definition in the lexer module.
   * A name for the token will be automatically generated and the location
   * at which it will be inserted is defined by constTokenPrefix. This prefix
   * should contain the path to the containing module and it should not
   * include the scope (the scope will be root:). The value of constTokenPrefix
   * should be set by the derived class before calling
   * generateConstTokenDefinitions.
   */
  protected: virtual Word addConstToken(Char const *text);

  /**
   * @brief Generate a string key for the given text.
   * The key will begin with __ and will be followed by the text itself after
   * converting any character that isn't a letter or a number into its ASCII
   * code preceeded by a single _.
   */
  protected: static void generateKey(Char const *text, Str &result);

  public: GrammarRepository* getRepository()
  {
    return &this->repository;
  }

}; // class

} } // namespace

#endif
