/**
 * @file Scg/CodeGeneration/CodeGenerator.cpp
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>

// Boost header files
#include <boost/lexical_cast.hpp>

// Core header files.
#include <Processing/BuildMsgStore.h>

// Scg header files
#include <CodeGeneration/CodeGenerator.h>
#include <CodeGeneration/FunctionAstBlock.h>
#include <CodeGeneration/FunctionLinkExpression.h>
#include <Containers/Block.h>
#include <Containers/List.h>
#include <Containers/Module.h>
#include <Instructions/CallFunction.h>
#include <Instructions/DeclareExtFunction.h>
#include <Instructions/DefineFunction.h>
#include <Instructions/DefineStruct.h>
#include <Instructions/DefineVariable.h>
#include <Instructions/ForStatement.h>
#include <Instructions/IfStatement.h>
#include <Instructions/Return.h>
#include <Instructions/WhileStatement.h>
#include <Operators/AssignmentOperator.h>
#include <Operators/BinaryOperator.h>
#include <Operators/Content.h>
#include <Operators/IdentifierReference.h>
#include <Values/DoubleConst.h>
#include <Values/FloatConst.h>
#include <Values/IntegerConst.h>
#include <Values/CharConst.h>
#include <Values/StringConst.h>

// LLVM header files

// TODO: Optimise this by returning structures by reference rather than by value.

namespace Scg
{
using namespace Core::Basic;
using namespace Core::Data;

//----------------------------------------------------------------------------

void CodeGenerator::initializeIds()
{
#define GENERATE_ID(var, name) \
  (var) = ID_GENERATOR->getId(name); /* \
    LOG_INFO(std::string(name) + ": " + boost::lexical_cast<std::string>(var));*/

  //statementListId = ID_GENERATOR->getId("StatementList");
  GENERATE_ID(statementListId, "Main.StatementList");
  GENERATE_ID(expressionId, "Expression.Exp");
  GENERATE_ID(lowLinkExpId, "Expression.LowLinkExp");
  GENERATE_ID(lowerLinkExpId, "Expression.LowerLinkExp");
  GENERATE_ID(lowestLinkExpId, "Expression.LowestLinkExp");
  GENERATE_ID(listExpId, "Expression.ListExp");
  GENERATE_ID(functionalExpId, "Expression.FunctionalExp");
  GENERATE_ID(comparisonExpId, "Expression.ComparisonExp");
  GENERATE_ID(assignmentExpId, "Expression.AssignmentExp");
  GENERATE_ID(addExpId, "Expression.AddExp");
  GENERATE_ID(mulExpId, "Expression.MulExp");
  GENERATE_ID(paramPassId, "Expression.ParamPassExp");
  GENERATE_ID(linkExpId, "Expression.LinkExp");
  GENERATE_ID(postfixTildeExpId, "Expression.PostfixTildeExp");
  GENERATE_ID(unaryExpId, "Expression.UnaryExp");
  GENERATE_ID(subjectId, "Subject.Subject1");
  GENERATE_ID(subSubjectId, "SubSubject.Subject1");
  GENERATE_ID(parameterId, "Subject.Parameter");
  GENERATE_ID(literalId, "Subject.Literal");
  GENERATE_ID(identifierTokenId, "LexerDefs.Identifier");

  GENERATE_ID(defId, "SubMain.Def");
  GENERATE_ID(returnId, "Main.Return");
  GENERATE_ID(ifId, "Main.If");
  GENERATE_ID(forId, "Main.For");
  GENERATE_ID(whileId, "Main.While");
  GENERATE_ID(linkId, "Main.Link");
  GENERATE_ID(functionId, "Subject.Function");
  GENERATE_ID(structureId, "Subject.Structure");
  GENERATE_ID(castTildeId, "Expression.Cast_Tilde");
  GENERATE_ID(pointerTildeId, "Expression.Pointer_Tilde");
  GENERATE_ID(contentTildeId, "Expression.Content_Tilde");
  GENERATE_ID(sizeTildeId, "Expression.Size_Tilde");
}

//----------------------------------------------------------------------------

AstNodeSharedArray CodeGenerator::generateStatement(SharedPtr<IdentifiableObject> const &item)
{
  auto metadata = item.ii_cast_get<ParsingMetadataHolder>();

  if (metadata == 0) {
    throw EXCEPTION(SyntaxErrorException, "Invalid object type in def command.");
  }

  auto id = metadata->getProdId();

  if (id == defId)
    return generateDefine(item);
  else if (id == returnId)
    return{ generateReturn(item) };
  else if (id == ifId)
    return{ generateIfStatement(item) };
  else if (id == forId)
    return{ generateForStatement(item) };
  else if (id == whileId)
    return{ generateWhileStatement(item) };
  else if (id == linkId) {
    FunctionLinkExpression funcLink(this, item);
    auto declExtFunc = funcLink.toDeclareExtFunction();
    declExtFunc->setSourceLocation(metadata->getSourceLocation());
    return{ declExtFunc };
  } else
    return{ generateExpression(item) };
}

//----------------------------------------------------------------------------

SharedPtr<Block> CodeGenerator::generateSet(const SharedPtr<PrtList> &list)
{
  AstNodeSharedArray blockExprs;

  for (auto i = 0; i < list->getCount(); i++) {
    auto element = list->getShared(i);

    for (auto stat : generateStatement(element)) {
      blockExprs.push_back(stat);
    }
  }

  return std::make_shared<Block>(blockExprs);
}

//----------------------------------------------------------------------------

