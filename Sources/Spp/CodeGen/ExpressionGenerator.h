/**
 * @file Spp/CodeGen/ExpressionGenerator.h
 * Contains the header of class Spp::CodeGen::ExpressionGenerator.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_EXPRESSIONGENERATOR_H
#define SPP_CODEGEN_EXPRESSIONGENERATOR_H

namespace Spp { namespace CodeGen
{

class ExpressionGenerator : public TiObject, public DynamicBinding, public DynamicInterfacing
{
  //============================================================================
  // Type Info

  TYPE_INFO(ExpressionGenerator, TiObject, "Spp.CodeGen", "Spp", "alusus.org", (
    INHERITANCE_INTERFACES(DynamicBinding, DynamicInterfacing),
    OBJECT_INTERFACE_LIST(interfaceList)
  ));


  //============================================================================
  // Implementations

  IMPLEMENT_DYNAMIC_BINDINGS(bindingMap);
  IMPLEMENT_DYNAMIC_INTERFACING(interfaceList);


  //============================================================================
  // Member Variables

  private: Ast::Helper *astHelper;
  private: Core::Notices::Store *noticeStore = 0;
  private: Bool offlineExecution = false;


  //============================================================================
  // Constructors & Destructor

  public: ExpressionGenerator(Ast::Helper *h) : astHelper(h)
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

  public: void setNoticeStore(Core::Notices::Store *ns)
  {
    this->noticeStore = ns;
  }

  public: Core::Notices::Store* getNoticeStore() const
  {
    return this->noticeStore;
  }

  public: void setOfflineExecution(Bool oe)
  {
    this->offlineExecution = oe;
  }

  /// @}

  /// @name Top Level Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(generate,
    Bool, (
      TiObject* /* astNode */, Generation* /* g */, GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generate(
    TiObject *self, TiObject *astNode, Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateList,
    Bool, (
      Core::Data::Ast::List* /* astNode */, Generation* /* g */, GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateList(
    TiObject *self, Core::Data::Ast::List *astNode, Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateIdentifier,
    Bool, (
      Core::Data::Ast::Identifier* /* astNode */, Generation* /* g */,
      GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateIdentifier(
    TiObject *self, Core::Data::Ast::Identifier *astNode, Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateScopeMemberReference,
    Bool, (
      TiObject* /* scope */, Core::Data::Ast::Identifier* /* astNode */, Bool /* searchOwners */,
      Generation* /* g */, GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateScopeMemberReference(
    TiObject *self, TiObject *scope, Core::Data::Ast::Identifier *astNode, Bool searchOwners,
    Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateLinkOperator,
    Bool, (
      Core::Data::Ast::LinkOperator* /* astNode */, Generation* /* g */,
      GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateLinkOperator(
    TiObject *self, Core::Data::Ast::LinkOperator *astNode, Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateParamPass,
    Bool, (
      Core::Data::Ast::ParamPass* /* astNode */, Generation* /* g */,
      GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateParamPass(
    TiObject *self, Core::Data::Ast::ParamPass *astNode, Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateRoundParamPass,
    Bool, (
      Core::Data::Ast::ParamPass* /* astNode */, Generation* /* g */, GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateRoundParamPass(
    TiObject *self, Core::Data::Ast::ParamPass *astNode, Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateRoundParamPassOnCallee,
    Bool, (
      Core::Data::Node* /* astNode */, GenResult const& /* callee */, GenResult const& /* thisArg */,
      SharedList<TiObject>* /* paramTgValues */, PlainList<TiObject>* /* paramAstTypes */,
      PlainList<TiObject>* /* paramAstNodes */, Generation* /* g */, GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateRoundParamPassOnCallee(
    TiObject *self, Core::Data::Node *astNode, GenResult const &callee, GenResult const &thisArg,
    SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes, PlainList<TiObject> *paramAstNodes,
    Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateSquareParamPass,
    Bool, (
      Core::Data::Ast::ParamPass* /* astNode */, Generation* /* g */,
      GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateSquareParamPass(
    TiObject *self, Core::Data::Ast::ParamPass *astNode, Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateOperator,
    Bool, (
      Core::Data::Node* /* astNode */, Generation* /* g */,
      GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateOperator(
    TiObject *self, Core::Data::Node *astNode, Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateLogicalOp,
    Bool, (
      Core::Data::Ast::InfixOperator* /* astNode */, Generation* /* g */,
      GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateLogicalOp(
    TiObject *self, Core::Data::Ast::InfixOperator *astNode, Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateArithmeticOp,
    Bool, (
      Core::Data::Ast::InfixOperator* /* astNode */,
      SharedList<TiObject>* /* paramTgValues */, PlainList<TiObject>* /* paramAstTypes */,
      Generation* /* g */, GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateArithmeticOp(
    TiObject *self, Core::Data::Ast::InfixOperator *astNode,
    SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
    Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateBinaryOp,
    Bool, (
      Core::Data::Ast::InfixOperator* /* astNode */,
      SharedList<TiObject>* /* paramTgValues */, PlainList<TiObject>* /* paramAstTypes */,
      Generation* /* g */, GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateBinaryOp(
    TiObject *self, Core::Data::Ast::InfixOperator *astNode,
    SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
    Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateComparisonOp,
    Bool, (
      Core::Data::Ast::InfixOperator* /* astNode */,
      SharedList<TiObject>* /* paramTgValues */, PlainList<TiObject>* /* paramAstTypes */,
      Generation* /* g */, GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateComparisonOp(
    TiObject *self, Core::Data::Ast::InfixOperator *astNode,
    SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
    Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateAssignOp,
    Bool, (
      Core::Data::Ast::InfixOperator* /* astNode */,
      SharedList<TiObject>* /* paramTgValues */, PlainList<TiObject>* /* paramAstTypes */,
      Generation* /* g */, GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateAssignOp(
    TiObject *self, Core::Data::Ast::InfixOperator *astNode,
    SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
    Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateArithmeticAssignOp,
    Bool, (
      Core::Data::Ast::InfixOperator* /* astNode */,
      SharedList<TiObject>* /* paramTgValues */, PlainList<TiObject>* /* paramAstTypes */,
      Generation* /* g */, GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateArithmeticAssignOp(
    TiObject *self, Core::Data::Ast::InfixOperator *astNode,
    SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
    Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateBinaryAssignOp,
    Bool, (
      Core::Data::Ast::InfixOperator* /* astNode */,
      SharedList<TiObject>* /* paramTgValues */, PlainList<TiObject>* /* paramAstTypes */,
      Generation* /* g */, GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateBinaryAssignOp(
    TiObject *self, Core::Data::Ast::InfixOperator *astNode,
    SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
    Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateUnaryValOp,
    Bool, (
      Core::Data::Ast::OutfixOperator* /* astNode */,
      SharedList<TiObject>* /* paramTgValues */, PlainList<TiObject>* /* paramAstTypes */,
      Generation* /* g */, GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateUnaryValOp(
    TiObject *self, Core::Data::Ast::OutfixOperator *astNode,
    SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
    Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateIntUnaryValOp,
    Bool, (
      Core::Data::Ast::OutfixOperator* /* astNode */,
      SharedList<TiObject>* /* paramTgValues */, PlainList<TiObject>* /* paramAstTypes */,
      Generation* /* g */, GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateIntUnaryValOp(
    TiObject *self, Core::Data::Ast::OutfixOperator *astNode,
    SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
    Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateUnaryVarOp,
    Bool, (
      Core::Data::Ast::OutfixOperator* /* astNode */,
      SharedList<TiObject>* /* paramTgValues */, PlainList<TiObject>* /* paramAstTypes */,
      Generation* /* g */, GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateUnaryVarOp(
    TiObject *self, Core::Data::Ast::OutfixOperator *astNode,
    SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
    Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generatePointerOp,
    Bool, (
      Spp::Ast::PointerOp* /* astNode */, Generation* /* g */,
      GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generatePointerOp(
    TiObject *self, Spp::Ast::PointerOp *astNode, Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateAstRefOp,
    Bool, (
      Spp::Ast::AstRefOp* /* astNode */, Generation* /* g */,
      GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateAstRefOp(
    TiObject *self, Spp::Ast::AstRefOp *astNode, Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateContentOp,
    Bool, (
      Spp::Ast::ContentOp* /* astNode */, Generation* /* g */,
      GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateContentOp(
    TiObject *self, Spp::Ast::ContentOp *astNode, Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateCastOp,
    Bool, (
      Spp::Ast::CastOp* /* astNode */, Generation* /* g */,
      GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateCastOp(
    TiObject *self, Spp::Ast::CastOp *astNode, Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateSizeOp,
    Bool, (
      Spp::Ast::SizeOp* /* astNode */, Generation* /* g */,
      GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateSizeOp(
    TiObject *self, Spp::Ast::SizeOp *astNode, Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateInitOp,
    Bool, (
      Spp::Ast::InitOp* /* astNode */, Generation* /* g */,
      GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateInitOp(
    TiObject *self, Spp::Ast::InitOp *astNode, Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateTerminateOp,
    Bool, (
      Spp::Ast::TerminateOp* /* astNode */, Generation* /* g */,
      GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateTerminateOp(
    TiObject *self, Spp::Ast::TerminateOp *astNode, Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateStringLiteral,
    Bool, (
      Core::Data::Ast::StringLiteral* /* astNode */, Generation* /* g */,
      GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateStringLiteral(
    TiObject *self, Core::Data::Ast::StringLiteral *astNode, Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateCharLiteral,
    Bool, (
      Core::Data::Ast::CharLiteral* /* astNode */, Generation* /* g */,
      GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateCharLiteral(
    TiObject *self, Core::Data::Ast::CharLiteral *astNode, Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateIntegerLiteral,
    Bool, (
      Core::Data::Ast::IntegerLiteral* /* astNode */, Generation* /* g */,
      GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateIntegerLiteral(
    TiObject *self, Core::Data::Ast::IntegerLiteral *astNode, Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateFloatLiteral,
    Bool, (
      Core::Data::Ast::FloatLiteral* /* astNode */, Generation* /* g */,
      GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateFloatLiteral(
    TiObject *self, Core::Data::Ast::FloatLiteral *astNode, Generation *g, GenDeps const &deps, GenResult &result
  );

  /// @}

  /// @name Inner Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(generateVarReference,
    Bool, (
      TiObject* /* refAstNode */, TiObject* /* varAstNode */,
      Generation* /* g */, GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateVarReference(
    TiObject *self, TiObject *refAstNode, TiObject *varAstNode, Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateMemberReference,
    Bool, (
      TiObject* /* tgValue */, Ast::Type* /* astType */, Core::Data::Ast::Identifier* /* astNode */,
      Generation* /* g */, GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateMemberReference(
    TiObject *self, TiObject *tgValue, Ast::Type * astType, Core::Data::Ast::Identifier *astNode,
    Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateMemberVarReference,
    Bool, (
      Core::Data::Node* /* astNode */, TiObject* /* tgStructValue */, Ast::Type* /* astStructType */,
      TiObject* /* astMemberVar */, Generation* /* g */, GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateMemberVarReference(
    TiObject *self, Core::Data::Node *astNode, TiObject *tgStructValue, Ast::Type * astStructType, TiObject *astMemberVar,
    Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateArrayReference,
    Bool, (
      Core::Data::Node* /* astNode */, TiObject* /* tgValue */, Ast::Type* /* astType */, TiObject* /* tgIndexVal */,
      Ast::Type* /* astIndexType */, Generation* /* g */, GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateArrayReference(
    TiObject *self, Core::Data::Node *astNode, TiObject *tgValue, Ast::Type *astType, TiObject *tgIndexVal,
    Ast::Type *astIndexType, Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateFunctionCall,
    Bool, (
      Core::Data::Node* /* astNode */, Spp::Ast::Function* /* callee */,
      Containing<TiObject>* /* paramAstTypes */, Containing<TiObject>* /* paramTgValues */,
      Generation* /* g */, GenDeps const& /* deps */, GenResult& /* result */
    )
  );
  private: static Bool _generateFunctionCall(
    TiObject *self, Core::Data::Node *astNode, Spp::Ast::Function *callee,
    Containing<TiObject> *paramAstTypes, Containing<TiObject> *paramTgValues,
    Generation *g, GenDeps const &deps, GenResult &result
  );

  public: METHOD_BINDING_CACHE(prepareFunctionParams,
    Bool, (
      Spp::Ast::FunctionType* /* calleeType */, Generation* /* g */, GenDeps const& /* deps */,
      DynamicContaining<TiObject>* /* paramAstNodes */, DynamicContaining<TiObject>* /* paramAstTypes */,
      SharedList<TiObject>* /* paramTgVals */
    )
  );
  private: static Bool _prepareFunctionParams(
    TiObject *self, Spp::Ast::FunctionType *calleeType, Generation *g, GenDeps const &deps,
    DynamicContaining<TiObject> *paramAstNodes, DynamicContaining<TiObject> *paramAstTypes,
    SharedList<TiObject> *paramTgVals
  );

  public: METHOD_BINDING_CACHE(generateCalleeReferenceChain,
    Bool, (
      Ast::CalleeLookupResult const& /* calleeInfo */, Core::Data::Node* /* astNode */,
      GenResult const& /* prevResult */, Generation* /* g */, GenDeps const& /* deps */,
      GenResult& /* calleeResult */, GenResult& /* thisResult */
    )
  );
  private: static Bool _generateCalleeReferenceChain(
    TiObject *self, Ast::CalleeLookupResult const &calleeInfo, Core::Data::Node *astNode, GenResult const &prevResult,
    Generation *g, GenDeps const &deps, GenResult &calleeResult, GenResult &thisResult
  );

  /// @}

  /// @name Helper Functions
  /// @{

  private: Bool generateParamList(
    TiObject *astNode, Generation *g, GenDeps const &deps,
    DynamicContaining<TiObject> *resultAstNodes, DynamicContaining<TiObject> *resultTypes, SharedList<TiObject> *resultValues
  );

  private: Bool generateParamList(
    Containing<TiObject> *astNodes, Generation *g, GenDeps const &deps,
    DynamicContaining<TiObject> *resultAstNodes, DynamicContaining<TiObject> *resultTypes, SharedList<TiObject> *resultValues
  );

  public: Bool dereferenceIfNeeded(
    Spp::Ast::Type *astType, TiObject *tgValue, Bool valueNeeded, GenDeps const &deps, GenResult &result
  );

  private: Bool castLogicalOperand(
    Generation *g, GenDeps const &deps, TiObject *astNode, Spp::Ast::Type *astType,
    TiObject *tgValue, TioSharedPtr &result
  );

  /// @}

}; // class

} } // namespace

#endif
