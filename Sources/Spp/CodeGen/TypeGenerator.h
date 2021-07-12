/**
 * @file Spp/CodeGen/TypeGenerator.h
 * Contains the header of class Spp::CodeGen::TypeGenerator.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
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
  private: AstProcessor *astProcessor = 0;


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

  public: void setAstProcessor(AstProcessor *ap)
  {
    this->astProcessor = ap;
  }

  public: AstProcessor* getAstProcessor() const
  {
    return this->astProcessor;
  }

  /// @}

  /// @name Main Operation Functions
  /// @{

  public: Bool getGeneratedType(
    TiObject *ref, Generation *g, Session *session, TiObject *&targetTypeResult, Ast::Type **astTypeResult
  );

  public: Bool getGeneratedVoidType(
    Generation *g, Session *session, TiObject *&tgTypeResult, Ast::VoidType **astTypeResult
  );

  private: Bool _getGeneratedType(TiObject *ref, Generation *g, Session *session, Spp::Ast::Type *&type);

  /// @}

  /// @name Code Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(generateType, Bool, (Spp::Ast::Type*, Generation*, Session*));
  private: static Bool _generateType(TiObject *self, Spp::Ast::Type *astType, Generation *g, Session *session);

  public: METHOD_BINDING_CACHE(generateVoidType, Bool, (Spp::Ast::VoidType*, Session*));
  private: static Bool _generateVoidType(TiObject *self, Spp::Ast::VoidType *astType, Session *session);

  public: METHOD_BINDING_CACHE(generateIntegerType, Bool, (Spp::Ast::IntegerType*, Session*));
  private: static Bool _generateIntegerType(TiObject *self, Spp::Ast::IntegerType *astType, Session *session);

  public: METHOD_BINDING_CACHE(generateFloatType, Bool, (Spp::Ast::FloatType*, Session*));
  private: static Bool _generateFloatType(TiObject *self, Spp::Ast::FloatType *astType, Session *session);

  public: METHOD_BINDING_CACHE(generatePointerType, Bool, (Spp::Ast::PointerType*, Generation*, Session*));
  private: static Bool _generatePointerType(
    TiObject *self, Spp::Ast::PointerType *astType, Generation *g, Session *session
  );

  public: METHOD_BINDING_CACHE(generateReferenceType, Bool, (Spp::Ast::ReferenceType*, Generation*, Session*));
  private: static Bool _generateReferenceType(
    TiObject *self, Spp::Ast::ReferenceType *astType, Generation *g, Session *session
  );

  public: METHOD_BINDING_CACHE(generateArrayType, Bool, (Spp::Ast::ArrayType*, Generation*, Session*));
  private: static Bool _generateArrayType(
    TiObject *self, Spp::Ast::ArrayType *astType, Generation *g, Session *session
  );

  public: METHOD_BINDING_CACHE(generateUserType, Bool, (Spp::Ast::UserType*, Generation*, Session*));
  private: static Bool _generateUserType(
    TiObject *self, Spp::Ast::UserType *astType, Generation *g, Session *session
  );

  public: METHOD_BINDING_CACHE(generateUserTypeMemberVars,
    Bool, (Spp::Ast::UserType*, Generation *, Session*)
  );
  private: static Bool _generateUserTypeMemberVars(
    TiObject *self, Spp::Ast::UserType *astType, Generation *g, Session *session
  );

  public: METHOD_BINDING_CACHE(generateUserTypeAutoConstructor,
    Bool, (Spp::Ast::UserType*, Generation*, Session*)
  );
  private: static Bool _generateUserTypeAutoConstructor(
    TiObject *self, Spp::Ast::UserType *astType, Generation *g, Session *session
  );

  public: METHOD_BINDING_CACHE(generateUserTypeAutoDestructor,
    Bool, (Spp::Ast::UserType*, Generation*, Session*)
  );
  private: static Bool _generateUserTypeAutoDestructor(
    TiObject *self, Spp::Ast::UserType *astType, Generation *g, Session *session
  );

  public: METHOD_BINDING_CACHE(generateFunctionType, Bool, (Spp::Ast::FunctionType*, Generation*, Session*));
  private: static Bool _generateFunctionType(
    TiObject *self, Spp::Ast::FunctionType *astType, Generation *g, Session *session
  );

  public: METHOD_BINDING_CACHE(generateCast,
    Bool, (
      Generation*, Session*, Spp::Ast::Type*, Spp::Ast::Type*, Core::Data::Node *, TiObject*, Bool, GenResult&
    )
  );
  private: static Bool _generateCast(
    TiObject *self, Generation *g, Session *session, Spp::Ast::Type *srcType, Spp::Ast::Type *targetType,
    Core::Data::Node *astNode, TiObject *tgValue, Bool implicit, GenResult &result
  );

  public: METHOD_BINDING_CACHE(generateDefaultValue,
    Bool, (Spp::Ast::Type*, Generation*, Session*, TioSharedPtr&)
  );
  private: static Bool _generateDefaultValue(
    TiObject *self, Spp::Ast::Type *astType, Generation *g, Session *session, TioSharedPtr &result
  );

  public: METHOD_BINDING_CACHE(generateDefaultArrayValue,
    Bool, (Spp::Ast::ArrayType*, Generation*, Session*, TioSharedPtr&)
  );
  private: static Bool _generateDefaultArrayValue(
    TiObject *self, Spp::Ast::ArrayType *astType, Generation *g, Session *session, TioSharedPtr &result
  );

  public: METHOD_BINDING_CACHE(generateDefaultUserTypeValue,
    Bool, (Spp::Ast::UserType*, Generation*, Session*, TioSharedPtr&)
  );
  private: static Bool _generateDefaultUserTypeValue(
    TiObject *self, Spp::Ast::UserType *astType, Generation *g, Session *session, TioSharedPtr &result
  );

  public: METHOD_BINDING_CACHE(getTypeAllocationSize, Bool, (Spp::Ast::Type*, Generation*, Session*, Word&));
  private: static Bool _getTypeAllocationSize(
    TiObject *self, Spp::Ast::Type *astType, Generation *g, Session *session, Word &result
  );

  /// @}

  /// @name Helper Functions
  /// @{

  private: static Bool isInjection(Core::Data::Ast::Definition *def);

  /// @}

}; // class

} } // namespace

#endif