SharedPtr<Block> CodeGenerator::generateInnerSet(SharedPtr<IdentifiableObject> const &item)
{
  static SharedPtr<Reference> setReference = REF_PARSER->parseQualifier(
        STR("self~where(prodId=Expression.Exp)."
            "0~where(prodId=Subject.Subject1)."
            "0~where(prodId=Main.StatementList)"),
        ReferenceUsageCriteria::MULTI_DATA);
  static ReferenceSeeker seeker;
  IdentifiableObject *set = seeker.tryGet(setReference.get(), item.get());

  if (set == 0) {
    AstNodeSharedArray blockExprs;

    for (auto stat : generateStatement(item)) {
      blockExprs.push_back(stat);
    }

    // Creates the block representing the inner set and sets its line and
    // column number.
    auto block = std::make_shared<Block>(blockExprs);
    auto metadata = item.ii_cast_get<ParsingMetadataHolder>();

    if (metadata != nullptr) {
      block->setSourceLocation(metadata->getSourceLocation());
    }

    return block;
  } else {
    return generateSet(s_cast<PrtList>(getSharedPtr(set)));
  }
}

//----------------------------------------------------------------------------

AstNodeSharedArray CodeGenerator::generateDefine(SharedPtr<IdentifiableObject> const &item)
{
  // Def -- [LIST]:
  //  Expression.Exp -- [LIST]:
  //   Expression.LowerLinkExp -- [LIST]:
  //    Subject.Subject1 -- [ROUTE]: 0
  //     Subject.Parameter -- [TOKEN]: IDENTIFIER_TOKEN ("main")
  //    [TOKEN]: CONSTANT_59 (":")
  //    Subject.Subject1 -- [ROUTE]: 0
  //     Function -- [LIST]:
  //      Subject.Subject1 -- [ROUTE]: 0
  //       StatementList -- [LIST]:

  static ReferenceSeeker seeker;

  ParsingMetadataHolder *itemMetadata = item->getInterface<ParsingMetadataHolder>();

  if (itemMetadata == 0) {
    throw EXCEPTION(SyntaxErrorException, "Invalid 'def' command data.");
  }

  // Get the name of the definition.
  static SharedPtr<Reference> nameReference = REF_PARSER->parseQualifier(
        STR("1~where(prodId=Expression.Exp)."
            "0." //~where(prodId=Expression.LowerLinkExp | prodId=Expression.AssignmentExp)."
            "0~where(prodId=Subject.Subject1)."
            "0~where(prodId=Subject.Parameter)"),
        ReferenceUsageCriteria::MULTI_DATA);
  auto nameToken = io_cast<PrtToken>(seeker.tryGet(nameReference.get(), item.get()));

  if (nameToken == nullptr || nameToken->getId() != identifierTokenId) {
    this->buildMsgStore->add(std::make_shared<Processing::CustomBuildMsg>(
                               STR("A 'def' command needs a definition name."),
                               STR("SCG1002"), 1, itemMetadata->getSourceLocation()));
    return{};
  }

  auto name = this->translateAliasedName(nameToken->getText().c_str());

  // Get the defined (after the colon).

  static SharedPtr<Reference> defOrAssignReference = REF_PARSER->parseQualifier(
        STR("1~where(prodId=Expression.Exp)."
            "0"), ReferenceUsageCriteria::MULTI_DATA);
  auto defOrAssign = seeker.tryGet(defOrAssignReference.get(), item.get());
  auto defOrAssignMetadata = defOrAssign != nullptr ?
                             defOrAssign->getInterface<ParsingMetadataHolder>() :
                             nullptr;

  if (defOrAssignMetadata == nullptr) {
    this->buildMsgStore->add(std::make_shared<Processing::CustomBuildMsg>(
                               STR("Invalid 'def' command."),
                               STR("SCG1004"), 1, itemMetadata->getSourceLocation(),
                               nameToken->getText().c_str()));
    return{};
  }

  auto isAssignment = false;

  if (defOrAssignMetadata->getProdId() == assignmentExpId) {
    isAssignment = true;
  } else if (defOrAssignMetadata->getProdId() == lowerLinkExpId) {
  } else {
    this->buildMsgStore->add(std::make_shared<Processing::CustomBuildMsg>(
                               STR("Invalid 'def' command."),
                               STR("SCG1004"), 1, itemMetadata->getSourceLocation(),
                               nameToken->getText().c_str()));
    return{};
  }

  static SharedPtr<Reference> defReference = REF_PARSER->parseQualifier(
        STR("1~where(prodId=Expression.Exp)."
            "0." //~where(prodId=Expression.LowerLinkExp)."
            "2"), ReferenceUsageCriteria::MULTI_DATA);
  auto def = seeker.tryGet(defReference.get(), item.get());
  ParsingMetadataHolder *defMetadata = def == 0 ? 0 : def->getInterface<ParsingMetadataHolder>();

  if (defMetadata == nullptr) {
    // TODO: We need to choose terms for the parts of a define command, e.g.
    // definition name, definition, etc.
    this->buildMsgStore->add(std::make_shared<Processing::CustomBuildMsg>(
                               STR("A 'def' command needs a definition body"),
                               STR("SCG1003"), 1, itemMetadata->getSourceLocation(),
                               nameToken->getText().c_str()));
    return{};
  }

  if (defMetadata->getProdId() == functionalExpId)
    // Defining a variable
    return generateDefineVariable(name, getSharedPtr(def), isAssignment);
  else if (defMetadata->getProdId() == subjectId) {
    auto routeData = static_cast<PrtRoute*>(def)->getData();
    auto routeMetadata = routeData->getInterface<ParsingMetadataHolder>();

    if (routeMetadata->getProdId() == parameterId ||
        routeMetadata->getProdId() == literalId) {
      // Defining a variable
      return generateDefineVariable(name, routeData, isAssignment);
    } else if (routeMetadata->getProdId() == functionId)
      // Defining a function
      return AstNodeSharedArray({ generateDefineFunction(name, routeData) });
    else if (routeMetadata->getProdId() == structureId)
      // Defining a structure
      return AstNodeSharedArray({ generateDefineStructure(name, routeData) });
    else {
      this->buildMsgStore->add(std::make_shared<Processing::CustomBuildMsg>(
                                 STR("Invalid def command."),
                                 STR("SCG1004"), 1, itemMetadata->getSourceLocation(),
                                 nameToken->getText().c_str()));
      return AstNodeSharedArray();
    }
  } else {
    this->buildMsgStore->add(std::make_shared<Processing::CustomBuildMsg>(
                               STR("Invalid def command."),
                               STR("SCG1004"), 1, itemMetadata->getSourceLocation(),
                               nameToken->getText().c_str()));
    return AstNodeSharedArray();
  }
}

