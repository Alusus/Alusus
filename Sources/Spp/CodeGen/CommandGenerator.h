/**
 * @file Spp/CodeGen/CommandGenerator.h
 * Contains the header of class Spp::CodeGen::CommandGenerator.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_COMMANDGENERATOR_H
#define SPP_CODEGEN_COMMANDGENERATOR_H

namespace Spp { namespace CodeGen
{

class CommandGenerator : public TiObject, public DynamicBinding, public DynamicInterfacing
{
  //============================================================================
  // Type Info

  TYPE_INFO(CommandGenerator, TiObject, "Spp.CodeGen", "Spp", "alusus.org", (
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


  //============================================================================
  // Constructors & Destructor

  public: CommandGenerator(Ast::Helper *h) : astHelper(h)
  {
    this->initBindingCaches();
    this->initBindings();
  }

  public: CommandGenerator(CommandGenerator *parent)
  {
    this->initBindingCaches();
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);
    this->astHelper = parent->getAstHelper();
  }

  public: virtual ~CommandGenerator()
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

  /// @}

  /// @name Code Generation Functions
  /// @{

  public: METHOD_BINDING_CACHE(generateReturnStatement,
    Bool, (
      Spp::Ast::ReturnStatement*, Generation*, Session*
    )
  );
  private: static Bool _generateReturnStatement(
    TiObject *self, Spp::Ast::ReturnStatement *astNode, Generation *g, Session *session
  );

  public: METHOD_BINDING_CACHE(generateIfStatement,
    Bool, (
      Spp::Ast::IfStatement*, Generation*, Session*, TerminalStatement&
    )
  );
  private: static Bool _generateIfStatement(
    TiObject *self, Spp::Ast::IfStatement *astNode, Generation *g, Session *session,
    TerminalStatement &terminal
  );

  public: METHOD_BINDING_CACHE(generateWhileStatement,
    Bool, (
      Spp::Ast::WhileStatement*, Generation*, Session*
    )
  );
  private: static Bool _generateWhileStatement(
    TiObject *self, Spp::Ast::WhileStatement *astNode, Generation *g, Session *session
  );

  public: METHOD_BINDING_CACHE(generateForStatement,
    Bool, (
      Spp::Ast::ForStatement*, Generation*, Session*
    )
  );
  private: static Bool _generateForStatement(
    TiObject *self, Spp::Ast::ForStatement *astNode, Generation *g, Session *session
  );

  public: METHOD_BINDING_CACHE(generateContinueStatement,
    Bool, (
      Spp::Ast::ContinueStatement* /* astNode */, Generation* /* g */, Session* /* session */
    )
  );
  private: static Bool _generateContinueStatement(
    TiObject *self, Spp::Ast::ContinueStatement *astNode, Generation *g, Session *session
  );

  public: METHOD_BINDING_CACHE(generateBreakStatement,
    Bool, (
      Spp::Ast::BreakStatement* /* astNode */, Generation* /* g */, Session* /* session */
    )
  );
  private: static Bool _generateBreakStatement(
    TiObject *self, Spp::Ast::BreakStatement *astNode, Generation *g, Session *session
  );

  /// @}

  /// @name Helper Functions
  /// @{

  private: Bool generateCondition(TiObject *astNode, Generation *g, Session *session, GenResult &result);

  private: Bool castCondition(
    Generation *g, Session *session, TiObject *astNode, Spp::Ast::Type *astType,
    TiObject *tgValue, TioSharedPtr &result
  );

  /// @}

}; // class

} } // namespace

#endif
