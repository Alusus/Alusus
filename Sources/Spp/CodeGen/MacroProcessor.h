/**
 * @file Spp/CodeGen/MacroProcessor.h
 * Contains the header of class Spp::CodeGen::MacroProcessor.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
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

class MacroProcessor : public TiObject, public DynamicBinding, public DynamicInterfacing
{
  //============================================================================
  // Type Info

  TYPE_INFO(MacroProcessor, TiObject, "Spp.CodeGen", "Spp", "alusus.org", (
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

  public: MacroProcessor(Ast::Helper *h) : astHelper(h)
  {
    this->initBindingCaches();
    this->initBindings();
  }

  public: MacroProcessor(MacroProcessor *parent)
  {
    this->initBindingCaches();
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);
    this->astHelper = parent->getAstHelper();
  }

  public: virtual ~MacroProcessor()
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

  public: METHOD_BINDING_CACHE(runMacroPass, Bool, (Core::Data::Ast::Scope* /* root */));
  private: static Bool _runMacroPass(TiObject *self, Core::Data::Ast::Scope *root);

  public: METHOD_BINDING_CACHE(processMacros, Bool, (TiObject* /* owner */));
  private: static Bool _processMacros(TiObject *self, TiObject *owner);

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

  public: METHOD_BINDING_CACHE(applyMacroArgsIteration_clonable,
    Bool, (
      Core::Data::Clonable* /* obj */, Core::Data::Ast::Map* /* argTypes */, Containing<TiObject>* /* args */,
      Core::Data::SourceLocation* /* sl */, TioSharedPtr& /* result */
    )
  );
  private: static Bool _applyMacroArgsIteration_clonable(
    TiObject *self, Core::Data::Clonable *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
    Core::Data::SourceLocation *sl, TioSharedPtr &result
  );

  public: METHOD_BINDING_CACHE(applyMacroArgsIteration_binding,
    Bool, (
      Binding* /* obj */, Core::Data::Ast::Map* /* argTypes */, Containing<TiObject>* /* args */,
      Core::Data::SourceLocation* /* sl */
    )
  );
  private: static Bool _applyMacroArgsIteration_binding(
    TiObject *self, Binding *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
    Core::Data::SourceLocation *sl
  );

  public: METHOD_BINDING_CACHE(applyMacroArgsIteration_containing,
    Bool, (
      Containing<TiObject>* /* obj */, Core::Data::Ast::Map* /* argTypes */, Containing<TiObject>* /* args */,
      Core::Data::SourceLocation* /* sl */
    )
  );
  private: static Bool _applyMacroArgsIteration_containing(
    TiObject *self, Containing<TiObject> *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
    Core::Data::SourceLocation *sl
  );

  public: METHOD_BINDING_CACHE(applyMacroArgsIteration_mapContaining,
    Bool, (
      DynamicMapContaining<TiObject>* /* obj */, Core::Data::Ast::Map* /* argTypes */, Containing<TiObject>* /* args */,
      Core::Data::SourceLocation* /* sl */
    )
  );
  private: static Bool _applyMacroArgsIteration_mapContaining(
    TiObject *self, DynamicMapContaining<TiObject> *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
    Core::Data::SourceLocation *sl
  );

  /// @}

  /// @name Helper Functions
  /// @{

  private: TioSharedPtr cloneTree(TiObject *obj, Core::Data::SourceLocation *sl);

  private: void parseStringTemplate(
    Char const *str, Char *var, Word varBufSize, Word &prefixSize, Char const *&suffix,
    Char const *varOpening = S("__"), Char const *varClosing = S("__")
  );

  private: void generateStringFromTemplate(
    Char const *prefix, Word prefixSize, Char const *var, Char const *suffix, Char *output, Word outputBufSize
  );

  private: void addSourceLocation(TiObject *obj, Core::Data::SourceLocation *sl);

  /// @}

}; // class

} // namespace

#endif