//----------------------------------------------------------------------------

AstNodeSharedArray CodeGenerator::generateDefineVariable(Char const *name,
    SharedPtr<IdentifiableObject> const &expr, bool isAssignment)
{
  if (isAssignment) {
    auto value = generateExpression(expr);
    auto defVar = std::make_shared<DefineVariable>(value, name);
    auto assign = std::make_shared<AssignmentOperator>(
      std::make_shared<Content>(std::make_shared<IdentifierReference>(name)), value);

    auto exprMetadata = expr->getInterface<ParsingMetadataHolder>();

    if (exprMetadata != nullptr) {
      defVar->setSourceLocation(exprMetadata->getSourceLocation());
      assign->setSourceLocation(exprMetadata->getSourceLocation());
    }

    return {defVar, assign};
  } else {
    // Parses the variable type.
    auto type = parseVariableType(expr);
    // Creates the DefineVariable instruction and sets its line and column numbers.
    auto defVar = std::make_shared<DefineVariable>(type, name);
    auto exprMetadata = expr->getInterface<ParsingMetadataHolder>();

    if (exprMetadata != 0) {
      defVar->setSourceLocation(exprMetadata->getSourceLocation());
    }

    return {defVar};
  }
}

//----------------------------------------------------------------------------

SharedPtr<DefineFunction> CodeGenerator::generateDefineFunction(Char const *name,
    const SharedPtr<IdentifiableObject> &item)
{
  return FunctionAstBlock(this, item.s_cast<PrtList>()).toDefineFunction(name);
}

//----------------------------------------------------------------------------

SharedPtr<DefineStruct> CodeGenerator::generateDefineStructure(Char const *name,
    SharedPtr<IdentifiableObject> const &item)
{
  static ReferenceSeeker seeker;
  auto itemMetadata = item->getInterface<ParsingMetadataHolder>();

  if (itemMetadata == 0) {
    throw EXCEPTION(SyntaxErrorException, "Invalid struct definition data.");
  }

  // Generate function body.
  static SharedPtr<Reference> structBodyReference = REF_PARSER->parseQualifier(
        STR("1~where(prodId=Main.StatementList)"), ReferenceUsageCriteria::MULTI_DATA);
  auto bodyStmtList = getSharedPtr(seeker.tryGet(structBodyReference.get(), item.get())).io_cast<PrtList>();

  if (bodyStmtList == 0) {
    this->buildMsgStore->add(std::make_shared<Processing::CustomBuildMsg>(
                               STR("A struct definition expects a body."),
                               STR("SCG1005"), 1, itemMetadata->getSourceLocation()));
    return 0;
  }

  auto structBody = generateSet(bodyStmtList);

  // Extract members names and types from structBody and ignore the body.
  VariableDefinitionArray fields;
  for (Int i = 0; i < structBody->getCount(); ++i) {
    auto child = io_cast<AstNode>(structBody->get(i));
    if (child == 0) {
      throw EXCEPTION(GenericException, STR("Invalid node type found."));
    }

    auto field = io_cast<DefineVariable>(child);

    if (field == nullptr) {
      this->buildMsgStore->add(std::make_shared<Processing::CustomBuildMsg>(
                                 STR("A struct body can only contain variable definitions."),
                                 STR("SCG1006"), 1, child->getSourceLocation()));
      return 0;
    }

    auto typeSpec = field->getVarTypeSpec()->clone();
    auto name = field->getVarName();
    fields.push_back(VariableDefinition(typeSpec, name));
  }

  // Creates the DefineStruct instruction and sets its line and column numbers.
  auto defStruct = std::make_shared<DefineStruct>(name, fields);
  defStruct->setSourceLocation(itemMetadata->getSourceLocation());

  return defStruct;
}

//----------------------------------------------------------------------------

SharedPtr<Return> CodeGenerator::generateReturn(SharedPtr<IdentifiableObject> const &item)
{
  static ReferenceSeeker seeker;
  auto itemMetadata = item.ii_cast_get<ParsingMetadataHolder>();

  if (itemMetadata == 0) {
    throw EXCEPTION(SyntaxErrorException, "Invalid return argument.");
  }

  static SharedPtr<Reference> expReference = REF_PARSER->parseQualifier(
        STR("{find prodId=Expression.Exp, 0}"), ReferenceUsageCriteria::MULTI_DATA);
  auto exp = getSharedPtr(seeker.tryGet(expReference.get(), item.get()));

  if (exp == nullptr) {
    this->buildMsgStore->add(std::make_shared<Processing::CustomBuildMsg>(
                               STR("Invalid return argument."),
                               STR("SCG1007"), 1, itemMetadata->getSourceLocation()));
    return 0;
  }

  // Creates the Return instruction and sets the line and column numbers.
  auto ret = std::make_shared<Return>(generateExpression(exp));
  ret->setSourceLocation(itemMetadata->getSourceLocation());
  return ret;
}

//----------------------------------------------------------------------------

