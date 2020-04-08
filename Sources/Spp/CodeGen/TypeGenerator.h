/**
 * @file Spp/CodeGen/TypeGenerator.h
 * Contains the header of class Spp::CodeGen::TypeGenerator.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
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
    TiObject *ref, Generation *g, GenDeps const &deps, TiObject *&targetTypeResult, Ast::Type **astTypeResult
  );

  public: Bool getGeneratedVoidType(
    Generation *g, GenDeps const &deps, TiObject *&tgTypeResult, Ast::VoidType **astTypeResult
  );

  private: Bool _getGeneratedType(TiObject *ref, Generation *g, GenDeps const &deps, Spp::Ast::Type *&type);

  /// @}

  /// @name Code Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(generateType, Bool, (Spp::Ast::Type*, Generation*, GenDeps const&));
  private: static Bool _generateType(TiObject *self, Spp::Ast::Type *astType, Generation *g, GenDeps const &deps);

  public: METHOD_BINDING_CACHE(generateVoidType, Bool, (Spp::Ast::VoidType*, GenDeps const&));
  private: static Bool _generateVoidType(TiObject *self, Spp::Ast::VoidType *astType, GenDeps const &deps);

  public: METHOD_BINDING_CACHE(generateIntegerType, Bool, (Spp::Ast::IntegerType*, GenDeps const&));
  private: static Bool _generateIntegerType(TiObject *self, Spp::Ast::IntegerType *astType, GenDeps const &deps);

  public: METHOD_BINDING_CACHE(generateFloatType, Bool, (Spp::Ast::FloatType*, GenDeps const&));
  private: static Bool _generateFloatType(TiObject *self, Spp::Ast::FloatType *astType, GenDeps const &deps);

  public: METHOD_BINDING_CACHE(generatePointerType, Bool, (Spp::Ast::PointerType*, Generation*, GenDeps const&));
  private: static Bool _generatePointerType(
    TiObject *self, Spp::Ast::PointerType *astType, Generation *g, GenDeps const &deps
  );

  public: METHOD_BINDING_CACHE(generateReferenceType, Bool, (Spp::Ast::ReferenceType*, Generation*, GenDeps const&));
  private: static Bool _generateReferenceType(
    TiObject *self, Spp::Ast::ReferenceType *astType, Generation *g, GenDeps const &deps
  );

  public: METHOD_BINDING_CACHE(generateArrayType, Bool, (Spp::Ast::ArrayType*, Generation*, GenDeps const&));
  private: static Bool _generateArrayType(
    TiObject *self, Spp::Ast::ArrayType *astType, Generation *g, GenDeps const &deps
  );

  public: METHOD_BINDING_CACHE(generateUserType, Bool, (Spp::Ast::UserType*, Generation*, GenDeps const&));
  private: static Bool _generateUserType(
    TiObject *self, Spp::Ast::UserType *astType, Generation *g, GenDeps const &deps
  );

  public: METHOD_BINDING_CACHE(generateUserTypeMemberVars,
    Bool, (Spp::Ast::UserType*, Generation *, GenDeps const&)
  );
  private: static Bool _generateUserTypeMemberVars(
    TiObject *self, Spp::Ast::UserType *astType, Generation *g, GenDeps const &deps
  );

  public: METHOD_BINDING_CACHE(generateUserTypeAutoConstructor,
    Bool, (Spp::Ast::UserType*, Generation*, GenDeps const&)
  );
  private: static Bool _generateUserTypeAutoConstructor(
    TiObject *self, Spp::Ast::UserType *astType, Generation *g, GenDeps const &deps
  );

  public: METHOD_BINDING_CACHE(generateUserTypeAutoDestructor,
    Bool, (Spp::Ast::UserType*, Generation*, GenDeps const&)
  );
  private: static Bool _generateUserTypeAutoDestructor(
    TiObject *self, Spp::Ast::UserType *astType, Generation *g, GenDeps const &deps
  );

  public: METHOD_BINDING_CACHE(generateFunctionType, Bool, (Spp::Ast::FunctionType*, Generation*, GenDeps const&));
  private: static Bool _generateFunctionType(
    TiObject *self, Spp::Ast::FunctionType *astType, Generation *g, GenDeps const &deps
  );

  public: METHOD_BINDING_CACHE(generateCast,
    Bool, (
      Generation*, GenDeps const&, Spp::Ast::Type*, Spp::Ast::Type*, Core::Data::Node *, TiObject*, Bool, TioSharedPtr&
    )
  );
  private: static Bool _generateCast(
    TiObject *self, Generation *g, GenDeps const &deps, Spp::Ast::Type *srcType, Spp::Ast::Type *targetType,
    Core::Data::Node *astNode, TiObject *tgValue, Bool implicit, TioSharedPtr &tgCastedValue
  );

  public: METHOD_BINDING_CACHE(generateDefaultValue,
    Bool, (Spp::Ast::Type*, Generation*, GenDeps const&, TioSharedPtr&)
  );
  private: static Bool _generateDefaultValue(
    TiObject *self, Spp::Ast::Type *astType, Generation *g, GenDeps const &deps, TioSharedPtr &result
  );

  public: METHOD_BINDING_CACHE(generateDefaultArrayValue,
    Bool, (Spp::Ast::ArrayType*, Generation*, GenDeps const&, TioSharedPtr&)
  );
  private: static Bool _generateDefaultArrayValue(
    TiObject *self, Spp::Ast::ArrayType *astType, Generation *g, GenDeps const &deps, TioSharedPtr &result
  );

  public: METHOD_BINDING_CACHE(generateDefaultUserTypeValue,
    Bool, (Spp::Ast::UserType*, Generation*, GenDeps const&, TioSharedPtr&)
  );
  private: static Bool _generateDefaultUserTypeValue(
    TiObject *self, Spp::Ast::UserType *astType, Generation *g, GenDeps const &deps, TioSharedPtr &result
  );

  public: METHOD_BINDING_CACHE(getTypeAllocationSize, Bool, (Spp::Ast::Type*, Generation*, GenDeps const&, Word&));
  private: static Bool _getTypeAllocationSize(
    TiObject *self, Spp::Ast::Type *astType, Generation *g, GenDeps const &deps, Word &result
  );

  /// @}

  /// @name Helper Functions
  /// @{

  private: static Bool isInjection(Core::Data::Ast::Definition *def);

  /// @}

}; // class

} } // namespace

#endif
