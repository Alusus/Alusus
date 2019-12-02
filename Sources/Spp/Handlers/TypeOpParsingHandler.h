/**
 * @file Spp/Handlers/TypeOpParsingHandler.h
 * Contains the header of class Spp::Handlers::TypeOpParsingHandler
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_HANDLERS_TYPEOPPARSINGHANDLER_H
#define SPP_HANDLERS_TYPEOPPARSINGHANDLER_H

namespace Spp::Handlers
{

class TypeOpParsingHandler : public Core::Processing::Handlers::GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(TypeOpParsingHandler, Core::Processing::Handlers::GenericParsingHandler,
            "Spp.Handlers", "Spp", "alusus.org");


  //============================================================================
  // Constructor

  public: TypeOpParsingHandler()
  {
  }


  //============================================================================
  // Member Functions

  public: virtual void onProdEnd(Core::Processing::Parser *parser, Core::Processing::ParserState *state);

  private: SharedPtr<Core::Data::Ast::Scope> prepareBody(TioSharedPtr const &stmt);

  private: void createAssignmentHandler(
    Processing::ParserState *state, Core::Data::Ast::AssignmentOperator *assignmentOp,
    SharedPtr<Core::Data::Ast::Scope> const &body
  );

  private: void createComparisonHandler(
    Processing::ParserState *state, Core::Data::Ast::ComparisonOperator *comparisonOp,
    SharedPtr<Core::Data::Ast::Scope> const &body
  );

  private: void createInfixOpHandler(
    Processing::ParserState *state, Core::Data::Ast::InfixOperator *infixOp,
    SharedPtr<Core::Data::Ast::Scope> const &body, TioSharedPtr const &retType
  );

  private: void createInitOpHandler(
    Processing::ParserState *state, Spp::Ast::InitOp *initOp,
    SharedPtr<Core::Data::Ast::Scope> const &body
  );

  private: void createTerminateOpHandler(
    Processing::ParserState *state, Spp::Ast::TerminateOp *terminateOp,
    SharedPtr<Core::Data::Ast::Scope> const &body
  );

  private: void createCastHandler(
    Processing::ParserState *state, Spp::Ast::CastOp *castOp,
    SharedPtr<Core::Data::Ast::Scope> const &body
  );

  private: void createParensOpHandler(
    Processing::ParserState *state, Core::Data::Ast::ParamPass *parensOp,
    SharedPtr<Core::Data::Ast::Scope> const &body, TioSharedPtr const &retType
  );

  private: SharedPtr<Core::Data::Ast::Definition> createBinaryOpFunction(
    Char const *funcName, TioSharedPtr const &thisType, Char const *inputName, TioSharedPtr const &inputType,
    TioSharedPtr const &retType, TioSharedPtr const &body, SharedPtr<Core::Data::SourceLocation> const &sourceLocation
  );

  private: SharedPtr<Core::Data::Ast::Definition> createFunction(
    Char const *funcName, SharedPtr<Core::Data::Ast::Map> const argTypes,
    TioSharedPtr const &retType, TioSharedPtr const &body, SharedPtr<Core::Data::SourceLocation> const &sourceLocation
  );

  private: Bool prepareInputArg(
    Processing::ParserState *state, TioSharedPtr input, Char const *&inputName, TioSharedPtr &inputType
  );

  private: SharedPtr<Core::Data::Ast::ParamPass> prepareThisType(
    SharedPtr<Core::Data::SourceLocation> const &sourceLocation
  );

  private: SharedPtr<Core::Data::Ast::ParamPass> prepareAssignmentRetType(
    SharedPtr<Core::Data::SourceLocation> const &sourceLocation
  );

  private: SharedPtr<Core::Data::Ast::ParamPass> prepareComparisonRetType(
    SharedPtr<Core::Data::SourceLocation> const &sourceLocation
  );

  private: SharedPtr<Core::Data::Ast::Definition> createDefinition(
    Char const *funcName, SharedPtr<Spp::Ast::Function> func,
    SharedPtr<Core::Data::SourceLocation> const &sourceLocation
  );

}; // class

} // namespace

#endif