SharedPtr<AstNode> CodeGenerator::generateExpression(SharedPtr<IdentifiableObject> const &item)
{
  auto itemMetadata = item->getInterface<ParsingMetadataHolder>();

  if (itemMetadata == nullptr)
    throw EXCEPTION(SyntaxErrorException, "Invalid expression data structure.");

  auto id = itemMetadata->getProdId();

  SharedPtr<AstNode> expr;

  if (id == expressionId)
    expr = generateExpression(item.s_cast_get<PrtList>()->getShared(0));
  else if (id == subjectId)
    expr = generateExpression(item.s_cast_get<PrtRoute>()->getData());
  else if (id == parameterId)
    // Subject.Subject1>Subject.Parameter are variable references.
    expr = generateVariableRef(item);
  else if (id == literalId)
    // Literals are constants
    expr = generateConst(item.s_cast<PrtToken>());
  else if (id == functionalExpId) {
    /*auto list = item.s_cast<PrtList>();
    auto id2 = list->get(1)->getInterface<ParsingMetadataHolder>()->getProdId();
    if (id2 == paramPassId)
    expr = GenerateFunctionCall(item.s_cast<PrtList>());
    else if (id2 == postfixTildeExpId)
    expr = GenerateVariableRef(item);
    else if (id2 == linkExpId)
    expr = GenerateMemberAccess(item);
    else
    throw EXCEPTION(ArgumentOutOfRangeException, "The given parsing data doesn't evaluate to an expression.");*/
    expr = FunctionalExpression(this, item.s_cast<PrtList>()).toExpression();
  } else if (id == listExpId)
    expr = generateList(item.s_cast<PrtList>());
  else if (id == comparisonExpId || id == assignmentExpId || id == addExpId || id == mulExpId)
    expr = generateBinaryOperator(item.s_cast<PrtList>());
  else if (id == statementListId)
    expr = generateSet(item.s_cast<PrtList>());
  else if (id == unaryExpId)
    expr = generateUnaryOperator(item.s_cast<PrtList>());
  else
    throw EXCEPTION(ArgumentOutOfRangeException, "The given parsing data doesn't evaluate to an expression.");

  if (expr == 0) {
    // In case of errors we'll end up with a null expr. To avoid exceptions we'll
    // return a dummy expression.
    expr = std::make_shared<StringConst>(STR("__DUMMY__"));
    expr->setSourceLocation(itemMetadata->getSourceLocation());
  }

  return expr;
}

//----------------------------------------------------------------------------

SharedPtr<AstNode> CodeGenerator::generateVariableRef(SharedPtr<IdentifiableObject> const &parsedItem)
{
  auto parsedItemMetadata = parsedItem->getInterface<ParsingMetadataHolder>();

  if (parsedItemMetadata == nullptr) {
    throw EXCEPTION(SyntaxErrorException, "Invalid variable ref data structure.");
  }

  auto id = parsedItemMetadata->getProdId();

  //    if (id == functionalExpId)
  //    {
  //      auto PrtList = parsedItem.s_cast<PrtList>();
  //      if (PrtList->getElementCount() != 2)
  //        throw EXCEPTION(SyntaxErrorException, "Expression doesn't evaluate to a variable reference.");
  //      auto varName = PrtList
  //          ->getElement(0).s_cast<PrtRoute>()
  //          ->getData().s_cast<PrtToken>()
  //          ->getText();
  //      auto postfix = PrtList->get(1);
  //      auto postfixId = postfix->getInterface<ParsingMetadataHolder>()->getProdId();
  //      if (postfixId != postfixTildeExpId)
  //        throw EXCEPTION(SyntaxErrorException, "Expression doesn't evaluate to a variable reference.");
  //      auto postfixType = postfix.s_cast<PrtList>()
  //          ->get(0)->getInterface<ParsingMetadataHolder>()->getProdId();
  //      if (postfixType == pointerTildeId)
  //        return new IdentifierReference(varName);
  //      else if (postfixType == contentTildeId)
  //        // TODO: Implement this.
  //        return nullptr; //new VariableDeref(varName);
  //      else
  //        throw EXCEPTION(SyntaxErrorException, "Expression doesn't evaluate to a variable reference.");
  //    }
  //    else
  if (id == subjectId) {
    auto varName = parseToken(parsedItem);
    return std::make_shared<Content>(std::make_shared<IdentifierReference>(varName));
  }
  // This shouldn't be needed anymore.
  else if (id == parameterId) {
    auto varName = this->translateAliasedName(parsedItem.s_cast_get<PrtToken>()->getText().c_str());
    return std::make_shared<Content>(std::make_shared<IdentifierReference>(varName));
  } else {
    this->buildMsgStore->add(std::make_shared<Processing::CustomBuildMsg>(
                               STR("Expression doesn't evaluate to a variable reference."),
                               STR("SCG1008"), 1, parsedItemMetadata->getSourceLocation()));
    return 0;
  }
}

SharedPtr<AstNode> CodeGenerator::generateConst(const SharedPtr<PrtToken> &literal)
{
  auto literalId = literal->getId();
  auto literalText = literal->getText();
  SharedPtr<AstNode> constant;

  if (literalId == ID_GENERATOR->getId("LexerDefs.IntLiteral"))
    // Integral constant
    constant = std::make_shared<IntegerConst>(boost::lexical_cast<int>(literalText));
  else if (literalId == ID_GENERATOR->getId("LexerDefs.FloatLiteral")) {
    if (literalText[literalText.size() - 1] == 'f' ||
        literalText[literalText.size() - 1] == 'F')
      // Floating point constant
      constant = std::make_shared<FloatConst>(boost::lexical_cast<float>(
                                              literalText.substr(0, literalText.size() - 1)));
    else
      // Double floating point constant
      constant = std::make_shared<DoubleConst>(boost::lexical_cast<float>(literalText));
  } else if (literalId == ID_GENERATOR->getId("LexerDefs.StringLiteral"))
    // String constant
    constant = std::make_shared<StringConst>(literalText);
  else if (literalId == ID_GENERATOR->getId("LexerDefs.CharLiteral")) {
    // Char constant
    char tempLiteral[2];
    tempLiteral[0] = literalText[1];
    tempLiteral[1] = '\0';
    constant = std::make_shared<CharConst>(boost::lexical_cast<char>(tempLiteral));
  } else
    throw EXCEPTION(NotImplementedException, "Not implemented yet.");

  // Sets the line and the column of the source code that generated this
  // expression.
  constant->setSourceLocation(literal->getSourceLocation());

  return constant;
}

