/**
 * @file Core/Data/GrammarHelper.h
 * Contains the header of class Core::Data::GrammarHelper.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_GRAMMAR_HELPER_H
#define DATA_GRAMMAR_HELPER_H

namespace Core { namespace Data
{

// TODO: DOC

class GrammarHelper
{
  //============================================================================
  // Member Variables

  private: Seeker seeker;


  //============================================================================
  // Constructor & Destructor

  public: GrammarHelper(const Provider *p = 0) : seeker(p)
  {
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Functions
  /// @{

  public: void setDataProvider(const Provider *prov)
  {
    this->seeker.setDataProvider(prov);
  }

  public: const Provider* getDataProvider() const
  {
    return this->seeker.getDataProvider();
  }

  /// @}

  /// @name Misc Functions
  /// @{

  public: void getValue(Reference *ref, Module *module, IdentifiableObject *&retVal, Module *&retModule) const;

  public: IdentifiableObject* traceValue(IdentifiableObject *val, Module *module=0) const;

  public: void traceValue(IdentifiableObject *val, IdentifiableObject *&retVal, Module *&retModule,
                          Module *module=0) const;

  /// @}

  /// @name Term Helper Functions
  /// @{

  public: IdentifiableObject* getListTermData(ListTerm *term, Module *module=0) const;

  public: Word getListTermChildCount(ListTerm *term, IdentifiableObject *data = 0, Module *module=0) const;

  public: Term* useListTermChild(DataContext *context, ListTerm *term, Int index,
                                 IdentifiableObject *data = 0, Module *module=0) const;

  public: Integer* getTokenTermId(TokenTerm *term, Module *module=0) const;

  public: IdentifiableObject* getTokenTermText(TokenTerm *term, Module *module=0) const;

  public: void getReferencedDefinition(Reference *ref, Module *&retModule, SymbolDefinition *&retDef,
                                       Module *module=0) const;

  public: Integer* getMultiplyTermMax(MultiplyTerm *term, Module *module=0) const;

  public: Integer* getMultiplyTermMin(MultiplyTerm *term, Module *module=0) const;

  public: Integer* getMultiplyTermPriority(MultiplyTerm *term, Module *module=0) const;

  /// @}

  /// @name Symbol Definition Helper Functions
  /// @{

  public: Term* getSymbolTerm(const SymbolDefinition *definition, Module *module=0) const;

  public: Map* getSymbolVars(const SymbolDefinition *definition, Module *module=0) const;

  /// @}

}; // class

} } // namespace

#endif
