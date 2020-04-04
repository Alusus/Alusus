/**
 * @file Spp/CodeGen/Generation.h
 * Contains the header of class Spp::CodeGen::Generation.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
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
      &this->generateFunctionCall,
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

  public: METHOD_BINDING_CACHE(generateModules, Bool, (Core::Data::Ast::Scope* /* root */, Session* /* session */));

  public: METHOD_BINDING_CACHE(generateModule, Bool, (Spp::Ast::Module* /* astModule */, Session* /* session */));

  public: METHOD_BINDING_CACHE(generateModuleInit,
    Bool, (Spp::Ast::Module* /* astModule */, Session* /* session */)
  );

  public: METHOD_BINDING_CACHE(generateFunction, Bool, (Spp::Ast::Function* /* astFunc */, Session* /* session */));

  public: METHOD_BINDING_CACHE(generateFunctionDecl,
    Bool, (Spp::Ast::Function* /* astFunc */, Session* /* session */)
  );

  public: METHOD_BINDING_CACHE(generateUserTypeBody,
    Bool, (Spp::Ast::UserType* /* astType */, Session* /* session */)
  );

  public: METHOD_BINDING_CACHE(generateVarDef,
    Bool, (Core::Data::Ast::Definition* /* def */, Session* /* session */)
  );

  public: METHOD_BINDING_CACHE(generateTempVar,
    Bool, (
      Core::Data::Node* /* astNode */, Spp::Ast::Type* /* astType */, Session* /* session */, Bool /* initialize */
    )
  );

  public: METHOD_BINDING_CACHE(generateVarInitialization, Bool, (
    Spp::Ast::Type* /* varAstType */, TiObject* /* tgVarRef */, Core::Data::Node* /* astNode */,
    PlainList<TiObject>* /* paramAstNodes */, PlainList<TiObject>* /* paramAstTypes */,
    SharedList<TiObject>* /* paramTgValues */, Session* /* session */
  ));

  public: METHOD_BINDING_CACHE(generateMemberVarInitialization,
    Bool, (TiObject* /* astMemberNode */, Session* /* session */)
  );

  public: METHOD_BINDING_CACHE(generateVarDestruction,
    Bool, (
      Spp::Ast::Type* /* varAstType */, TiObject* /* tgVarRef */, Core::Data::Node* /* astNode */,
      Session* /* session */
    )
  );

  public: METHOD_BINDING_CACHE(generateMemberVarDestruction,
    Bool, (TiObject* /* astMemberNode */, Session* /* session */)
  );

  public: METHOD_BINDING_CACHE(registerDestructor,
    void, (
      Core::Data::Node* /* varAstNode */, Ast::Type* /* astType */, ExecutionContext const* /* ec */,
      DestructionStack* /* destructionStack */
    )
  );

  public: METHOD_BINDING_CACHE(generateVarGroupDestruction,
    Bool, (Session* /* session */, Int /* index */)
  );

  public: METHOD_BINDING_CACHE(generateStatements,
    Bool, (
      Core::Data::Ast::Scope* /* astBlock */, Session* /* session */,
      TerminalStatement& /* terminal */
    )
  );

  public: METHOD_BINDING_CACHE(generateStatement,
    Bool, (
      TiObject* /* astNode */, Session* /* session */, TerminalStatement& /* terminal */
    )
  );

  public: METHOD_BINDING_CACHE(generateExpression,
    Bool, (TiObject* /* astNode */, Session* /* session */, GenResult& /* result */)
  );

  public: METHOD_BINDING_CACHE(generateCast,
    Bool, (
      Session* /* session */, Spp::Ast::Type* /* srcType */, Spp::Ast::Type* /* destType */,
      Core::Data::Node* /* astNode */, TiObject* /* tgValue */, Bool /* implicit */, TioSharedPtr& /* tgCastedValue */
    )
  );

  public: METHOD_BINDING_CACHE(generateFunctionCall,
    Bool, (
      Core::Data::Node* /* astNode */, Spp::Ast::Function* /* callee */,
      Containing<TiObject>* /* paramAstTypes */, Containing<TiObject>* /* paramTgValues */,
      Session* /* session */, GenResult& /* result */
    )
  );

  public: METHOD_BINDING_CACHE(getGeneratedType,
    Bool, (
      TiObject* /* ref */, Session* /* session */,
      TiObject*& /* targetTypeResult */, Ast::Type** /* astTypeResult */
    )
  );

  public: METHOD_BINDING_CACHE(getTypeAllocationSize,
    Bool, (Spp::Ast::Type* /* astType */, Session* /* session */, Word& /* result */)
  );

  /// @}

}; // class

} } // namespace

#endif
