/**
 * @file Spp/Handlers/TypeHandlersParsingHandler.h
 * Contains the header of class Spp::Handlers::TypeHandlersParsingHandler
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
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

  private: SharedPtr<Spp::Ast::Block> prepareBody(TioSharedPtr const &stmt);

  private: void createAssignmentHandler(
    Processing::ParserState *state, Core::Data::Ast::AssignmentOperator *assignmentOp,
    SharedPtr<Spp::Ast::Block> const &body, TioSharedPtr const &retType, Mode mode
  );

  private: void createComparisonHandler(
    Processing::ParserState *state, Core::Data::Ast::ComparisonOperator *comparisonOp,
    SharedPtr<Spp::Ast::Block> const &body, TioSharedPtr const &retType, Mode mode
  );

  private: void createInfixOpHandler(
    Processing::ParserState *state, Core::Data::Ast::InfixOperator *infixOp,
    SharedPtr<Spp::Ast::Block> const &body, TioSharedPtr const &retType, Mode mode
  );

  private: void createReadHandler(
    Processing::ParserState *state, Core::Data::Ast::LinkOperator *linkOp,
    SharedPtr<Spp::Ast::Block> const &body, TioSharedPtr const &retType, Mode mode
  );

  private: void createInitOpHandler(
    Processing::ParserState *state, Spp::Ast::InitOp *initOp,
    SharedPtr<Spp::Ast::Block> const &body
  );

  private: void createTerminateOpHandler(
    Processing::ParserState *state, Spp::Ast::TerminateOp *terminateOp,
    SharedPtr<Spp::Ast::Block> const &body, Mode mode
  );

  private: void createCastHandler(
    Processing::ParserState *state, Spp::Ast::CastOp *castOp,
    SharedPtr<Spp::Ast::Block> const &body, Mode mode
  );

  private: void createParensOpHandler(
    Processing::ParserState *state, Core::Data::Ast::ParamPass *parensOp,
    SharedPtr<Spp::Ast::Block> const &body, TioSharedPtr const &retType, Mode mode
  );

  private: TioSharedPtr createBinaryOpFunction(
    Processing::ParserState *state, Char const *funcName, Char const *op, TioSharedPtr const &thisType,
    Char const *inputName, TioSharedPtr const &inputType, TioSharedPtr const &retType, TioSharedPtr const &body,
    SharedPtr<Core::Data::SourceLocation> const &sourceLocation, Mode mode
  );

  private: TioSharedPtr createFunction(
    Processing::ParserState *state, Char const *funcName, Char const *op,
    SharedPtr<Core::Data::Ast::Map> const argTypes, TioSharedPtr const &retType, TioSharedPtr const &body,
    SharedPtr<Core::Data::SourceLocation> const &sourceLocation, Mode mode
  );

  private: Bool prepareInputArg(
    Processing::ParserState *state, TioSharedPtr input, Char const *&inputName, TioSharedPtr &inputType,
    Char const *defaultname = S("value")
  );

  private: Bool getThisAndPropIdentifiers(
    Processing::ParserState *state, TiObject *astNode,
    Core::Data::Ast::Identifier *&thisIdentifier, SharedPtr<Core::Data::Ast::ParamPass> &thisType,
    Core::Data::Ast::Identifier *&propIdentifier
  );

  private: Bool getThisIdentifierAndType(
    Processing::ParserState *state, TiObject *astNode,
    Core::Data::Ast::Identifier *&thisIdentifier, SharedPtr<Core::Data::Ast::ParamPass> &thisType
  );

  private: SharedPtr<Core::Data::Ast::ParamPass> prepareThisType(
    SharedPtr<Core::Data::SourceLocation> const &sourceLocation
  );

  private: SharedPtr<Core::Data::Ast::ParamPass> prepareComparisonRetType(
    SharedPtr<Core::Data::SourceLocation> const &sourceLocation
  );

  private: SharedPtr<Core::Data::Ast::Definition> createDefinition(
    Char const *name, Char const *op, TioSharedPtr target,
    SharedPtr<Core::Data::SourceLocation> const &sourceLocation
  );

  public: virtual Bool onIncomingModifier(
    Core::Processing::Parser *parser, Core::Processing::ParserState *state,
    TioSharedPtr const &modifierData, Bool prodProcessingComplete
  );

  private: Bool processExpnameModifier(
    Core::Processing::ParserState *state, TioSharedPtr const &modifierData
  );

  private: Bool processUnknownModifier(
    Core::Processing::ParserState *state, TioSharedPtr const &modifierData
  );

  private: Str getImplementationPostfix();

}; // class

} // namespace

#endif
