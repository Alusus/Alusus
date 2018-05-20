/**
 * @file Spp/CodeGen/ExpressionGenerator.h
 * Contains the header of class Spp::CodeGen::ExpressionGenerator.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_EXPRESSIONGENERATOR_H
#define SPP_CODEGEN_EXPRESSIONGENERATOR_H

namespace Spp { namespace CodeGen
{

class ExpressionGenerator : public TiObject, public virtual DynamicBindings, public virtual DynamicInterfaces
{
  //============================================================================
  // Type Info

  TYPE_INFO(ExpressionGenerator, TiObject, "Spp.CodeGen", "Spp", "alusus.net", (
    INHERITANCE_INTERFACES(DynamicBindings, DynamicInterfaces),
    OBJECT_INTERFACE_LIST(interfaceList)
  ));


  //============================================================================
  // Implementations

  IMPLEMENT_DYNAMIC_BINDINGS(bindingMap);
  IMPLEMENT_DYNAMIC_INTERFACES(interfaceList);


  //============================================================================
  // Member Variables

  private: Ast::Helper *astHelper;
  private: NoOpTargetGenerator *noOpTargetGenerator;
  private: Core::Notices::Store *noticeStore = 0;


  //============================================================================
  // Constructors & Destructor

  public: ExpressionGenerator(Ast::Helper *h, NoOpTargetGenerator *noOpTg) : astHelper(h), noOpTargetGenerator(noOpTg)
  {
    this->initBindingCaches();
    this->initBindings();
  }

  public: ExpressionGenerator(ExpressionGenerator *parent)
  {
    this->initBindingCaches();
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);
    this->astHelper = parent->getAstHelper();
    this->noOpTargetGenerator = parent->getNoOpTargetGenerator();
  }

  public: virtual ~ExpressionGenerator()
  {
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Functions
  /// @{

  private: void initBindingCaches();
  private: void initBindings();

  public: Ast::Helper* getAstHelper() const
  {
    return this->astHelper;
  }

  public: NoOpTargetGenerator *getNoOpTargetGenerator() const
  {
    return this->noOpTargetGenerator;
  }

  public: void setNoticeStore(Core::Notices::Store *ns)
  {
    this->noticeStore = ns;
  }

  public: Core::Notices::Store* getNoticeStore() const
  {
    return this->noticeStore;
  }

  /// @}

  /// @name Top Level Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(generate,
    Bool, (
      TiObject* /* astNode */, Generation* /* g */, TargetGeneration* /* tg */, TiObject* /* tgContext */,
      GenResult& /* result */
    )
  );
  private: static Bool _generate(
    TiObject *self, TiObject *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateList,
    Bool, (
      Core::Data::Ast::ExpressionList* /* astNode */, Generation* /* g */, TargetGeneration* /* tg */,
      TiObject* /* tgContext */, GenResult& /* result */
    )
  );
  private: static Bool _generateList(
    TiObject *self, Core::Data::Ast::ExpressionList *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
    GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateIdentifier,
    Bool, (
      Core::Data::Ast::Identifier* /* astNode */, Generation* /* g */, TargetGeneration* /* tg */,
      TiObject* /* tgContext */, GenResult& /* result */
    )
  );
  private: static Bool _generateIdentifier(
    TiObject *self, Core::Data::Ast::Identifier *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
    GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateScopeMemberReference,
    Bool, (
      TiObject* /* scope */, Core::Data::Ast::Identifier* /* astNode */, Bool /* searchOwners */,
      Generation* /* g */, TargetGeneration* /* tg */, TiObject* /* tgContext */, GenResult& /* result */
    )
  );
  private: static Bool _generateScopeMemberReference(
    TiObject *self, TiObject *scope, Core::Data::Ast::Identifier *astNode, Bool searchOwners,
    Generation *g, TargetGeneration *tg, TiObject *tgContext, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateLinkOperator,
    Bool, (
      Core::Data::Ast::LinkOperator* /* astNode */, Generation* /* g */, TargetGeneration* /* tg */,
      TiObject* /* tgContext */, GenResult& /* result */
    )
  );
  private: static Bool _generateLinkOperator(
    TiObject *self, Core::Data::Ast::LinkOperator *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
    GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateParamPass,
    Bool, (
      Core::Data::Ast::ParamPass* /* astNode */, Generation* /* g */, TargetGeneration* /* tg */,
      TiObject* /* tgContext */, GenResult& /* result */
    )
  );
  private: static Bool _generateParamPass(
    TiObject *self, Core::Data::Ast::ParamPass *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
    GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateRoundParamPass,
    Bool, (
      Core::Data::Ast::ParamPass* /* astNode */, Generation* /* g */, TargetGeneration* /* tg */,
      TiObject* /* tgContext */, GenResult& /* result */
    )
  );
  private: static Bool _generateRoundParamPass(
    TiObject *self, Core::Data::Ast::ParamPass *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
    GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateSquareParamPass,
    Bool, (
      Core::Data::Ast::ParamPass* /* astNode */, Generation* /* g */, TargetGeneration* /* tg */,
      TiObject* /* tgContext */, GenResult& /* result */
    )
  );
  private: static Bool _generateSquareParamPass(
    TiObject *self, Core::Data::Ast::ParamPass *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
    GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateOperator,
    Bool, (
      Core::Data::Node* /* astNode */, Generation* /* g */, TargetGeneration* /* tg */,
      TiObject* /* tgContext */, GenResult& /* result */
    )
  );
  private: static Bool _generateOperator(
    TiObject *self, Core::Data::Node *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
    GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateLogicalOperator,
    Bool, (
      Core::Data::Ast::InfixOperator* /* astNode */, Generation* /* g */, TargetGeneration* /* tg */,
      TiObject* /* tgContext */, GenResult& /* result */
    )
  );
  private: static Bool _generateLogicalOperator(
    TiObject *self, Core::Data::Ast::InfixOperator *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
    GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateAssignment,
    Bool, (
      Core::Data::Ast::AssignmentOperator* /* astNode */, Generation* /* g */, TargetGeneration* /* tg */,
      TiObject* /* tgContext */, GenResult& /* result */
    )
  );
  private: static Bool _generateAssignment(
    TiObject *self, Core::Data::Ast::AssignmentOperator *astNode, Generation *g, TargetGeneration *tg,
    TiObject *tgContext, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generatePointerOp,
    Bool, (
      Spp::Ast::PointerOp* /* astNode */, Generation* /* g */, TargetGeneration* /* tg */,
      TiObject* /* tgContext */, GenResult& /* result */
    )
  );
  private: static Bool _generatePointerOp(
    TiObject *self, Spp::Ast::PointerOp *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
    GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateContentOp,
    Bool, (
      Spp::Ast::ContentOp* /* astNode */, Generation* /* g */, TargetGeneration* /* tg */,
      TiObject* /* tgContext */, GenResult& /* result */
    )
  );
  private: static Bool _generateContentOp(
    TiObject *self, Spp::Ast::ContentOp *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
    GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateCastOp,
    Bool, (
      Spp::Ast::CastOp* /* astNode */, Generation* /* g */, TargetGeneration* /* tg */,
      TiObject* /* tgContext */, GenResult& /* result */
    )
  );
  private: static Bool _generateCastOp(
    TiObject *self, Spp::Ast::CastOp *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
    GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateSizeOp,
    Bool, (
      Spp::Ast::SizeOp* /* astNode */, Generation* /* g */, TargetGeneration* /* tg */,
      TiObject* /* tgContext */, GenResult& /* result */
    )
  );
  private: static Bool _generateSizeOp(
    TiObject *self, Spp::Ast::SizeOp *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
    GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateStringLiteral,
    Bool, (
      Core::Data::Ast::StringLiteral* /* astNode */, Generation* /* g */, TargetGeneration* /* tg */,
      TiObject* /* tgContext */, GenResult& /* result */
    )
  );
  private: static Bool _generateStringLiteral(
    TiObject *self, Core::Data::Ast::StringLiteral *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
    GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateIntegerLiteral,
    Bool, (
      Core::Data::Ast::IntegerLiteral* /* astNode */, Generation* /* g */, TargetGeneration* /* tg */,
      TiObject* /* tgContext */, GenResult& /* result */
    )
  );
  private: static Bool _generateIntegerLiteral(
    TiObject *self, Core::Data::Ast::IntegerLiteral *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
    GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateFloatLiteral,
    Bool, (
      Core::Data::Ast::FloatLiteral* /* astNode */, Generation* /* g */, TargetGeneration* /* tg */,
      TiObject* /* tgContext */, GenResult& /* result */
    )
  );
  private: static Bool _generateFloatLiteral(
    TiObject *self, Core::Data::Ast::FloatLiteral *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
    GenResult &result
  );

  /// @}

  /// @name Inner Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(generateVarReference,
    Bool, (
      TiObject* /* astNode */, Generation* /* g */, TargetGeneration* /* tg */, TiObject* /* tgContext */,
      GenResult& /* result */
    )
  );
  private: static Bool _generateVarReference(
    TiObject *self, TiObject *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateMemberReference,
    Bool, (
      TiObject* /* tgValue */, Ast::Type* /* astType */, Core::Data::Ast::Identifier* /* astNode */,
      Generation* /* g */, TargetGeneration* /* tg */, TiObject* /* tgContext */, GenResult& /* result */
    )
  );
  private: static Bool _generateMemberReference(
    TiObject *self, TiObject *tgValue, Ast::Type * astType, Core::Data::Ast::Identifier *astNode,
    Generation *g, TargetGeneration *tg, TiObject *tgContext, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateArrayReference,
    Bool, (
      TiObject* /* tgValue */, Ast::Type* /* astType */, TiObject* /* tgIndexVal */, Ast::Type* /* astIndexType */,
      Generation* /* g */, TargetGeneration* /* tg */, TiObject* /* tgContext */, GenResult& /* result */
    )
  );
  private: static Bool _generateArrayReference(
    TiObject *self, TiObject *tgValue, Ast::Type *astType, TiObject *tgIndexVal, Ast::Type *astIndexType,
    Generation *g, TargetGeneration *tg, TiObject *tgContext, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateFunctionCall,
    Bool, (
      Spp::Ast::Function* /* callee */,
      Core::Basic::Container<TiObject>* /* paramAstTypes */, Core::Basic::Container<TiObject>* /* paramTgValues */,
      Generation* /* g */, TargetGeneration* /* tg */, TiObject* /* tgContext */, GenResult& /* result */
    )
  );
  private: static Bool _generateFunctionCall(
    TiObject *self, Spp::Ast::Function *callee,
    Core::Basic::Container<TiObject> *paramAstTypes, Core::Basic::Container<TiObject> *paramTgValues,
    Generation *g, TargetGeneration *tg, TiObject *tgContext, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateBuiltInFunctionCall,
    Bool, (
      Spp::Ast::Function* /* callee */,
      Core::Basic::Container<TiObject>* /* paramAstTypes */, Core::Basic::Container<TiObject>* /* paramTgValues */,
      Generation* /* g */, TargetGeneration* /* tg */, TiObject* /* tgContext */, GenResult& /* result */
    )
  );
  private: static Bool _generateBuiltInFunctionCall(
    TiObject *self, Spp::Ast::Function *callee,
    Core::Basic::Container<TiObject> *paramAstTypes, Core::Basic::Container<TiObject> *paramTgValues,
    Generation *g, TargetGeneration *tg, TiObject *tgContext, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateUserFunctionCall,
    Bool, (
      Spp::Ast::Function* /* callee */,
      Core::Basic::Container<TiObject>* /* paramAstTypes */, Core::Basic::Container<TiObject>* /* paramTgValues */,
      Generation* /* g */, TargetGeneration* /* tg */, TiObject* /* tgContext */, GenResult& /* result */
    )
  );
  private: static Bool _generateUserFunctionCall(
    TiObject *self, Spp::Ast::Function *callee,
    Core::Basic::Container<TiObject> *paramAstTypes, Core::Basic::Container<TiObject> *paramTgValues,
    Generation *g, TargetGeneration *tg, TiObject *tgContext, GenResult &result
  );

  /// @}

  /// @name Helper Functions
  /// @{

  private: Bool generateParamList(
    Core::Basic::TiObject *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
    Core::Basic::ListContainer<TiObject> *resultTypes, Core::Basic::SharedList<TiObject, TiObject> *resultValues
  );

  private: Bool generateParamList(
    Core::Basic::Container<TiObject> *astNodes, Generation *g, TargetGeneration *tg, TiObject *tgContext,
    Core::Basic::ListContainer<TiObject> *resultTypes, Core::Basic::SharedList<TiObject, TiObject> *resultValues
  );

  private: void prepareFunctionParams(
    Spp::Ast::Function *callee, Generation *g, TargetGeneration *tg, TiObject *tgContext,
    Core::Basic::ListContainer<TiObject> *paramAstTypes, Core::Basic::SharedList<TiObject, TiObject> *paramTgVals
  );

  private: Bool dereferenceIfNeeded(
    TargetGeneration *tg, TiObject *tgContext, Spp::Ast::Type *astType, TiObject *tgValue, GenResult &result
  );

  private: Bool castLogicalOperand(
    Generation *g, TargetGeneration *tg, TiObject *tgContext, Core::Basic::TiObject *astNode, Spp::Ast::Type *astType,
    TiObject *tgValue, TioSharedPtr &result
  );

  /// @}

}; // class

} } // namespace

#endif