//----------------------------------------------------------------------------

SharedPtr<List> CodeGenerator::generateList(const SharedPtr<PrtList> &listExpr)
{
  // Generate an array containing the expressions representing the elements of
  // the list.
  AstNodeSharedArray elements;

  for (auto i = 0; i < listExpr->getCount(); i++)
    elements.push_back(generateExpression(listExpr->getShared(i)));

  auto list = std::make_shared<List>(elements);

  // Sets the line and the column of the source code that generated this
  // expression.
  list->setSourceLocation(listExpr->getSourceLocation());

  return list;
}

//----------------------------------------------------------------------------

SharedPtr<AstNode> CodeGenerator::generateBinaryOperator(const SharedPtr<PrtList> &cmpExpr)
{
  auto createOperator = [](const std::string &opText, SharedPtr<AstNode> const &lhs, SharedPtr<AstNode> const &rhs) {
    SharedPtr<AstNode> expr;

    // Arithmetic operators
    if (opText.compare("+") == 0) {
      expr = std::make_shared<CallFunction>("__add", List::create({ lhs, rhs }));
    } else if (opText.compare("-") == 0) {
      expr = std::make_shared<CallFunction>("__sub", List::create({ lhs, rhs }));
    } else if (opText.compare("*") == 0) {
      expr = std::make_shared<CallFunction>("__mul", List::create({ lhs, rhs }));
    } else if (opText.compare("/") == 0) {
      expr = std::make_shared<CallFunction>("__div", List::create({ lhs, rhs }));
      // Comparison operators
    } else if (opText.compare("==") == 0)
      expr = std::make_shared<BinaryOperator>(BinaryOperator::EQUAL, lhs, rhs);
    else if (opText.compare("!=") == 0)
      expr = std::make_shared<BinaryOperator>(BinaryOperator::NOTEQUAL, lhs, rhs);
    else if (opText.compare(">") == 0)
      expr = std::make_shared<BinaryOperator>(BinaryOperator::GREATERTHAN, lhs, rhs);
    else if (opText.compare(">=") == 0)
      expr = std::make_shared<BinaryOperator>(BinaryOperator::GREATERTHAN_EQUAL, lhs, rhs);
    else if (opText.compare("<") == 0)
      expr = std::make_shared<BinaryOperator>(BinaryOperator::LESSTHAN, lhs, rhs);
    else if (opText.compare("<=") == 0)
      expr = std::make_shared<BinaryOperator>(BinaryOperator::LESSTHAN_EQUAL, lhs, rhs);
    // Assignment operators
    else if (opText.compare("=") == 0)
      expr = std::make_shared<AssignmentOperator>(lhs, rhs);
    // Invalid operator
    else
      throw EXCEPTION(InvalidOperationException, "Unrecognized binary operator.");

    return expr;
  };

  SharedPtr<AstNode> expr;

  for (auto i = 0; i < cmpExpr->getCount();) {
    if (i == 0) {
      auto lhs = generateExpression(cmpExpr->getShared(0));
      auto opText = static_cast<PrtToken*>(cmpExpr->get(1))->getText();
      auto rhs = generateExpression(cmpExpr->getShared(2));
      expr = createOperator(opText, lhs, rhs);
      i += 3;
    } else {
      auto opText = static_cast<PrtToken*>(cmpExpr->get(i))->getText();
      auto rhs = generateExpression(cmpExpr->getShared(i + 1));
      expr = createOperator(opText, expr, rhs);
      i += 2;
    }

    // Sets the line and the column of the source code that generated this
    // expression.
    expr->setSourceLocation(cmpExpr->getSourceLocation());
  }

  return expr;
}

SharedPtr<AstNode> CodeGenerator::generateUnaryOperator(const SharedPtr<PrtList> &unaryExpr)
{
  auto opText = static_cast<PrtToken*>(unaryExpr->get(0))->getText();
  auto expr = generateExpression(unaryExpr->getShared(1));

  if (opText.compare("+") == 0) {
    // Return the expression as it is.
    return expr;
  } else if (opText.compare("-") == 0) {
    expr = std::make_shared<CallFunction>("__neg", List::create({ expr }));
  } else if (opText.compare("--") == 0) {
    expr = std::make_shared<AssignmentOperator>(expr,
      std::make_shared<CallFunction>("__sub", List::create({ expr, std::make_shared<IntegerConst>(1) })));
  } else if (opText.compare("++") == 0) {
    expr = std::make_shared<AssignmentOperator>(expr,
      std::make_shared<CallFunction>("__add", List::create({ expr, std::make_shared<IntegerConst>(1) })));
  } else {
    throw EXCEPTION(InvalidOperationException, "Unrecognized unary operator.");
  }

  expr->setSourceLocation(unaryExpr->getSourceLocation());
  return expr;
}

