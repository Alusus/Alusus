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
      &this->generateModuleInit,
      &this->generateFunction,
      &this->generateFunctionDecl,
      &this->generateUserTypeBody,
      &this->generateVarDef,
      &this->generateTempVar,
      &this->generateVarInitialization,
      &this->generateMemberVarInitialization,
      &this->generateVarDestruction,
      &this->generateMemberVarDestruction,
      &this->registerDestructor,
      &this->generateVarGroupDestruction,
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

  public: METHOD_BINDING_CACHE(generateModules, Bool, (Core::Data::Ast::Scope* /* root */, GenDeps const& /* deps */));

  public: METHOD_BINDING_CACHE(generateModule, Bool, (Spp::Ast::Module* /* astModule */, GenDeps const& /* deps */));

  public: METHOD_BINDING_CACHE(generateModuleInit,
    Bool, (Spp::Ast::Module* /* astModule */, GenDeps const& /* deps */)
  );

  public: METHOD_BINDING_CACHE(generateFunction, Bool, (Spp::Ast::Function* /* astFunc */, GenDeps const& /* deps */));

  public: METHOD_BINDING_CACHE(generateFunctionDecl,
    Bool, (Spp::Ast::Function* /* astFunc */, GenDeps const& /* deps */)
  );

  public: METHOD_BINDING_CACHE(generateUserTypeBody,
    Bool, (Spp::Ast::UserType* /* astType */, GenDeps const& /* deps */)
  );

  public: METHOD_BINDING_CACHE(generateVarDef,
    Bool, (Core::Data::Ast::Definition* /* def */, GenDeps const& /* deps */)
  );

  public: METHOD_BINDING_CACHE(generateTempVar,
    Bool, (
      Core::Data::Node* /* astNode */, Spp::Ast::Type* /* astType */, GenDeps const& /* deps */, Bool /* initialize */
    )
  );

  public: METHOD_BINDING_CACHE(generateVarInitialization, Bool, (
    Spp::Ast::Type* /* varAstType */, TiObject* /* tgVarRef */, Core::Data::Node* /* paramsAstNode */,
    PlainList<TiObject>* /* paramAstTypes */, SharedList<TiObject>* /* paramTgValues */,
    GenDeps const& /* deps */
  ));

  public: METHOD_BINDING_CACHE(generateMemberVarInitialization,
    Bool, (TiObject* /* astMemberNode */, GenDeps const& /* deps */)
  );

  public: METHOD_BINDING_CACHE(generateVarDestruction,
    Bool, (
      Spp::Ast::Type* /* varAstType */, TiObject* /* tgVarRef */, Core::Data::Node* /* astNode */,
      GenDeps const& /* deps */
    )
  );

  public: METHOD_BINDING_CACHE(generateMemberVarDestruction,
    Bool, (TiObject* /* astMemberNode */, GenDeps const& /* deps */)
  );

  public: METHOD_BINDING_CACHE(registerDestructor,
    void, (
      Core::Data::Node* /* varAstNode */, Ast::Type* /* astType */, ExecutionContext const* /* ec */,
      DestructionStack* /* destructionStack */
    )
  );

  public: METHOD_BINDING_CACHE(generateVarGroupDestruction,
    Bool, (GenDeps const& /* deps */, Int /* index */)
  );

  public: METHOD_BINDING_CACHE(generateStatements,
    Bool, (
      Core::Data::Ast::Scope* /* astBlock */, GenDeps const& /* deps */,
      TerminalStatement& /* terminal */
    )
  );

  public: METHOD_BINDING_CACHE(generateStatement,
    Bool, (
      TiObject* /* astNode */, GenDeps const& /* deps */, TerminalStatement& /* terminal */
    )
  );

  public: METHOD_BINDING_CACHE(generateExpression,
    Bool, (TiObject* /* astNode */, GenDeps const& /* deps */, GenResult& /* result */)
  );

  public: METHOD_BINDING_CACHE(generateCast,
    Bool, (
      GenDeps const& /* deps */,
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
