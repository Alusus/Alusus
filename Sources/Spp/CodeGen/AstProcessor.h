/**
 * @file Spp/CodeGen/AstProcessor.h
 * Contains the header of class Spp::CodeGen::AstProcessor.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
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
  private: Core::Notices::Store *noticeStore = 0;


  //============================================================================
  // Constructors & Destructor

  public: AstProcessor(Ast::Helper *h) : astHelper(h)
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

  public: void setNoticeStore(Core::Notices::Store *ns)
  {
    this->noticeStore = ns;
  }

  public: Core::Notices::Store* getNoticeStore() const
  {
    return this->noticeStore;
  }

  /// @}

  /// @name Code Generation Functions
  /// @{

  public: void preparePass(Core::Notices::Store *noticeStore);

  public: METHOD_BINDING_CACHE(runPass, Bool, (Core::Data::Ast::Scope* /* root */));
  private: static Bool _runPass(TiObject *self, Core::Data::Ast::Scope *root);

  public: METHOD_BINDING_CACHE(process, Bool, (TiObject* /* owner */));
  private: static Bool _process(TiObject *self, TiObject *owner);

  public: METHOD_BINDING_CACHE(processMemberFunction, Bool, (Spp::Ast::Function* /* func */));
  private: static Bool _processMemberFunction(TiObject *self, Spp::Ast::Function *func);

  public: METHOD_BINDING_CACHE(processMacro,
    Bool, (
      Spp::Ast::Macro* /* macro */, Containing<TiObject>* /* args */,
      TiObject* /* owner */, TiInt /* indexInOwner */, Core::Data::SourceLocation* /* sl */
    )
  );
  private: static Bool _processMacro(
    TiObject *self, Spp::Ast::Macro *macro, Containing<TiObject> *args,
    TiObject *owner, TiInt indexInOwner, Core::Data::SourceLocation *sl
  );

  public: METHOD_BINDING_CACHE(applyMacroArgs,
    Bool, (
      Spp::Ast::Macro* /* macro */, Containing<TiObject>* /* args */, Core::Data::SourceLocation* /* sl */,
      TioSharedPtr& /* result */
    )
  );
  private: static Bool _applyMacroArgs(
    TiObject *self, Spp::Ast::Macro *macro, Containing<TiObject> *args, Core::Data::SourceLocation *sl,
    TioSharedPtr &result
  );

  public: METHOD_BINDING_CACHE(applyMacroArgsIteration,
    Bool, (
      TiObject* /* obj */, Core::Data::Ast::Map* /* argTypes */, Containing<TiObject>* /* args */,
      Core::Data::SourceLocation* /* sl */, TioSharedPtr& /* result */
    )
  );
  private: static Bool _applyMacroArgsIteration(
    TiObject *self, TiObject *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
    Core::Data::SourceLocation *sl, TioSharedPtr &result
  );

  public: METHOD_BINDING_CACHE(applyMacroArgsIteration_identifier,
    Bool, (
      Core::Data::Ast::Identifier* /* obj */, Core::Data::Ast::Map* /* argTypes */, Containing<TiObject>* /* args */,
      Core::Data::SourceLocation* /* sl */, TioSharedPtr& /* result */
    )
  );
  private: static Bool _applyMacroArgsIteration_identifier(
    TiObject *self, Core::Data::Ast::Identifier *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
    Core::Data::SourceLocation *sl, TioSharedPtr &result
  );

  public: METHOD_BINDING_CACHE(applyMacroArgsIteration_stringLiteral,
    Bool, (
      Core::Data::Ast::StringLiteral* /* obj */, Core::Data::Ast::Map* /* argTypes */, Containing<TiObject>* /* args */,
      Core::Data::SourceLocation* /* sl */, TioSharedPtr& /* result */
    )
  );
  private: static Bool _applyMacroArgsIteration_stringLiteral(
    TiObject *self, Core::Data::Ast::StringLiteral *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
    Core::Data::SourceLocation *sl, TioSharedPtr &result
  );

  public: METHOD_BINDING_CACHE(applyMacroArgsIteration_tiStr,
    Bool, (
      TiStr* /* obj */, Core::Data::Ast::Map* /* argTypes */, Containing<TiObject>* /* args */,
      Core::Data::SourceLocation* /* sl */, TioSharedPtr& /* result */
    )
  );
  private: static Bool _applyMacroArgsIteration_tiStr(
    TiObject *self, TiStr *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
    Core::Data::SourceLocation *sl, TioSharedPtr &result
  );

  public: METHOD_BINDING_CACHE(applyMacroArgsIteration_other,
    Bool, (
      TiObject* /* obj */, Core::Data::Ast::Map* /* argTypes */, Containing<TiObject>* /* args */,
      Core::Data::SourceLocation* /* sl */, TioSharedPtr& /* result */
    )
  );
  private: static Bool _applyMacroArgsIteration_other(
    TiObject *self, TiObject *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
    Core::Data::SourceLocation *sl, TioSharedPtr &result
  );

  public: METHOD_BINDING_CACHE(applyMacroArgsIteration_binding,
    Bool, (
      Binding* /* obj */, Core::Data::Ast::Map* /* argTypes */, Containing<TiObject>* /* args */,
      Core::Data::SourceLocation* /* sl */, Binding* /* destObj */
    )
  );
  private: static Bool _applyMacroArgsIteration_binding(
    TiObject *self, Binding *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
    Core::Data::SourceLocation *sl, Binding *destObj
  );

  public: METHOD_BINDING_CACHE(applyMacroArgsIteration_containing,
    Bool, (
      Containing<TiObject>* /* obj */, Core::Data::Ast::Map* /* argTypes */, Containing<TiObject>* /* args */,
      Core::Data::SourceLocation* /* sl */, Containing<TiObject>* /* destObj */
    )
  );
  private: static Bool _applyMacroArgsIteration_containing(
    TiObject *self, Containing<TiObject> *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
    Core::Data::SourceLocation *sl, Containing<TiObject> *destObj
  );

  public: METHOD_BINDING_CACHE(applyMacroArgsIteration_dynContaining,
    Bool, (
      DynamicContaining<TiObject>* /* obj */, Core::Data::Ast::Map* /* argTypes */, Containing<TiObject>* /* args */,
      Core::Data::SourceLocation* /* sl */, DynamicContaining<TiObject>* /* destObj */
    )
  );
  private: static Bool _applyMacroArgsIteration_dynContaining(
    TiObject *self, DynamicContaining<TiObject> *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
    Core::Data::SourceLocation *sl, DynamicContaining<TiObject> *destObj
  );

  public: METHOD_BINDING_CACHE(applyMacroArgsIteration_dynMapContaining,
    Bool, (
      DynamicMapContaining<TiObject>* /* obj */, Core::Data::Ast::Map* /* argTypes */, Containing<TiObject>* /* args */,
      Core::Data::SourceLocation* /* sl */, DynamicMapContaining<TiObject>* /* destObj */
    )
  );
  private: static Bool _applyMacroArgsIteration_dynMapContaining(
    TiObject *self, DynamicMapContaining<TiObject> *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
    Core::Data::SourceLocation *sl, DynamicMapContaining<TiObject> *destObj
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