SharedPtr<IfStatement> CodeGenerator::generateIfStatement(SharedPtr<IdentifiableObject> const &command)
{
  static ReferenceSeeker seeker;
  auto commandMetadata = command.ii_cast_get<ParsingMetadataHolder>();

  if (commandMetadata == 0) {
    throw EXCEPTION(SyntaxErrorException, "Invalid if command data.");
  }

  // The condition of the if statement.
  static SharedPtr<Reference> expReference = REF_PARSER->parseQualifier(
        STR("1~where(prodId=Expression.Exp)"), ReferenceUsageCriteria::MULTI_DATA);

  auto exp = getSharedPtr(seeker.tryGet(expReference.get(), command.get())).io_cast<PrtList>();

  if (exp == 0) {
    this->buildMsgStore->add(std::make_shared<Processing::CustomBuildMsg>(
                               STR("Invalid if command's condition."),
                               STR("SCG1009"), 1, commandMetadata->getSourceLocation()));
    return 0;
  }

  auto condition = generateExpression(exp);

  // The body of the if statement.
  static SharedPtr<Reference> bodyReference = REF_PARSER->parseQualifier(
        STR("2"), ReferenceUsageCriteria::MULTI_DATA);
  auto body = getSharedPtr(seeker.tryGet(bodyReference.get(), command.get()));

  if (body == 0) {
    this->buildMsgStore->add(std::make_shared<Processing::CustomBuildMsg>(
                               STR("Invalid if command's body."),
                               STR("SCG1010"), 1, commandMetadata->getSourceLocation()));
    return 0;
  }

  auto thenBody = generateInnerSet(body);

  // Creates the IfStatement instruction and sets the line and column numbers.
  auto ifStat = std::make_shared<IfStatement>(condition, thenBody, SharedPtr<Block>::null);
  ifStat->setSourceLocation(commandMetadata->getSourceLocation());

  return ifStat;
}

//----------------------------------------------------------------------------

SharedPtr<ForStatement> CodeGenerator::generateForStatement(SharedPtr<IdentifiableObject> const &command)
{
  static ReferenceSeeker seeker;

  auto commandMetadata = command.ii_cast_get<ParsingMetadataHolder>();

  if (commandMetadata == 0) {
    throw EXCEPTION(SyntaxErrorException, "Invalid for command data.");
  }

  // The condition of the for statement.
  static SharedPtr<Reference> expReference = REF_PARSER->parseQualifier(
        STR("1~where(prodId=Expression.Exp)"), ReferenceUsageCriteria::MULTI_DATA);
  auto exp = getSharedPtr(seeker.tryGet(expReference.get(), command.get())).io_cast<PrtList>();

  if (exp == 0) {
    this->buildMsgStore->add(std::make_shared<Processing::CustomBuildMsg>(
                               STR("Invalid for command condition."),
                               STR("SCG1011"), 1, commandMetadata->getSourceLocation()));
    return 0;
  }

  auto initCondLoop = generateExpression(exp);

  if (!initCondLoop->isDerivedFrom<List>()) {
    this->buildMsgStore->add(std::make_shared<Processing::CustomBuildMsg>(
                               STR("A 'for' keyword should be followed by a list of three "
                                   "expressions specifying the initial, condition, and loop "
                                   "expressions."),
                               STR("SCG1012"), 1, commandMetadata->getSourceLocation()));
    return 0;
  }

  auto initCondLoopAsList = initCondLoop.io_cast_get<List>();

  if (initCondLoopAsList->getCount() != 3) {
    this->buildMsgStore->add(std::make_shared<Processing::CustomBuildMsg>(
                               STR("A 'for' keyword should be followed by a list of three "
                                   "expressions specifying the initial, condition, and loop "
                                   "expressions."),
                               STR("SCG1012"), 1, commandMetadata->getSourceLocation()));
    return 0;
  }

  auto init = initCondLoopAsList->getShared(0).s_cast<AstNode>();
  auto cond = initCondLoopAsList->getShared(1).s_cast<AstNode>();
  auto loop = initCondLoopAsList->getShared(2).s_cast<AstNode>();

  // The body of the for statement.
  static SharedPtr<Reference> bodyReference = REF_PARSER->parseQualifier(
        STR("2"), ReferenceUsageCriteria::MULTI_DATA);
  auto body = getSharedPtr(seeker.tryGet(bodyReference.get(), command.get()));

  if (body == 0) {
    this->buildMsgStore->add(std::make_shared<Processing::CustomBuildMsg>(
                               STR("Invalid for command body."),
                               STR("SCG1013"), 1, commandMetadata->getSourceLocation()));
    return 0;
  }

  auto thenBody = generateInnerSet(body);

  // Creates the IfStatement instruction and sets the line and column numbers.
  auto forStat = std::make_shared<ForStatement>(init, cond, loop, thenBody);
  forStat->setSourceLocation(commandMetadata->getSourceLocation());

  return forStat;
}

//----------------------------------------------------------------------------

SharedPtr<WhileStatement> CodeGenerator::generateWhileStatement(SharedPtr<IdentifiableObject> const &command)
{
  static ReferenceSeeker seeker;

  auto commandMetadata = command.ii_cast_get<ParsingMetadataHolder>();

  if (commandMetadata == 0) {
    throw EXCEPTION(SyntaxErrorException, "Invalid 'while' command data.");
  }

  // The condition of the while statement.
  static SharedPtr<Reference> condReference = REF_PARSER->parseQualifier(
        STR("1~where(prodId=Expression.Exp)"), ReferenceUsageCriteria::MULTI_DATA);
  auto condAST = getSharedPtr(seeker.tryGet(condReference.get(), command.get())).io_cast<PrtList>();

  if (exp == nullptr) {
    this->buildMsgStore->add(std::make_shared<Processing::CustomBuildMsg>(
                               STR("Invalid while command condition."),
                               STR("SCG1014"), 1, commandMetadata->getSourceLocation()));
    return 0;
  }

  auto cond = generateExpression(condAST);

  // The body of the 'while' statement.
  static SharedPtr<Reference> bodyReference = REF_PARSER->parseQualifier(
        STR("2"), ReferenceUsageCriteria::MULTI_DATA);
  auto bodyAST = getSharedPtr(seeker.tryGet(bodyReference.get(), command.get()));

  if (bodyAST == nullptr) {
    this->buildMsgStore->add(std::make_shared<Processing::CustomBuildMsg>(
                               STR("Invalid while command body."),
                               STR("SCG1015"), 1, commandMetadata->getSourceLocation()));
    return 0;
  }

  auto body = generateInnerSet(bodyAST);

  // Creates the IfStatement instruction and sets the line and column numbers.
  auto whileState = std::make_shared<WhileStatement>(cond, body);
  whileState->setSourceLocation(commandMetadata->getSourceLocation());

  return whileState;
}

