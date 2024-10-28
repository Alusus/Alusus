/**
 * @file Spp/CodeGen/ExpressionGenerator.h
 * Contains the header of class Spp::CodeGen::ExpressionGenerator.
 *
 * @copyright Copyright (C) 2024 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_EXPRESSIONGENERATOR_H
#define SPP_CODEGEN_EXPRESSIONGENERATOR_H

namespace Spp::CodeGen
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
  private: Ast::CalleeTracer *calleeTracer;
  private: SharedList<TiObject> *astNodeRepo;


  //============================================================================
  // Constructors & Destructor

  public: ExpressionGenerator(Ast::Helper *h, Ast::CalleeTracer *t, SharedList<TiObject> *anr)
    : astHelper(h), calleeTracer(t), astNodeRepo(anr)
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
    this->calleeTracer = parent->getCalleeTracer();
    this->astNodeRepo = parent->getAstNodeRepo();
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

  public: Ast::CalleeTracer* getCalleeTracer() const
  {
    return this->calleeTracer;
  }

  public: SharedList<TiObject>* getAstNodeRepo() const
  {
    return this->astNodeRepo;
  }

  /// @}

  /// @name Top Level Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(generate,
    Bool, (
      TiObject* /* astNode */, Generation* /* g */, Session* /* session */, GenResult& /* result */,
      TerminalStatement& /* terminal */
    )
  );
  private: static Bool _generate(
    TiObject *self, TiObject *astNode, Generation *g, Session *session, GenResult &result, TerminalStatement &terminal
  );

  public: METHOD_BINDING_CACHE(generateList,
    Bool, (
      Core::Data::Ast::List* /* astNode */, Generation* /* g */, Session* /* session */, GenResult& /* result */,
      TerminalStatement& /* terminal */
    )
  );
  private: static Bool _generateList(
    TiObject *self, Core::Data::Ast::List *astNode, Generation *g, Session *session, GenResult &result,
    TerminalStatement &terminal
  );

  public: METHOD_BINDING_CACHE(generateIdentifier,
    Bool, (
      Core::Data::Ast::Identifier* /* astNode */, Generation* /* g */,
      Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateIdentifier(
    TiObject *self, Core::Data::Ast::Identifier *astNode, Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateCalleePointer,
    Bool, (
      Spp::Ast::CalleePointer* /* astNode */, Generation* /* g */,
      Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateCalleePointer(
    TiObject *self, Spp::Ast::CalleePointer *astNode, Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateLinkOperator,
    Bool, (
      Core::Data::Ast::LinkOperator* /* astNode */, Generation* /* g */,
      Session* /* session */, GenResult& /* result */, TerminalStatement& /* terminal */
    )
  );
  private: static Bool _generateLinkOperator(
    TiObject *self, Core::Data::Ast::LinkOperator *astNode, Generation *g, Session *session, GenResult &result,
    TerminalStatement &terminal
  );

  public: METHOD_BINDING_CACHE(generateParamPass,
    Bool, (
      Core::Data::Ast::ParamPass* /* astNode */, Generation* /* g */,
      Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateParamPass(
    TiObject *self, Core::Data::Ast::ParamPass *astNode, Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateRoundParamPass,
    Bool, (
      Core::Data::Ast::ParamPass* /* astNode */, Generation* /* g */, Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateRoundParamPass(
    TiObject *self, Core::Data::Ast::ParamPass *astNode, Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateRoundParamPassOnCallee,
    Bool, (
      Core::Data::Node* /* astNode */, GenResult const& /* callee */, GenResult const& /* thisArg */,
      SharedList<TiObject>* /* paramTgValues */, PlainList<TiObject>* /* paramAstTypes */,
      PlainList<TiObject>* /* paramAstNodes */, Generation* /* g */, Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateRoundParamPassOnCallee(
    TiObject *self, Core::Data::Node *astNode, GenResult const &callee, GenResult const &thisArg,
    SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes, PlainList<TiObject> *paramAstNodes,
    Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateSquareParamPass,
    Bool, (
      Core::Data::Ast::ParamPass* /* astNode */, Generation* /* g */,
      Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateSquareParamPass(
    TiObject *self, Core::Data::Ast::ParamPass *astNode, Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateOperator,
    Bool, (
      Core::Data::Node* /* astNode */, Generation* /* g */,
      Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateOperator(
    TiObject *self, Core::Data::Node *astNode, Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateLogicalOp,
    Bool, (
      Core::Data::Ast::InfixOperator* /* astNode */, Generation* /* g */,
      Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateLogicalOp(
    TiObject *self, Core::Data::Ast::InfixOperator *astNode, Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateArithmeticOp,
    Bool, (
      Core::Data::Ast::InfixOperator* /* astNode */,
      SharedList<TiObject>* /* paramTgValues */, PlainList<TiObject>* /* paramAstTypes */,
      Generation* /* g */, Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateArithmeticOp(
    TiObject *self, Core::Data::Ast::InfixOperator *astNode,
    SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
    Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateBinaryOp,
    Bool, (
      Core::Data::Ast::InfixOperator* /* astNode */,
      SharedList<TiObject>* /* paramTgValues */, PlainList<TiObject>* /* paramAstTypes */,
      Generation* /* g */, Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateBinaryOp(
    TiObject *self, Core::Data::Ast::InfixOperator *astNode,
    SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
    Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateComparisonOp,
    Bool, (
      Core::Data::Ast::InfixOperator* /* astNode */,
      SharedList<TiObject>* /* paramTgValues */, PlainList<TiObject>* /* paramAstTypes */,
      Generation* /* g */, Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateComparisonOp(
    TiObject *self, Core::Data::Ast::InfixOperator *astNode,
    SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
    Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateAssignOp,
    Bool, (
      Core::Data::Ast::InfixOperator* /* astNode */,
      SharedList<TiObject>* /* paramTgValues */, PlainList<TiObject>* /* paramAstTypes */,
      Generation* /* g */, Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateAssignOp(
    TiObject *self, Core::Data::Ast::InfixOperator *astNode,
    SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
    Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateArithmeticAssignOp,
    Bool, (
      Core::Data::Ast::InfixOperator* /* astNode */,
      SharedList<TiObject>* /* paramTgValues */, PlainList<TiObject>* /* paramAstTypes */,
      Generation* /* g */, Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateArithmeticAssignOp(
    TiObject *self, Core::Data::Ast::InfixOperator *astNode,
    SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
    Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateBinaryAssignOp,
    Bool, (
      Core::Data::Ast::InfixOperator* /* astNode */,
      SharedList<TiObject>* /* paramTgValues */, PlainList<TiObject>* /* paramAstTypes */,
      Generation* /* g */, Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateBinaryAssignOp(
    TiObject *self, Core::Data::Ast::InfixOperator *astNode,
    SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
    Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateUnaryValOp,
    Bool, (
      Core::Data::Ast::OutfixOperator* /* astNode */,
      SharedList<TiObject>* /* paramTgValues */, PlainList<TiObject>* /* paramAstTypes */,
      Generation* /* g */, Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateUnaryValOp(
    TiObject *self, Core::Data::Ast::OutfixOperator *astNode,
    SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
    Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateIntUnaryValOp,
    Bool, (
      Core::Data::Ast::OutfixOperator* /* astNode */,
      SharedList<TiObject>* /* paramTgValues */, PlainList<TiObject>* /* paramAstTypes */,
      Generation* /* g */, Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateIntUnaryValOp(
    TiObject *self, Core::Data::Ast::OutfixOperator *astNode,
    SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
    Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateUnaryVarOp,
    Bool, (
      Core::Data::Ast::OutfixOperator* /* astNode */,
      SharedList<TiObject>* /* paramTgValues */, PlainList<TiObject>* /* paramAstTypes */,
      Generation* /* g */, Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateUnaryVarOp(
    TiObject *self, Core::Data::Ast::OutfixOperator *astNode,
    SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
    Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generatePointerOp,
    Bool, (
      Spp::Ast::PointerOp* /* astNode */, Generation* /* g */,
      Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generatePointerOp(
    TiObject *self, Spp::Ast::PointerOp *astNode, Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateAstRefOp,
    Bool, (
      Spp::Ast::AstRefOp* /* astNode */, Generation* /* g */,
      Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateAstRefOp(
    TiObject *self, Spp::Ast::AstRefOp *astNode, Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateAstLiteralCommand,
    Bool, (
      Spp::Ast::AstLiteralCommand* /* astNode */, Generation* /* g */,
      Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateAstLiteralCommand(
    TiObject *self, Spp::Ast::AstLiteralCommand *astNode, Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateContentOp,
    Bool, (
      Spp::Ast::ContentOp* /* astNode */, Generation* /* g */,
      Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateContentOp(
    TiObject *self, Spp::Ast::ContentOp *astNode, Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateDerefOp,
    Bool, (
      Spp::Ast::DerefOp* /* astNode */, Generation* /* g */,
      Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateDerefOp(
    TiObject *self, Spp::Ast::DerefOp *astNode, Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateNoDerefOp,
    Bool, (
      Spp::Ast::NoDerefOp* /* astNode */, Generation* /* g */,
      Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateNoDerefOp(
    TiObject *self, Spp::Ast::NoDerefOp *astNode, Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateCastOp,
    Bool, (
      Spp::Ast::CastOp* /* astNode */, Generation* /* g */,
      Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateCastOp(
    TiObject *self, Spp::Ast::CastOp *astNode, Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateSizeOp,
    Bool, (
      Spp::Ast::SizeOp* /* astNode */, Generation* /* g */,
      Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateSizeOp(
    TiObject *self, Spp::Ast::SizeOp *astNode, Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateInitOp,
    Bool, (
      Spp::Ast::InitOp* /* astNode */, Generation* /* g */,
      Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateInitOp(
    TiObject *self, Spp::Ast::InitOp *astNode, Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateTerminateOp,
    Bool, (
      Spp::Ast::TerminateOp* /* astNode */, Generation* /* g */,
      Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateTerminateOp(
    TiObject *self, Spp::Ast::TerminateOp *astNode, Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateNextArgOp,
    Bool, (
      Spp::Ast::NextArgOp* /* astNode */, Generation* /* g */, Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateNextArgOp(
    TiObject *self, Spp::Ast::NextArgOp *astNode, Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateUseInOp,
    Bool, (
      Spp::Ast::UseInOp* /* astNode */, Generation* /* g */,
      Session* /* session */, GenResult& /* result */,
      TerminalStatement& /* terminal */
    )
  );
  private: static Bool _generateUseInOp(
    TiObject *self, Spp::Ast::UseInOp *astNode, Generation *g, Session *session, GenResult &result,
    TerminalStatement &terminal
  );

  public: METHOD_BINDING_CACHE(generateStringLiteral,
    Bool, (
      Core::Data::Ast::StringLiteral* /* astNode */, Generation* /* g */,
      Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateStringLiteral(
    TiObject *self, Core::Data::Ast::StringLiteral *astNode, Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateCharLiteral,
    Bool, (
      Core::Data::Ast::CharLiteral* /* astNode */, Generation* /* g */,
      Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateCharLiteral(
    TiObject *self, Core::Data::Ast::CharLiteral *astNode, Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateIntegerLiteral,
    Bool, (
      Core::Data::Ast::IntegerLiteral* /* astNode */, Generation* /* g */,
      Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateIntegerLiteral(
    TiObject *self, Core::Data::Ast::IntegerLiteral *astNode, Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateFloatLiteral,
    Bool, (
      Core::Data::Ast::FloatLiteral* /* astNode */, Generation* /* g */,
      Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateFloatLiteral(
    TiObject *self, Core::Data::Ast::FloatLiteral *astNode, Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateInnerFunction,
    Bool, (
      Spp::Ast::Function* /* astFunction */, Generation* /* g */,
      Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateInnerFunction(
    TiObject *self, Spp::Ast::Function *astFunction, Generation *g, Session *session, GenResult &result
  );

  /// @}

  /// @name Inner Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(generateReferenceToNonObjectMember,
    Bool, (
      TiObject* /* obj */, Core::Data::Node* /* astNode */,
      Generation* /* g */, Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateReferenceToNonObjectMember(
    TiObject *self, TiObject *obj, Core::Data::Node *astNode, Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateVarReference,
    Bool, (
      TiObject* /* refAstNode */, TiObject* /* varAstNode */,
      Generation* /* g */, Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateVarReference(
    TiObject *self, TiObject *refAstNode, TiObject *varAstNode, Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateMemberVarReference,
    Bool, (
      Core::Data::Node* /* astNode */, TiObject* /* tgStructValue */, Ast::Type* /* astStructType */,
      TiObject* /* astMemberVar */, Generation* /* g */, Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateMemberVarReference(
    TiObject *self, Core::Data::Node *astNode, TiObject *tgStructValue, Ast::Type * astStructType, TiObject *astMemberVar,
    Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateArrayReference,
    Bool, (
      Core::Data::Node* /* astNode */, TiObject* /* tgValue */, Ast::Type* /* astType */, TiObject* /* tgIndexVal */,
      Ast::Type* /* astIndexType */, Generation* /* g */, Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateArrayReference(
    TiObject *self, Core::Data::Node *astNode, TiObject *tgValue, Ast::Type *astType, TiObject *tgIndexVal,
    Ast::Type *astIndexType, Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateFunctionCall,
    Bool, (
      Core::Data::Node* /* astNode */, Spp::Ast::Function* /* callee */,
      Containing<TiObject>* /* paramAstTypes */, Containing<TiObject>* /* paramTgValues */,
      Generation* /* g */, Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateFunctionCall(
    TiObject *self, Core::Data::Node *astNode, Spp::Ast::Function *callee,
    Containing<TiObject> *paramAstTypes, Containing<TiObject> *paramTgValues,
    Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateFunctionPtrCall,
    Bool, (
      Core::Data::Node* /* astNode */, Spp::Ast::FunctionType* /* astFuncType */,
      TiObject* /* tgFuncPtr */, TiObject* /* tgFuncPtrType */, Containing<TiObject>* /* paramTgValues */,
      Generation* /* g */, Session* /* session */, GenResult& /* result */
    )
  );
  private: static Bool _generateFunctionPtrCall(
    TiObject *self, Core::Data::Node *astNode, Spp::Ast::FunctionType *astFuncType,
    TiObject *tgFuncPtr, TiObject *tgFuncPtrType, Containing<TiObject> *paramTgValues,
    Generation *g, Session *session, GenResult &result
  );

  public: METHOD_BINDING_CACHE(prepareFunctionParams,
    Bool, (
      Spp::Ast::FunctionType* /* calleeType */, Generation* /* g */, Session* /* session */,
      DynamicContaining<TiObject>* /* paramAstNodes */, DynamicContaining<TiObject>* /* paramAstTypes */,
      SharedList<TiObject>* /* paramTgVals */
    )
  );
  private: static Bool _prepareFunctionParams(
    TiObject *self, Spp::Ast::FunctionType *calleeType, Generation *g, Session *session,
    DynamicContaining<TiObject> *paramAstNodes, DynamicContaining<TiObject> *paramAstTypes,
    SharedList<TiObject> *paramTgVals
  );

  public: METHOD_BINDING_CACHE(prepareCallee,
    Bool, (
      Core::Data::Node* /* astNode */, Containing<TiObject>* /* argTypes */, Char const* /* op */,
      Generation* /* g */, Session* /* session */, GenResult& /* calleeResult */, GenResult& /* thisResult */,
      TerminalStatement& /* terminal */
    )
  );
  private: static Bool _prepareCallee(
    TiObject *self, Core::Data::Node *astNode, Containing<TiObject> *argTypes, Char const *op,
    Generation *g, Session *session, GenResult &calleeResult, GenResult &thisResult,
    TerminalStatement &terminal
  );

  public: METHOD_BINDING_CACHE(prepareCalleeLookupRequest,
    Bool, (
      TiObject* /* operand */, Generation* /* g */, Session* /* session */,
      GenResult& /* prevResult */, Ast::CalleeLookupRequest& /* calleeRequest */,
      TerminalStatement& /* terminal */
    )
  );
  private: static Bool _prepareCalleeLookupRequest(
    TiObject *self, TiObject *operand, Generation *g, Session *session,
    GenResult &prevResult, Ast::CalleeLookupRequest &calleeRequest,
    TerminalStatement &terminal
  );

  public: METHOD_BINDING_CACHE(generateCalleeReferenceChain,
    Bool, (
      Ast::CalleeLookupResult const& /* calleeInfo */, Core::Data::Node* /* astNode */,
      GenResult const& /* prevResult */, Generation* /* g */, Session* /* session */,
      GenResult& /* calleeResult */, GenResult& /* thisResult */
    )
  );
  private: static Bool _generateCalleeReferenceChain(
    TiObject *self, Ast::CalleeLookupResult const &calleeInfo, Core::Data::Node *astNode, GenResult const &prevResult,
    Generation *g, Session *session, GenResult &calleeResult, GenResult &thisResult
  );

  public: METHOD_BINDING_CACHE(referencifyThisIfNeeded,
    Bool, (
      Core::Data::Node* /* astNode */, GenResult const& /* thisArg */,
      Generation* /* g */, Session* /* session */, GenResult& /* thisResult */
    )
  );
  private: static Bool _referencifyThisIfNeeded(
    TiObject *self, Core::Data::Node *astNode, GenResult const &thisArg,
    Generation *g, Session *session, GenResult &thisResult
  );

  public: METHOD_BINDING_CACHE(generateParams,
    Bool, (
      TiObject* /* astNode */, Generation* /* g */, Session* /* session */,
      DynamicContaining<TiObject>* /* resultAstNodes */, DynamicContaining<TiObject>* /* resultTypes */,
      SharedList<TiObject>* /* resultValues */
    )
  );
  private: static Bool _generateParams(
    TiObject *self, TiObject *astNode, Generation *g, Session *session,
    DynamicContaining<TiObject> *resultAstNodes, DynamicContaining<TiObject> *resultTypes,
    SharedList<TiObject> *resultValues
  );

  /// @}

  /// @name Helper Functions
  /// @{

  public: Bool dereferenceIfNeeded(
    Spp::Ast::Type *astType, TiObject *tgValue, Bool valueNeeded, Bool implicitOnly, Session *session, GenResult &result
  );

  private: Bool castLogicalOperand(
    Generation *g, Session *session, TiObject *astNode, Spp::Ast::Type *astType,
    TiObject *tgValue, TioSharedPtr &result
  );

  /// @}

}; // class

} // namespace

#endif
