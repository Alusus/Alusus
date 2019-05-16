/**
 * @file Spp/CodeGen/Generation.h
 * Contains the header of class Spp::CodeGen::Generation.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_GENERATION_H
#define SPP_CODEGEN_GENERATION_H

namespace Spp { namespace CodeGen
{

class Generation : public ObjTiInterface
{
  //============================================================================
  // Type Info

  OBJ_INTERFACE_INFO(Generation, ObjTiInterface, "Spp", "CodeGen", "alusus.org");


  //============================================================================
  // Member Variables

  private: TiObject *owner;


  //============================================================================
  // Constructor

  public: Generation(TiObject *o) : owner(o)
  {
    Basic::initBindingCaches(this->owner, {
      &this->generateModules,
      &this->generateModule,
      &this->generateFunction,
      &this->generateFunctionDecl,
      &this->generateUserTypeBody,
      &this->generateVarDef,
      &this->generateStatements,
      &this->generateStatement,
      &this->generateExpression,
      &this->generateCast,
      &this->getGeneratedType,
      &this->getTypeAllocationSize
    });
  }


  //============================================================================
  // Member Functions

  /// @name ObjTiInterface Implementation
  /// @{

  public: virtual TiObject* getTiObject()
  {
    return this->owner;
  }

  public: virtual TiObject const* getTiObject() const
  {
    return this->owner;
  }

  /// @}

  /// @name Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(generateModules, Bool, (Core::Data::Ast::Scope* /* root */, TargetGeneration* /* tg */));

  public: METHOD_BINDING_CACHE(generateModule, Bool, (Spp::Ast::Module* /* astModule */, TargetGeneration* /* tg */));

  public: METHOD_BINDING_CACHE(generateFunction, Bool, (Spp::Ast::Function* /* astFunc */, TargetGeneration* /* tg */));

  public: METHOD_BINDING_CACHE(generateFunctionDecl,
    Bool, (Spp::Ast::Function* /* astFunc */, TargetGeneration* /* tg */)
  );

  public: METHOD_BINDING_CACHE(generateUserTypeBody,
    Bool, (Spp::Ast::UserType* /* astType */, TargetGeneration* /* tg */)
  );

  public: METHOD_BINDING_CACHE(generateVarDef, Bool, (Core::Data::Ast::Definition*, TargetGeneration* /* tg */));

  public: METHOD_BINDING_CACHE(generateStatements,
    Bool, (
      Core::Data::Ast::Scope* /* astBlock */, TargetGeneration* /* tg */, TiObject* /* tgContext */,
      TerminalStatement& /* terminal */
    )
  );

  public: METHOD_BINDING_CACHE(generateStatement,
    Bool, (
      TiObject* /* astNode */, TargetGeneration* /* tg */, TiObject* /* tgContext */, TerminalStatement& /* terminal */
    )
  );

  public: METHOD_BINDING_CACHE(generateExpression,
    Bool, (TiObject* /* astNode */, TargetGeneration* /* tg */, TiObject* /* tgContext */, GenResult& /* result */)
  );

  public: METHOD_BINDING_CACHE(generateCast,
    Bool, (
      TargetGeneration* /* tg */, TiObject* /* tgContext */,
      Spp::Ast::Type* /* srcType */, Spp::Ast::Type* /* destType */,
      TiObject* /* tgValue */, TioSharedPtr& /* tgCastedValue */
    )
  );

  public: METHOD_BINDING_CACHE(getGeneratedType,
    Bool, (
      TiObject* /* ref */, TargetGeneration* /* tg */,
      TiObject*& /* targetTypeResult */, Ast::Type** /* astTypeResult */
    )
  );

  public: METHOD_BINDING_CACHE(getTypeAllocationSize,
    Bool, (Spp::Ast::Type* /* astType */, TargetGeneration* /* tg */, Word& /* result */)
  );

  /// @}

}; // class

} } // namespace

#endif