//----------------------------------------------------------------------------

// TODO: Change the parameter type to PrtRoute to instead.
Char const* CodeGenerator::parseToken(SharedPtr<IdentifiableObject> const &item)
{
  static ReferenceSeeker seeker;
  static SharedPtr<Reference> tokenReference = REF_PARSER->parseQualifier(
        STR("self~where(prodId=Subject.Subject1).0~where(prodId=Subject.Parameter)"),
        ReferenceUsageCriteria::MULTI_DATA);
  auto token = io_cast<PrtToken>(seeker.tryGet(tokenReference.get(), item.get()));

  if (token == nullptr)
    // TODO: Add the index of the non-token to the exception message.
    throw EXCEPTION(InvalidArgumentException,
                    "This parsed item doesn't contain a token.");

  return this->translateAliasedName(token->getText().c_str());
}

//----------------------------------------------------------------------------

SharedPtr<ValueTypeSpec> CodeGenerator::parseVariableType(Core::Basic::SharedPtr<IdentifiableObject> const &item)
{
  auto itemMetadata = item->getInterface<ParsingMetadataHolder>();

  if (itemMetadata == 0) {
    throw EXCEPTION(InvalidArgumentException, "Invalid variable type.");
  }

  if (itemMetadata->getProdId() == parameterId) {
    auto typeName = this->translateAliasedName(item.s_cast<PrtToken>()->getText().c_str());
    return std::make_shared<ValueTypeSpecByName>(typeName);
  }

  if (itemMetadata->getProdId() == subjectId) {
    // TODO: Handle the exception potentially thrown by parseToken.
    auto typeName = parseToken(item);
    return std::make_shared<ValueTypeSpecByName>(typeName);
  } else if (itemMetadata->getProdId() == functionalExpId) {
    // This is a compound type so we need to parse the modifier (ptr, ary, etc.)
    static ReferenceSeeker seeker;
    static SharedPtr<Reference> modifierReference = REF_PARSER->parseQualifier(
          STR("0~where(prodId=Subject.Subject1).0~where(prodId=Subject.Parameter)"),
          ReferenceUsageCriteria::MULTI_DATA);
    auto funcName = io_cast<PrtToken>(seeker.tryGet(modifierReference.get(), item.get()));

    if (funcName == nullptr) {
      this->buildMsgStore->add(std::make_shared<Processing::CustomBuildMsg>(
                                 STR("Invalid variable type."),
                                 STR("SCG1016"), 1, itemMetadata->getSourceLocation()));
      return std::make_shared<ValueTypeSpecByName>(STR("__INVALID__"));
    } else if (SBSTR(this->translateAliasedName(funcName->getText().c_str())) == "ptr") {
      // TODO: Re-factor this if block into a separate function.
      // Pointer to a type.
      static SharedPtr<Reference> contentTypeReference = REF_PARSER->parseQualifier(
            STR("1~where(prodId=Expression.ParamPassExp).0~where(prodId=Expression.Exp).0"),
            ReferenceUsageCriteria::MULTI_DATA);
      auto typeAstRoot = getSharedPtr(seeker.tryGet(contentTypeReference.get(), item.get()));

      if (typeAstRoot == nullptr) {
        this->buildMsgStore->add(std::make_shared<Processing::CustomBuildMsg>(
                                   STR("Invalid pointer type."),
                                   STR("SCG1017"), 1, funcName->getSourceLocation()));
        return std::make_shared<ValueTypeSpecByName>(STR("__INVALID__"));
      }

      auto contentTypeSpec = parseVariableType(typeAstRoot);
      return std::make_shared<PointerValueTypeSpec>(contentTypeSpec);
    } else if (SBSTR(this->translateAliasedName(funcName->getText().c_str())) == "ary") {
      // TODO: Re-factor this if block into a separate function.
      // Array of types
      static SharedPtr<Reference> elementTypeReference = REF_PARSER->parseQualifier(
            STR("1~where(prodId=Expression.ParamPassExp)."
                "0~where(prodId=Expression.Exp)."
                "0~where(prodId=Expression.ListExp)."
                "0"), ReferenceUsageCriteria::MULTI_DATA);
      static SharedPtr<Reference> arraySizeReference = REF_PARSER->parseQualifier(
            STR("1~where(prodId=Expression.ParamPassExp)."
                "0~where(prodId=Expression.Exp)."
                "0~where(prodId=Expression.ListExp)."
                "1~where(prodId=Subject.Subject1)."
                "0~where(prodId=Subject.Literal)"),
            ReferenceUsageCriteria::MULTI_DATA);
      auto elementTypeAst = getSharedPtr(seeker.tryGet(elementTypeReference.get(), item.get()));

      if (elementTypeAst == nullptr) {
        this->buildMsgStore->add(std::make_shared<Processing::CustomBuildMsg>(
                                   STR("Invalid array type."),
                                   STR("SCG1018"), 1, funcName->getSourceLocation()));
        return std::make_shared<ValueTypeSpecByName>(STR("__INVALID__"));
      }

      auto arraySizeAst = io_cast<PrtToken>(seeker.tryGet(arraySizeReference.get(), item.get()));

      if (arraySizeAst == nullptr) {
        this->buildMsgStore->add(std::make_shared<Processing::CustomBuildMsg>(
                                   STR("Invalid array type."),
                                   STR("SCG1018"), 1, funcName->getSourceLocation()));
        return std::make_shared<ValueTypeSpecByName>(STR("__INVALID__"));
      }

      auto elementTypeSpec = parseVariableType(elementTypeAst);
      auto arraySize = boost::lexical_cast<int>(arraySizeAst->getText());
      return std::make_shared<ArrayValueTypeSpec>(elementTypeSpec, arraySize);
    } else {
      this->buildMsgStore->add(std::make_shared<Processing::CustomBuildMsg>(
                                 STR("Invalid variable type."),
                                 STR("SCG1019"), 1, funcName->getSourceLocation(),
                                 funcName->getText().c_str()));
      return std::make_shared<ValueTypeSpecByName>(STR("__INVALID__"));
    }
  } else {
    this->buildMsgStore->add(std::make_shared<Processing::CustomBuildMsg>(
                               STR("Invalid variable type."),
                               STR("SCG1019"), 1, itemMetadata->getSourceLocation()));
    return std::make_shared<ValueTypeSpecByName>(STR("__INVALID__"));
  }
}

