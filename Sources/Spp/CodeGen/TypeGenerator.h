/**
 * @file Spp/CodeGen/TypeGenerator.h
 * Contains the header of class Spp::CodeGen::TypeGenerator.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_TYPEGENERATOR_H
#define SPP_CODEGEN_TYPEGENERATOR_H

namespace Spp { namespace CodeGen
{

class TypeGenerator : public TiObject, public DynamicBinding, public DynamicInterfacing
{
  //============================================================================
  // Type Info

  TYPE_INFO(TypeGenerator, TiObject, "Spp.CodeGen", "Spp", "alusus.org", (
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

  public: TypeGenerator(Ast::Helper *h) : astHelper(h)
  {
    this->initBindingCaches();
    this->initBindings();
  }

  public: TypeGenerator(TypeGenerator *parent)
  {
    this->initBindingCaches();
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);
    this->astHelper = parent->getAstHelper();
  }

  public: virtual ~TypeGenerator()
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

  /// @name Main Operation Functions
  /// @{

  public: Bool getGeneratedType(
    TiObject *ref, TargetGeneration *tg, TiObject *&targetTypeResult, Ast::Type **astTypeResult
  );

  public: Bool getGeneratedVoidType(
    TargetGeneration *tg, TiObject *&tgTypeResult, Ast::VoidType **astTypeResult
  );

  private: Bool _getGeneratedType(TiObject *ref, TargetGeneration *tg, Spp::Ast::Type *&type);

  /// @}

  /// @name Code Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(generateType, Bool, (Spp::Ast::Type*, TargetGeneration*));
  private: static Bool _generateType(TiObject *self, Spp::Ast::Type *astType, TargetGeneration *tg);

  public: METHOD_BINDING_CACHE(generateVoidType, Bool, (Spp::Ast::VoidType*, TargetGeneration*));
  private: static Bool _generateVoidType(TiObject *self, Spp::Ast::VoidType *astType, TargetGeneration *tg);

  public: METHOD_BINDING_CACHE(generateIntegerType, Bool, (Spp::Ast::IntegerType*, TargetGeneration*));
  private: static Bool _generateIntegerType(TiObject *self, Spp::Ast::IntegerType *astType, TargetGeneration *tg);

  public: METHOD_BINDING_CACHE(generateFloatType, Bool, (Spp::Ast::FloatType*, TargetGeneration*));
  private: static Bool _generateFloatType(TiObject *self, Spp::Ast::FloatType *astType, TargetGeneration *tg);

  public: METHOD_BINDING_CACHE(generatePointerType, Bool, (Spp::Ast::PointerType*, TargetGeneration*));
  private: static Bool _generatePointerType(TiObject *self, Spp::Ast::PointerType *astType, TargetGeneration *tg);

  public: METHOD_BINDING_CACHE(generateArrayType, Bool, (Spp::Ast::ArrayType*, TargetGeneration*));
  private: static Bool _generateArrayType(TiObject *self, Spp::Ast::ArrayType *astType, TargetGeneration *tg);

  public: METHOD_BINDING_CACHE(generateUserType, Bool, (Spp::Ast::UserType*, TargetGeneration*));
  private: static Bool _generateUserType(TiObject *self, Spp::Ast::UserType *astType, TargetGeneration *tg);

  public: METHOD_BINDING_CACHE(generateUserTypeMemberVars, Bool, (Spp::Ast::UserType*, TargetGeneration*));
  private: static Bool _generateUserTypeMemberVars(TiObject *self, Spp::Ast::UserType *astType, TargetGeneration *tg);

  public: METHOD_BINDING_CACHE(generateFunctionType, Bool, (Spp::Ast::FunctionType*, TargetGeneration*));
  private: static Bool _generateFunctionType(TiObject *self, Spp::Ast::FunctionType *astType, TargetGeneration *tg);

  public: METHOD_BINDING_CACHE(generateCast,
    Bool, (GenDeps const&, Spp::Ast::Type*, Spp::Ast::Type*, TiObject*, TioSharedPtr&)
  );
  private: static Bool _generateCast(
    TiObject *self, GenDeps const &deps, Spp::Ast::Type *srcType, Spp::Ast::Type *targetType,
    TiObject *tgValue, TioSharedPtr &tgCastedValue
  );

  public: METHOD_BINDING_CACHE(generateDefaultValue,
    Bool, (Spp::Ast::Type*, GenDeps const&, TioSharedPtr&)
  );
  private: static Bool _generateDefaultValue(
    TiObject *self, Spp::Ast::Type *astType, GenDeps const &deps, TioSharedPtr &result
  );

  public: METHOD_BINDING_CACHE(generateDefaultArrayValue,
    Bool, (Spp::Ast::ArrayType*, GenDeps const&, TioSharedPtr&)
  );
  private: static Bool _generateDefaultArrayValue(
    TiObject *self, Spp::Ast::ArrayType *astType, GenDeps const &deps, TioSharedPtr &result
  );

  public: METHOD_BINDING_CACHE(generateDefaultUserTypeValue,
    Bool, (Spp::Ast::UserType*, GenDeps const&, TioSharedPtr&)
  );
  private: static Bool _generateDefaultUserTypeValue(
    TiObject *self, Spp::Ast::UserType *astType, GenDeps const &deps, TioSharedPtr &result
  );

  public: METHOD_BINDING_CACHE(getTypeAllocationSize, Bool, (Spp::Ast::Type*, TargetGeneration*, Word&));
  private: static Bool _getTypeAllocationSize(
    TiObject *self, Spp::Ast::Type *astType, TargetGeneration *tg, Word &result
  );

  /// @}

}; // class

} } // namespace

#endif
