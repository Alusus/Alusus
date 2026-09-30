/**
 * @file Spp/Handlers/TypeHandlersParsingHandler.h
 * Contains the header of class Spp::Handlers::TypeHandlersParsingHandler
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_HANDLERS_TYPEHANDLERSPARSINGHANDLER_H
#define SPP_HANDLERS_TYPEHANDLERSPARSINGHANDLER_H

namespace Spp::Handlers
{

class TypeHandlersParsingHandler : public Core::Processing::Handlers::GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(TypeHandlersParsingHandler, Core::Processing::Handlers::GenericParsingHandler,
            "Spp.Handlers", "Spp", "alusus.org");


  //============================================================================
  // Types

  s_enum(Mode, FUNCTION, PTR_DEF, PTR_SET);


  //============================================================================
  // Constructor

  public: TypeHandlersParsingHandler()
  {
  }


  //============================================================================
  // Member Functions

  public: virtual void onProdEnd(Core::Processing::Parser *parser, Core::Processing::ParserState *state);

  private: SharedPtr<Spp::Ast::Block> prepareBody(SharedPtr<Core::Ast::Node> const &stmt);

  private: Bool createAssignmentHandler(
    Processing::ParserState *state, Core::Ast::AssignmentOperator *assignmentOp,
    SharedPtr<Spp::Ast::Block> const &body, SharedPtr<Core::Ast::Node> const &retType, Mode mode
  );

  private: Bool createComparisonHandler(
    Processing::ParserState *state, Core::Ast::ComparisonOperator *comparisonOp,
    SharedPtr<Spp::Ast::Block> const &body, SharedPtr<Core::Ast::Node> const &retType, Mode mode
  );

  private: Bool createInfixOpHandler(
    Processing::ParserState *state, Core::Ast::InfixOperator *infixOp,
    SharedPtr<Spp::Ast::Block> const &body, SharedPtr<Core::Ast::Node> const &retType, Mode mode
  );

  private: Bool createReadHandler(
    Processing::ParserState *state, Core::Ast::LinkOperator *linkOp,
    SharedPtr<Spp::Ast::Block> const &body, SharedPtr<Core::Ast::Node> const &retType, Mode mode
  );

  private: Bool createInitOpHandler(
    Processing::ParserState *state, Spp::Ast::InitOp *initOp,
    SharedPtr<Spp::Ast::Block> const &body
  );

  private: Bool createTerminateOpHandler(
    Processing::ParserState *state, Spp::Ast::TerminateOp *terminateOp,
    SharedPtr<Spp::Ast::Block> const &body, Mode mode
  );

  private: Bool createCastHandler(
    Processing::ParserState *state, Spp::Ast::CastOp *castOp,
    SharedPtr<Spp::Ast::Block> const &body, Mode mode
  );

  private: Bool createParensOpHandler(
    Processing::ParserState *state, Core::Ast::ParamPass *parensOp,
    SharedPtr<Spp::Ast::Block> const &body, SharedPtr<Core::Ast::Node> const &retType, Mode mode
  );

  private: SharedPtr<Core::Ast::Node> createBinaryOpFunction(
    Processing::ParserState *state, Char const *funcName, Char const *op, SharedPtr<Core::Ast::Node> const &thisType,
    Char const *inputName, SharedPtr<Core::Ast::Node> const &inputType, SharedPtr<Core::Ast::Node> const &retType, SharedPtr<Core::Ast::Node> const &body,
    SharedPtr<Core::Ast::SourceLocation> const &sourceLocation, Mode mode
  );

  private: SharedPtr<Core::Ast::Node> createFunction(
    Processing::ParserState *state, Char const *funcName, Char const *op, Bool member,
    SharedPtr<Core::Ast::Map> const argTypes, SharedPtr<Core::Ast::Node> const &retType, SharedPtr<Core::Ast::Node> const &body,
    SharedPtr<Core::Ast::SourceLocation> const &sourceLocation, Mode mode
  );

  private: Bool prepareInputArg(
    Processing::ParserState *state, SharedPtr<Core::Ast::Node> input, Char const *&inputName, SharedPtr<Core::Ast::Node> &inputType,
    Char const *defaultname = S("value")
  );

  private: Bool getThisAndPropIdentifiers(
    Processing::ParserState *state, Core::Ast::Node *astNode, Bool allowThisType,
    Core::Ast::Identifier *&thisIdentifier, SharedPtr<Core::Ast::ParamPass> &thisType,
    Core::Ast::Identifier *&propIdentifier
  );

  private: Bool getThisIdentifierAndType(
    Processing::ParserState *state, Core::Ast::Node *astNode, Bool allowThisType,
    Core::Ast::Identifier *&thisIdentifier, SharedPtr<Core::Ast::ParamPass> &thisType
  );

  private: SharedPtr<Core::Ast::ParamPass> prepareThisType(
    SharedPtr<Core::Ast::SourceLocation> const &sourceLocation
  );

  private: SharedPtr<Core::Ast::ParamPass> prepareComparisonRetType(
    SharedPtr<Core::Ast::SourceLocation> const &sourceLocation
  );

  private: SharedPtr<Core::Ast::Definition> createDefinition(
    Char const *name, Char const *op, SharedPtr<Core::Ast::Node> target,
    SharedPtr<Core::Ast::SourceLocation> const &sourceLocation
  );

  public: virtual Bool onIncomingModifier(
    Core::Processing::Parser *parser, Core::Processing::ParserState *state,
    SharedPtr<Core::Ast::Node> const &modifierData, Bool prodProcessingComplete
  );

  private: Bool processExpnameModifier(
    Core::Processing::ParserState *state, SharedPtr<Core::Ast::Node> const &modifierData
  );

}; // class

} // namespace

#endif