//----------------------------------------------------------------------------

VariableDefinition CodeGenerator::parseVariableDefinition(SharedPtr<IdentifiableObject> const &astBlockRoot)
{
  // Example of an AST block this function parses
  //
  // Expression.LowerLinkExp -- [LIST]:
  //  Subject.Subject1 -- [ROUTE]: 0
  //   Subject.Parameter -- [TOKEN]: IDENTIFIER_TOKEN ("i")
  //  [TOKEN]: CONSTANT_59 (":")
  //  Subject.Subject1 -- [ROUTE]: 0
  //   Subject.Parameter -- [TOKEN]: IDENTIFIER_TOKEN ("float")

  static ReferenceSeeker seeker;

  auto metadata = astBlockRoot.ii_cast_get<ParsingMetadataHolder>();

  if (metadata == 0) {
    throw EXCEPTION(SyntaxErrorException, "Invalid variable definition data.");
  }

  // Finds the name of the variable.
  static SharedPtr<Reference> nameReference = REF_PARSER->parseQualifier(
        STR("self~where(prodId=Expression.LowerLinkExp).0~where(prodId=Subject.Subject1)"),
        ReferenceUsageCriteria::MULTI_DATA);
  auto nameToken = getSharedPtr(seeker.tryGet(nameReference.get(), astBlockRoot.get()));

  if (nameToken == nullptr) {
    this->buildMsgStore->add(std::make_shared<Processing::CustomBuildMsg>(
                               STR("Invalid variable definition."),
                               STR("SCG1020"), 1, metadata->getSourceLocation()));
    return VariableDefinition(STR("__INVALID_TYPE__"), STR("__DUMMY_NAME__"));
  }

  // Finds the type of the variable.
  static SharedPtr<Reference> typeReference = REF_PARSER->parseQualifier(
        STR("self~where(prodId=Expression.LowerLinkExp).2"),
        ReferenceUsageCriteria::MULTI_DATA);
  auto typeAst = getSharedPtr(seeker.tryGet(typeReference.get(), astBlockRoot.get()));

  if (typeAst == nullptr) {
    this->buildMsgStore->add(std::make_shared<Processing::CustomBuildMsg>(
                               STR("Invalid variable definition."),
                               STR("SCG1020"), 1, metadata->getSourceLocation()));
    return VariableDefinition(STR("__INVALID_TYPE__"), STR("__DUMMY_NAME__"));
  }

  return VariableDefinition(parseVariableType(typeAst), parseToken(nameToken));
}

//----------------------------------------------------------------------------

VariableDefinitionArray CodeGenerator::parseFunctionArguments(SharedPtr<IdentifiableObject> const &astBlockRoot)
{
  // Example of an AST block this function parses:
  //
  // Expression.Exp -- [LIST]:
  //  Expression.ListExp -- [LIST]:
  //   Expression.LowerLinkExp -- [LIST]:
  //    Subject.Subject1 -- [ROUTE]: 0
  //     Subject.Parameter -- [TOKEN]: IDENTIFIER_TOKEN ("i")
  //    [TOKEN]: CONSTANT_59 (":")
  //    Subject.Subject1 -- [ROUTE]: 0
  //     Subject.Parameter -- [TOKEN]: IDENTIFIER_TOKEN ("float")
  //   Expression.LowerLinkExp -- [LIST]:
  //    Subject.Subject1 -- [ROUTE]: 0
  //     Subject.Parameter -- [TOKEN]: IDENTIFIER_TOKEN ("j")
  //    [TOKEN]: CONSTANT_59 (":")
  //    Subject.Subject1 -- [ROUTE]: 0
  //     Subject.Parameter -- [TOKEN]: IDENTIFIER_TOKEN ("float")

  auto astBlockRootMetadata = astBlockRoot->getInterface<ParsingMetadataHolder>();

  if (astBlockRootMetadata == 0) {
    throw EXCEPTION(SyntaxErrorException, "Invalid function argument list.");
  }

  auto id = astBlockRootMetadata->getProdId();

  VariableDefinitionArray args;

  if (id == expressionId) {
    return parseFunctionArguments(astBlockRoot.s_cast_get<PrtList>()->getShared(0));
  }

  if (id == subjectId) {
    auto route = astBlockRoot.s_cast<PrtRoute>();

    if (route->getData() == nullptr)
      return args;

    return parseFunctionArguments(route->getData());
  } else if (id == listExpId) {
    auto list = astBlockRoot.s_cast<PrtList>();

    for (int i = 0, e = (int)list->getCount(); i < e; ++i) {
      args.push_back(parseVariableDefinition(list->getShared(i)));
    }
  } else if (id == lowerLinkExpId) {
    args.push_back(parseVariableDefinition(astBlockRoot));
  } else {
    this->buildMsgStore->add(std::make_shared<Processing::CustomBuildMsg>(
                               STR("Invalid function argument list."),
                               STR("SCG1021"), 1, astBlockRootMetadata->getSourceLocation()));
  }

  return args;
}

//----------------------------------------------------------------------------

Char const* CodeGenerator::translateAliasedName(Char const *name)
{
  if (this->aliasDictionary != 0) {
    Int index = this->aliasDictionary->findIndex(name);

    if (index != -1) return static_cast<String*>(this->aliasDictionary->get(index))->get();
  }

  return name;
}

} // Scg
