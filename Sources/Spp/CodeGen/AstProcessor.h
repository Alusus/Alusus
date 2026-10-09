/**
 * @file Spp/CodeGen/AstProcessor.h
 * Contains the header of class Spp::CodeGen::AstProcessor.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_MACROPROCESSOR_H
#define SPP_CODEGEN_MACROPROCESSOR_H

namespace Spp::CodeGen
{

class AstProcessor : public TiObject, public DynamicBinding, public DynamicInterfacing
{
  //============================================================================
  // Type Info

  TYPE_INFO(AstProcessor, TiObject, "Spp.CodeGen", "Spp", "alusus.org", (
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
  private: Executing *executing;
  private: ExpressionComputation *expressionComputation;
  private: SharedList<Core::Ast::Node> *astNodeRepo;
  private: Core::Ast::Node *currentPreprocessOwner;
  private: Int currentPreprocessInsertionPosition;
  private: SharedPtr<Core::Ast::SourceLocation> currentPreprocessSourceLocation;


  //============================================================================
  // Constructors & Destructor

  public: AstProcessor(
    Ast::Helper *h, Ast::CalleeTracer *ct, Executing *b, ExpressionComputation *ec,
    SharedList<Core::Ast::Node> *anr
  ) : astHelper(h), calleeTracer(ct), executing(b), expressionComputation(ec), astNodeRepo(anr)
  {
    this->initBindingCaches();
    this->initBindings();
  }

  public: AstProcessor(AstProcessor *parent)
  {
    this->initBindingCaches();
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);
    this->astHelper = parent->getAstHelper();
    this->calleeTracer = parent->getCalleeTracer();
    this->executing = parent->getExecuting();
    this->expressionComputation = parent->getExpressionComputation();
    this->astNodeRepo = parent->getAstNodeRepo();
  }

  public: virtual ~AstProcessor()
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

  public: Executing* getExecuting() const
  {
    return this->executing;
  }

  public: ExpressionComputation* getExpressionComputation() const
  {
    return this->expressionComputation;
  }

  public: SharedList<Core::Ast::Node>* getAstNodeRepo() const
  {
    return this->astNodeRepo;
  }

  public: Core::Ast::Node* getCurrentPreprocessOwner() const
  {
    return this->currentPreprocessOwner;
  }

  public: Int getCurrentPreprocessInsertionPosition() const
  {
    return this->currentPreprocessInsertionPosition;
  }

  public: SharedPtr<Core::Ast::SourceLocation> getCurrentPreprocessSourceLocation() const
  {
    return this->currentPreprocessSourceLocation;
  }

  /// @}

  /// @name Code Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(process, Bool, (Core::Ast::Node* /* owner */));
  private: static Bool _process(TiObject *self, Core::Ast::Node *owner);

  public: METHOD_BINDING_CACHE(processParamPass,
    Bool, (Core::Ast::ParamPass* /* paramPass */, TiInt /* indexInOwner */, Bool& /* replaced */)
  );
  private: static Bool _processParamPass(
    TiObject *self, Core::Ast::ParamPass *paramPass, TiInt indexInOwner, Bool &replaced
  );

  public: METHOD_BINDING_CACHE(processPreprocessStatement,
    Bool, (
      Spp::Ast::PreprocessStatement* /* preprocessl */, Core::Ast::Node* /* owner */, TiInt /* indexInOwner */
    )
  );
  private: static Bool _processPreprocessStatement(
    TiObject *self, Spp::Ast::PreprocessStatement *preprocess, Core::Ast::Node *owner, TiInt indexInOwner
  );

  /**
   * @brief Process any unprocessed modifiers attached to the given node.
   *
   * For each modifier in `node`'s modifier list, looks for a directly accessible function named after the
   * modifier's keyword and matching the signature `function (node: ref[Core.Ast.Node], modifierParams:
   * ref[Core.Ast.Node])` (modifierParams is null if the modifier had no `[]` params). If one is found, it
   * gets built and called with the node and the modifier's params, and the modifier is then removed from
   * the list. Modifiers with no matching function raise a build error.
   */
  public: METHOD_BINDING_CACHE(processModifiers, Bool, (Containing<Core::Ast::Node>* /* container */, Int /* indexInContainer */));
  private: static Bool _processModifiers(TiObject *self, Containing<Core::Ast::Node> *container, Int indexInContainer);

  public: METHOD_BINDING_CACHE(processFunctionBody, Bool, (Spp::Ast::Function* /* func */));
  private: static Bool _processFunctionBody(TiObject *self, Spp::Ast::Function *func);

  public: METHOD_BINDING_CACHE(processTypeBody, Bool, (Spp::Ast::UserType* /* type */));
  private: static Bool _processTypeBody(TiObject *self, Spp::Ast::UserType *type);

  public: METHOD_BINDING_CACHE(processMacro,
    Bool, (
      Spp::Ast::Macro* /* macro */, Containing<Core::Ast::Node>* /* args */,
      Core::Ast::Node* /* owner */, TiInt /* indexInOwner */, Core::Ast::SourceLocation* /* sl */
    )
  );
  private: static Bool _processMacro(
    TiObject *self, Spp::Ast::Macro *macro, Containing<Core::Ast::Node> *args,
    Core::Ast::Node *owner, TiInt indexInOwner, Core::Ast::SourceLocation *sl
  );

  public: METHOD_BINDING_CACHE(applyMacroArgs,
    Bool, (
      Spp::Ast::Macro* /* macro */, Containing<Core::Ast::Node>* /* args */, Core::Ast::SourceLocation* /* sl */,
      SharedPtr<Core::Ast::Node>& /* result */
    )
  );
  private: static Bool _applyMacroArgs(
    TiObject *self, Spp::Ast::Macro *macro, Containing<Core::Ast::Node> *args, Core::Ast::SourceLocation *sl,
    SharedPtr<Core::Ast::Node> &result
  );

  public: METHOD_BINDING_CACHE(insertInterpolatedAst,
    Bool, (
      Core::Ast::Node* /* obj */, Array<Str> const* /* argNames */, Containing<Core::Ast::Node>* /* args */
    )
  );
  private: static Bool _insertInterpolatedAst(
    TiObject *self, Core::Ast::Node *obj, Array<Str> const *argNames, Containing<Core::Ast::Node> *args
  );

  public: METHOD_BINDING_CACHE(interpolateAst,
    Bool, (
      Core::Ast::Node* /* obj */, Array<Str> const* /* argNames */,
      Containing<Core::Ast::Node>* /* args */, Core::Ast::SourceLocation* /* sl */,
      SharedPtr<Core::Ast::Node>& /* result */
    )
  );
  private: static Bool _interpolateAst(
    TiObject *self, Core::Ast::Node *obj, Array<Str> const *argNames,
    Containing<Core::Ast::Node> *args, Core::Ast::SourceLocation *sl,
    SharedPtr<Core::Ast::Node> &result
  );

  public: METHOD_BINDING_CACHE(interpolateAst_identifier,
    Bool, (
      Core::Ast::Identifier* /* obj */, Array<Str> const* /* argNames */,
      Containing<Core::Ast::Node>* /* args */, Core::Ast::SourceLocation* /* sl */,
      SharedPtr<Core::Ast::Node>& /* result */
    )
  );
  private: static Bool _interpolateAst_identifier(
    TiObject *self, Core::Ast::Identifier *obj, Array<Str> const *argNames,
    Containing<Core::Ast::Node> *args, Core::Ast::SourceLocation *sl,
    SharedPtr<Core::Ast::Node> &result
  );

  public: METHOD_BINDING_CACHE(interpolateAst_stringLiteral,
    Bool, (
      Core::Ast::StringLiteral* /* obj */, Array<Str> const* /* argNames */,
      Containing<Core::Ast::Node>* /* args */, Core::Ast::SourceLocation* /* sl */,
      SharedPtr<Core::Ast::Node>& /* result */
    )
  );
  private: static Bool _interpolateAst_stringLiteral(
    TiObject *self, Core::Ast::StringLiteral *obj, Array<Str> const *argNames,
    Containing<Core::Ast::Node> *args, Core::Ast::SourceLocation *sl,
    SharedPtr<Core::Ast::Node> &result
  );

  public: METHOD_BINDING_CACHE(interpolateAst_other,
    Bool, (
      Core::Ast::Node* /* obj */, Array<Str> const* /* argNames */,
      Containing<Core::Ast::Node>* /* args */, Core::Ast::SourceLocation* /* sl */,
      SharedPtr<Core::Ast::Node>& /* result */
    )
  );
  private: static Bool _interpolateAst_other(
    TiObject *self, Core::Ast::Node *obj, Array<Str> const *argNames,
    Containing<Core::Ast::Node> *args, Core::Ast::SourceLocation *sl,
    SharedPtr<Core::Ast::Node> &result
  );

  public: METHOD_BINDING_CACHE(interpolateAst_binding,
    Bool, (
      Binding* /* obj */, Array<Str> const* /* argNames */, Containing<Core::Ast::Node>* /* args */,
      Core::Ast::SourceLocation* /* sl */, Binding* /* destObj */
    )
  );
  private: static Bool _interpolateAst_binding(
    TiObject *self, Binding *obj, Array<Str> const *argNames, Containing<Core::Ast::Node> *args,
    Core::Ast::SourceLocation *sl, Binding *destObj
  );

  public: METHOD_BINDING_CACHE(interpolateAst_containing,
    Bool, (
      Containing<Core::Ast::Node>* /* obj */, Array<Str> const* /* argNames */, Containing<Core::Ast::Node>* /* args */,
      Core::Ast::SourceLocation* /* sl */, Containing<Core::Ast::Node>* /* destObj */
    )
  );
  private: static Bool _interpolateAst_containing(
    TiObject *self, Containing<Core::Ast::Node> *obj, Array<Str> const *argNames, Containing<Core::Ast::Node> *args,
    Core::Ast::SourceLocation *sl, Containing<Core::Ast::Node> *destObj
  );

  public: METHOD_BINDING_CACHE(interpolateAst_dynContaining,
    Bool, (
      DynamicContaining<Core::Ast::Node>* /* obj */, Array<Str> const* /* argNames */, Containing<Core::Ast::Node>* /* args */,
      Core::Ast::SourceLocation* /* sl */, DynamicContaining<Core::Ast::Node>* /* destObj */
    )
  );
  private: static Bool _interpolateAst_dynContaining(
    TiObject *self, DynamicContaining<Core::Ast::Node> *obj, Array<Str> const *argNames, Containing<Core::Ast::Node> *args,
    Core::Ast::SourceLocation *sl, DynamicContaining<Core::Ast::Node> *destObj
  );

  public: METHOD_BINDING_CACHE(interpolateAst_dynMapContaining,
    Bool, (
      DynamicMapContaining<Core::Ast::Node>* /* obj */, Array<Str> const* /* argNames */, Containing<Core::Ast::Node>* /* args */,
      Core::Ast::SourceLocation* /* sl */, DynamicMapContaining<Core::Ast::Node>* /* destObj */
    )
  );
  private: static Bool _interpolateAst_dynMapContaining(
    TiObject *self, DynamicMapContaining<Core::Ast::Node> *obj, Array<Str> const *argNames, Containing<Core::Ast::Node> *args,
    Core::Ast::SourceLocation *sl, DynamicMapContaining<Core::Ast::Node> *destObj
  );

  /// @}

  /// @name Helper Functions
  /// @{

  private: void parseStringTemplate(
    Char const *str, Char *var, Word varBufSize, Word &prefixSize, Char const *&suffix,
    Char const *varOpening = S("__"), Char const *varClosing = S("__")
  );

  private: void generateStringFromTemplate(
    Char const *prefix, Word prefixSize, Char const *var, Char const *suffix, Char *output, Word outputBufSize
  );

  /// @}

}; // class

} // namespace

#endif
