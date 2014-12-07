/**
 * @file Scg/CodeGeneration/CodeGenerator.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>

// Boost header files
#include <boost/lexical_cast.hpp>

// Scg header files
#include <Containers/Block.h>
#include <Expression.h>
#include <BuiltInFunctions/AddDoubles.h>
#include <BuiltInFunctions/AddFloats.h>
#include <BuiltInFunctions/AddIntegers.h>
#include <Containers/ExpressionList.h>
#include <Containers/List.h>
#include <Containers/Module.h>
#include <CodeGeneration/CodeGenerator.h>
#include <CodeGeneration/FunctionAstBlock.h>
#include <CodeGeneration/FunctionLinkExpression.h>
#include <CodeGeneration/FunctionalExpression.h>
#include <CodeGeneration/ParamPassExp.h>
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
#include <Operators/PointerToVariable.h>
#include <Operators/UnaryOperator.h>
#include <Types/ValueTypeSpec.h>
#include <Values/DoubleConst.h>
#include <Values/FloatConst.h>
#include <Values/IntegerConst.h>
#include <Values/StringConst.h>

// LLVM header files
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Type.h>

// TODO: Used for debugging purposes for now; should be removed later.
/*namespace Scg
{
void debugPrintParsedData(
    const Core::Basic::SharedPtr<IdentifiableObject> &ptr, int indents=0);
}*/

// TODO: Optimise this by returning structures by reference rather than by value.

namespace Scg
{
  using namespace Core::Basic;
  using namespace Core::Data;

  //----------------------------------------------------------------------------

  void CodeGenerator::InitializeIds()
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
    GENERATE_ID(pointerTildeId, "Expression.Pointer_Tilde");
    GENERATE_ID(contentTildeId, "Expression.Content_Tilde");
  }

  //----------------------------------------------------------------------------

  Module *CodeGenerator::GenerateModule(const std::string &name,
  		const SharedPtr<Core::Data::Module> &srcModule)
  {
    Module *module = new Module(name);
    for (auto i = 0; i < srcModule->getCount(); i++)
    {
        auto item = srcModule->getShared(i);
        if (item == 0) {
            THROW_EXCEPTION(SyntaxErrorException, "Invalid object type in def command.");
        }
        module->AppendExpression(GenerateStatement(item));
    }

    return module;
  }

  //----------------------------------------------------------------------------

  Expression *CodeGenerator::GenerateStatement(SharedPtr<IdentifiableObject> const &item)
  {
    auto metadata = item.ii_cast_get<ParsingMetadataHolder>();
    if (metadata == 0) {
      THROW_EXCEPTION(SyntaxErrorException, "Invalid object type in def command.");
    }
    auto id = metadata->getProdId();
    if (id == defId)
      return GenerateDefine(item);
    else if (id == returnId)
      return GenerateReturn(item);
    else if (id == ifId)
      return GenerateIfStatement(item);
    else if (id == forId)
      return GenerateForStatement(item);
    else if (id == whileId)
      return GenerateWhileStatement(item);
    else if (id == linkId)
    {
      FunctionLinkExpression funcLink(this, item);
      auto declExtFunc = funcLink.ToDeclareExtFunction();
      declExtFunc->SetLineInCode(metadata->getLine());
      declExtFunc->SetColumnInCode(metadata->getColumn());
      return declExtFunc;
    }
    else
      return GenerateExpression(item);
  }

  //----------------------------------------------------------------------------

  Block *CodeGenerator::GenerateSet(const SharedPtr<ParsedList> &list)
  {
    ExpressionArray blockExprs;
    for (auto i = 0; i < list->getCount(); i++)
    {
      auto element = list->getShared(i);
      blockExprs.push_back(GenerateStatement(element));
    }

    return new Block(blockExprs);
  }

  //----------------------------------------------------------------------------

  Block *CodeGenerator::GenerateInnerSet(SharedPtr<IdentifiableObject> const &item)
  {
    static SharedPtr<Reference> setReference = ReferenceParser::parseQualifier(
      STR("self~where(prodId=Expression.Exp)."
          "0~where(prodId=Subject.Subject1)."
          "0~where(prodId=Main.StatementList)"),
      ReferenceUsageCriteria::MULTI_DATA);
    static ReferenceSeeker seeker;
    IdentifiableObject *set = seeker.tryGet(setReference.get(), item.get());
    if (set == 0) {
      ExpressionArray blockExprs;
      blockExprs.push_back(GenerateStatement(item));
      // Creates the block representing the inner set and sets its line and
      // column number.
      auto block = new Block(blockExprs);
      auto metadata = item.ii_cast_get<ParsingMetadataHolder>();
      if (metadata != nullptr) {
        block->SetLineInCode(metadata->getLine());
        block->SetColumnInCode(metadata->getColumn());
      }
      return block;
    } else {
      return GenerateSet(s_cast<ParsedList>(getSharedPtr(set)));
    }
  }

  //----------------------------------------------------------------------------

  Expression *CodeGenerator::GenerateDefine(SharedPtr<IdentifiableObject> const &item)
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

    // Get the name of the definition.
    static SharedPtr<Reference> nameReference = ReferenceParser::parseQualifier(
      STR("0~where(prodId=Expression.Exp)."
          "0~where(prodId=Expression.LowerLinkExp)."
          "0~where(prodId=Subject.Subject1)."
          "0~where(prodId=Subject.Parameter)"),
      ReferenceUsageCriteria::MULTI_DATA);
    auto nameToken = io_cast<ParsedToken>(seeker.tryGet(nameReference.get(), item.get()));
    if (nameToken == nullptr || nameToken->getId() != identifierTokenId)
      // TODO: Generate a build message instead of throwing an exception.
      THROW_EXCEPTION(SyntaxErrorException, "A 'def' command needs a definition name.");
    auto name = nameToken->getText();

    // Get the definee (after the colon).
    static SharedPtr<Reference> defReference = ReferenceParser::parseQualifier(
      STR("0~where(prodId=Expression.Exp)."
          "0~where(prodId=Expression.LowerLinkExp)."
          "2"), ReferenceUsageCriteria::MULTI_DATA);
    auto def = seeker.tryGet(defReference.get(), item.get());
    ParsingMetadataHolder *defMetadata = def==0 ? 0 : def->getInterface<ParsingMetadataHolder>();
    if (defMetadata == nullptr)
      // TODO: Generate a build message instead of throwing an exception.
      // TODO: We need to choose terms for the parts of a define command, e.g.
      // definition name, definition, etc.
      THROW_EXCEPTION(SyntaxErrorException, "A 'def' command needs a definition.");

    if (defMetadata->getProdId() == functionalExpId)
      // Defining a variable
      return GenerateDefineVariable(name, getSharedPtr(def));
    else if (defMetadata->getProdId() == subjectId)
    {
      auto routeData = static_cast<ParsedRoute*>(def)->getData();
      auto routeMetadata = routeData->getInterface<ParsingMetadataHolder>();
      if (routeMetadata->getProdId() == parameterId)
        // Defining a variable
        return GenerateDefineVariable(name, routeData);
      else if (routeMetadata->getProdId() == functionId)
        // Defining a function
        return GenerateDefineFunction(name, routeData);
      else if (routeMetadata->getProdId() == structureId)
        // Defining a structure
        return GenerateDefineStructure(name, routeData);
      else
        THROW_EXCEPTION(SyntaxErrorException, "Invalid define command.");
    }
    else
      THROW_EXCEPTION(SyntaxErrorException, "Invalid define command.");
  }

  //----------------------------------------------------------------------------

  DefineVariable *CodeGenerator::GenerateDefineVariable(const std::string &name,
      SharedPtr<IdentifiableObject> const &expr)
  {
    // Parses the variable type.
    auto type = ParseVariableType(expr);
    // Creates the DefineVariable instruction and sets its line and column numbers.
    auto defVar = new DefineVariable(type, name);
    auto exprMetadata = expr->getInterface<ParsingMetadataHolder>();
    if (exprMetadata != 0) {
      defVar->SetLineInCode(exprMetadata->getLine());
      defVar->SetColumnInCode(exprMetadata->getColumn());
    }
    return defVar;
  }

  //----------------------------------------------------------------------------

  DefineFunction *CodeGenerator::GenerateDefineFunction(const std::string &name,
      const SharedPtr<IdentifiableObject> &item)
  {
    return FunctionAstBlock(this, item.s_cast<ParsedList>()).ToDefineFunction(name);
  }

  //----------------------------------------------------------------------------

  DefineStruct *CodeGenerator::GenerateDefineStructure(const std::string &name,
                                                       SharedPtr<IdentifiableObject> const &item)
  {
    static ReferenceSeeker seeker;
    // Generate function body.
    static SharedPtr<Reference> structBodyReference = ReferenceParser::parseQualifier(
      STR("0~where(prodId=Main.StatementList)"), ReferenceUsageCriteria::MULTI_DATA);
    auto bodyStmtList = getSharedPtr(seeker.tryGet(structBodyReference.get(), item.get())).io_cast<ParsedList>();
    if (bodyStmtList == 0)
      // TODO: Generate a build message instead of throwing an exception.
      THROW_EXCEPTION(SyntaxErrorException, "A structure definition expects a body.");
    auto structBody = GenerateSet(bodyStmtList);

    // Extract members names and types.
    VariableDefinitionArray fields;
    for (auto child : structBody->GetChildren())
    {
      // TODO: Don't use dynamic_cast.
      auto field = dynamic_cast<DefineVariable*>(child);
      if (field == nullptr)
        THROW_EXCEPTION(SyntaxErrorException,
            "A structure body can only contain variable definitions.");
      auto typeSpec = field->GetVarTypeSpec()->Clone();
      auto name = field->GetVarName();
      fields.push_back(VariableDefinition(typeSpec, name));
    }
    // We are not going to attach the set (of DefineVariable's to the module) so
    // we need to delete it manually.
    delete structBody;

    // Creates the DefineStruct instruction and sets its line and column numbers.
    auto defStruct = new DefineStruct(name, fields);
    ParsingMetadataHolder *itemMetadata = item->getInterface<ParsingMetadataHolder>();
    if (itemMetadata != 0) {
      defStruct->SetLineInCode(itemMetadata->getLine());
      defStruct->SetColumnInCode(itemMetadata->getColumn());
    }
    return defStruct;
  }

  //----------------------------------------------------------------------------

  Return *CodeGenerator::GenerateReturn(SharedPtr<IdentifiableObject> const &item)
  {
    static ReferenceSeeker seeker;
    static SharedPtr<Reference> expReference = ReferenceParser::parseQualifier(
      STR("{find prodId=Expression.Exp, 0}"), ReferenceUsageCriteria::MULTI_DATA);
    auto exp = getSharedPtr(seeker.tryGet(expReference.get(), item.get()));
    if (exp == nullptr)
      THROW_EXCEPTION(SyntaxErrorException, "Invalid return argument.");

    // Creates the Return instruction and sets the line and column numbers.
    auto ret = new Return(GenerateExpression(exp));
    ParsingMetadataHolder *itemMetadata = item->getInterface<ParsingMetadataHolder>();
    if (itemMetadata != 0) {
      ret->SetLineInCode(itemMetadata->getLine());
      ret->SetColumnInCode(itemMetadata->getColumn());
    }
    return ret;
  }

  //----------------------------------------------------------------------------

  Expression* CodeGenerator::GenerateExpression(SharedPtr<IdentifiableObject> const &item)
  {
    auto itemMetadata = item->getInterface<ParsingMetadataHolder>();
    if (itemMetadata == nullptr)
      THROW_EXCEPTION(SyntaxErrorException, "Invalid expression data structure.");
    auto id = itemMetadata->getProdId();

    Expression *expr;
    if (id == expressionId)
      expr = GenerateExpression(item.s_cast_get<ParsedList>()->getShared(0));
    else if (id == subjectId)
      expr = GenerateExpression(item.s_cast_get<ParsedRoute>()->getData());
    else if (id == parameterId)
      // Subject.Subject1>Subject.Parameter are variable references.
      expr = GenerateVariableRef(item);
    else if (id == literalId)
      // Literals are constants
      expr = GenerateConst(item.s_cast<ParsedToken>());
    else if (id == functionalExpId) {
      /*auto list = item.s_cast<ParsedList>();
      auto id2 = list->get(1)->getInterface<ParsingMetadataHolder>()->getProdId();
      if (id2 == paramPassId)
        expr = GenerateFunctionCall(item.s_cast<ParsedList>());
      else if (id2 == postfixTildeExpId)
        expr = GenerateVariableRef(item);
      else if (id2 == linkExpId)
        expr = GenerateMemberAccess(item);
      else
        THROW_EXCEPTION(ArgumentOutOfRangeException, "The given parsing data doesn't evaluate to an expression.");*/
      expr = FunctionalExpression(this, item.s_cast<ParsedList>()).ToExpression();
    } else if (id == listExpId)
      expr = GenerateList(item.s_cast<ParsedList>());
    else if (id == comparisonExpId || id == assignmentExpId || id == addExpId || id == mulExpId)
      expr = GenerateBinaryOperator(item.s_cast<ParsedList>());
    else if (id == statementListId)
      expr = GenerateSet(item.s_cast<ParsedList>());
    else
      THROW_EXCEPTION(ArgumentOutOfRangeException, "The given parsing data doesn't evaluate to an expression.");

    return expr;
  }

  //----------------------------------------------------------------------------

  Expression *CodeGenerator::GenerateVariableRef(SharedPtr<IdentifiableObject> const &parsedItem)
  {
    auto parsedItemMetadata = parsedItem->getInterface<ParsingMetadataHolder>();
    if (parsedItemMetadata == nullptr) {
      THROW_EXCEPTION(SyntaxErrorException, "Invalid variable ref data structure.");
    }
    auto id = parsedItemMetadata->getProdId();

//    if (id == functionalExpId)
//    {
//      auto parsedList = parsedItem.s_cast<ParsedList>();
//      if (parsedList->getElementCount() != 2)
//        THROW_EXCEPTION(SyntaxErrorException, "Expression doesn't evaluate to a variable reference.");
//      auto varName = parsedList
//          ->getElement(0).s_cast<ParsedRoute>()
//          ->getData().s_cast<ParsedToken>()
//          ->getText();
//      auto postfix = parsedList->get(1);
//      auto postfixId = postfix->getInterface<ParsingMetadataHolder>()->getProdId();
//      if (postfixId != postfixTildeExpId)
//        THROW_EXCEPTION(SyntaxErrorException, "Expression doesn't evaluate to a variable reference.");
//      auto postfixType = postfix.s_cast<ParsedList>()
//          ->get(0)->getInterface<ParsingMetadataHolder>()->getProdId();
//      if (postfixType == pointerTildeId)
//        return new PointerToVariable(varName);
//      else if (postfixType == contentTildeId)
//        // TODO: Implement this.
//        return nullptr; //new VariableDeref(varName);
//      else
//        THROW_EXCEPTION(SyntaxErrorException, "Expression doesn't evaluate to a variable reference.");
//    }
//    else
    if (id == subjectId)
    {
      auto varName = ParseToken(parsedItem);
      return new Content(new PointerToVariable(varName));
    }
    // This shouldn't be needed anymore.
    else if (id == parameterId)
    {
      auto varName = parsedItem.s_cast_get<ParsedToken>()->getText();
      return new Content(new PointerToVariable(varName));
    }
    else
      THROW_EXCEPTION(SyntaxErrorException, "Expression doesn't evaluate to a variable reference.");
  }

//  Expression *CodeGenerator::GenerateMemberAccess(SharedPtr<IdentifiableObject> const &data)
//  {
//    auto postfixType = 0; // 0: Variable reference
//                          // 1: Variable pointer
//                          // 2: Variable dereference
//    auto parsedList = data.s_cast<ParsedList>();
//    auto varName = parsedList
//        ->getElement(0).s_cast<ParsedRoute>()
//        ->getData().s_cast<ParsedToken>()
//        ->getText();
//    std::vector<Expression*> subVarRefs;
//    for (auto i = 1; i < parsedList->getElementCount(); i++)
//    {
//      auto fieldExp = parsedList->get(i);
//      if (fieldExp->getInterface<ParsingMetadataHolder>()->getProdId() == linkExpId)
//      {
//        auto fieldName = fieldExp.s_cast<ParsedList>()
//            ->getElement(1).s_cast<ParsedRoute>()
//            ->getData().s_cast<ParsedToken>()
//            ->getText();
//        subVarRefs.push_back(new StringConst(fieldName));
//      }
//      else if (fieldExp->getProdId() == postfixTildeExpId)
//      {
//        auto postfixTypeId = fieldExp.s_cast<ParsedList>()
//            ->get(0)->getInterface<ParsingMetadataHolder>()->getProdId();
//        if (postfixTypeId == pointerTildeId)
//        {
//          postfixType = 1;
//          // TODO: We should throw an exception if there are more elements
//          // in the parsed list, as this mean that something is being added
//          // after the post-fix expression, which is a syntax error.
//          break;
//        }
//        else if (postfixTypeId == contentTildeId)
//        {
//          postfixType = 2;
//          // TODO: We should throw an exception if there are more elements
//          // in the parsed list, as this mean that something is being added
//          // after the post-fix expression, which is a syntax error.
//          break;
//        }
//        else
//          THROW_EXCEPTION(SyntaxErrorException, "Expression doesn't evaluate to a variable reference.");
//      }
//      else
//        THROW_EXCEPTION(SyntaxErrorException, "Expression doesn't evaluate to a variable reference.");
//    }

//    // Creates the expression for accessing a member and sets line and column numbers.
//    Expression *memberAccess;
//    switch (postfixType)
//    {
//    // TODO: Implement this.
//    //case 0: memberAccess = new VariableRef(varName, subVarRefs); break;
//    //case 1: memberAccess = new VariablePointer(varName, subVarRefs); break;
//    //case 2: memberAccess = new VariableDeref(varName, subVarRefs); break;
//    default:
//        THROW_EXCEPTION(UnreachableCodeException, "Unexpected value for postfixType");
//    }
//    memberAccess->SetLineInCode(parsedList->getLine());
//    memberAccess->SetColumnInCode(parsedList->getColumn());
//    return memberAccess;
//  }

  //----------------------------------------------------------------------------

  Expression *CodeGenerator::GenerateConst(const SharedPtr<ParsedToken> &literal)
  {
    auto literalId = literal->getId();
    auto literalText = literal->getText();
    Expression *constant;

    if (literalId == ID_GENERATOR->getId("LexerDefs.IntLiteral"))
      // Integral constant
      constant = new IntegerConst(boost::lexical_cast<int>(literalText));
    else if (literalId == ID_GENERATOR->getId("LexerDefs.FloatLiteral"))
    {
      if (literalText[literalText.size()-1] == 'f' ||
          literalText[literalText.size()-1] == 'F')
        // Floating point constant
        constant = new FloatConst(boost::lexical_cast<float>(
            literalText.substr(0, literalText.size()-1)));
      else
        // Double floating point constant
        constant = new DoubleConst(boost::lexical_cast<float>(literalText));
    }
    else if (literalId == ID_GENERATOR->getId("LexerDefs.StringLiteral"))
      // String constant
      constant = new StringConst(literalText);
    else
      THROW_EXCEPTION(NotImplementedException, "Not implemented yet.");

    // Sets the line and the column of the source code that generated this
    // expression.
    constant->SetLineInCode(literal->getLine());
    constant->SetColumnInCode(literal->getColumn());

    return constant;
  }

  //----------------------------------------------------------------------------

  List *CodeGenerator::GenerateList(const SharedPtr<ParsedList> &listExpr)
  {
    // Generate an array containing the expressions representing the elements of
    // the list.
    ExpressionArray elements;
    for (auto i = 0; i < listExpr->getCount(); i++)
      elements.push_back(GenerateExpression(listExpr->getShared(i)));

    auto list = new List(elements);

    // Sets the line and the column of the source code that generated this
    // expression.
    list->SetLineInCode(listExpr->getLine());
    list->SetColumnInCode(listExpr->getColumn());

    return list;
  }

  //----------------------------------------------------------------------------

  Expression *CodeGenerator::GenerateBinaryOperator(const SharedPtr<ParsedList> &cmpExpr)
  {
    auto lhs = GenerateExpression(cmpExpr->getShared(0));
    auto rhs = GenerateExpression(cmpExpr->getShared(2));
    auto opText = static_cast<ParsedToken*>(cmpExpr->get(1))->getText();
    Expression *expr;
    // Arithmetic operators
    if (opText.compare("+") == 0)
      expr = new BinaryOperator(BinaryOperator::ADD, lhs, rhs);
    else if (opText.compare("-") == 0)
      expr = new BinaryOperator(BinaryOperator::SUBTRACT, lhs, rhs);
    else if (opText.compare("*") == 0)
      expr = new BinaryOperator(BinaryOperator::MULTIPLY, lhs, rhs);
    else if (opText.compare("/") == 0)
      expr = new BinaryOperator(BinaryOperator::DIVISION, lhs, rhs);
    // Comparison operators
    else if (opText.compare("=") == 0)
      expr = new BinaryOperator(BinaryOperator::EQUAL, lhs, rhs);
    else if (opText.compare("^=") == 0)
      expr = new BinaryOperator(BinaryOperator::NOTEQUAL, lhs, rhs);
    else if (opText.compare(">") == 0)
      expr = new BinaryOperator(BinaryOperator::GREATERTHAN, lhs, rhs);
    else if (opText.compare(">=") == 0)
      expr = new BinaryOperator(BinaryOperator::GREATERTHAN_EQUAL, lhs, rhs);
    else if (opText.compare("<") == 0)
      expr = new BinaryOperator(BinaryOperator::LESSTHAN, lhs, rhs);
    else if (opText.compare("<=") == 0)
      expr = new BinaryOperator(BinaryOperator::LESSTHAN_EQUAL, lhs, rhs);
    // Assignment operators
    else if (opText.compare(":=") == 0)
      expr = new AssignmentOperator(lhs, rhs);
    // Invalid operator
    else
      THROW_EXCEPTION(InvalidOperationException, "Unrecognized binary operator.");

    // Sets the line and the column of the source code that generated this
    // expression.
    expr->SetLineInCode(cmpExpr->getLine());
    expr->SetColumnInCode(cmpExpr->getColumn());

    return expr;
  }

  //----------------------------------------------------------------------------

//  CallFunction *CodeGenerator::GenerateFunctionCall(const SharedPtr<ParsedList> &functionalExpr)
//  {
//    if (functionalExpr->getElementCount() != 2) {
//      THROW_EXCEPTION(SyntaxErrorException, "A function call should have a name and parameter list.");
//    }

//    // Get the name.
//    static ReferenceSeeker seeker;
//    static SharedPtr<Reference> nameReference = ReferenceParser::parseQualifier(
//      STR("0~where(prodId=Subject.Subject1).{find prodId=Subject.Parameter, 0}"),
//      ReferenceUsageCriteria::MULTI_DATA);
//    auto name = seeker.tryGet<ParsedToken>(nameReference.get(), functionalExpr.get());
//    if (name == 0) {
//      THROW_EXCEPTION(SyntaxErrorException, "A function call should have a name and parameter list.");
//    }
//    if (name->getId() != identifierTokenId) {
//      THROW_EXCEPTION(SyntaxErrorException, "A function call should have a name and parameter list.");
//    }
//    auto functionName = name->getText();

//    // Get the parameter.
//    static SharedPtr<Reference> paramsReference = ReferenceParser::parseQualifier(
//      STR("1~where(prodId=Expression.ParamPassExp).0"), ReferenceUsageCriteria::MULTI_DATA);
//    auto params = seeker.tryGet(paramsReference.get(), functionalExpr.get());
//    Expression *functionArgs = 0;
//    if (params != 0) {
//      functionArgs = GenerateExpression(params);
//    }

//    // Force arguments to be a list.
//    if (dynamic_cast<List*>(functionArgs) == 0) {
//      ExpressionArray ar;
//      if (functionArgs != 0) ar.push_back(functionArgs);
//      functionArgs = new List(ar);
//    }

//    // Creates the CallFunction instruction and sets the line and column numbers.
//    auto callFunc = new CallFunction(functionName, static_cast<List*>(functionArgs));
//    callFunc->SetLineInCode(functionalExpr->getLine());
//    callFunc->SetColumnInCode(functionalExpr->getColumn());
//    return callFunc;
//  }

  //----------------------------------------------------------------------------

  /*List *CodeGenerator::GenerateOperand(const SharedPtr< > &functionalExpr)
  {
    auto expr = GenerateExpression(functionalExpr.s_cast_get<ParsedRoute>()->getData());
    if (dynamic_cast<List*>(expr) != 0)
      return static_cast<List*>(expr);
    ExpressionArray exprs;
    exprs.push_back(expr);
    return new List(exprs);
  }*/

  //----------------------------------------------------------------------------

  IfStatement *CodeGenerator::GenerateIfStatement(SharedPtr<IdentifiableObject> const &command)
  {
    static ReferenceSeeker seeker;
    // The condition of the if statement.
    static SharedPtr<Reference> expReference = ReferenceParser::parseQualifier(
      STR("0~where(prodId=Expression.Exp)"), ReferenceUsageCriteria::MULTI_DATA);

    auto exp = getSharedPtr(seeker.tryGet(expReference.get(), command.get())).io_cast<ParsedList>();
    if (exp == 0)
      THROW_EXCEPTION(SyntaxErrorException, "Invalid if command's condition.");
    auto condition = GenerateExpression(exp);

    // The body of the if statement.
    static SharedPtr<Reference> bodyReference = ReferenceParser::parseQualifier(
      STR("1"), ReferenceUsageCriteria::MULTI_DATA);
    auto body = getSharedPtr(seeker.tryGet(bodyReference.get(), command.get()));
    if (body == 0)
      THROW_EXCEPTION(SyntaxErrorException, "Invalid if command's body.");
    auto thenBody = GenerateInnerSet(body);

    // Creates the IfStatement instruction and sets the line and column numbers.
    auto ifStat = new IfStatement(condition, thenBody, 0);
    auto commandMetadata = command->getInterface<ParsingMetadataHolder>();
    if (commandMetadata != 0) {
      ifStat->SetLineInCode(commandMetadata->getLine());
      ifStat->SetColumnInCode(commandMetadata->getColumn());
    }
    return ifStat;
  }

  //----------------------------------------------------------------------------

  ForStatement *CodeGenerator::GenerateForStatement(SharedPtr<IdentifiableObject> const &command)
  {
    static ReferenceSeeker seeker;

    // The condition of the for statement.
    static SharedPtr<Reference> expReference = ReferenceParser::parseQualifier(
      STR("0~where(prodId=Expression.Exp)"), ReferenceUsageCriteria::MULTI_DATA);
    auto exp = getSharedPtr(seeker.tryGet(expReference.get(), command.get())).io_cast<ParsedList>();
    if (exp == 0)
      THROW_EXCEPTION(SyntaxErrorException, "Invalid for command's condition.");
    auto initCondLoop = GenerateExpression(exp);
    if (dynamic_cast<List*>(initCondLoop) == 0)
      THROW_EXCEPTION(SyntaxErrorException, "A 'for' keyword should be followed "
      "by a list of three expressions specifying the initial, condition, and "
      "and loop expressions.");
    auto initCondLoopAsList = dynamic_cast<List*>(initCondLoop);
    if (initCondLoopAsList->GetElementCount() != 3)
      THROW_EXCEPTION(SyntaxErrorException, "A 'for' keyword should be followed "
      "by a list of three expressions specifying the initial, condition, and "
      "loop expressions.");
    auto init = initCondLoopAsList->GetElement(0);
    auto cond = initCondLoopAsList->GetElement(1);
    auto loop = initCondLoopAsList->GetElement(2);

    // The body of the for statement.
    static SharedPtr<Reference> bodyReference = ReferenceParser::parseQualifier(
      STR("1"), ReferenceUsageCriteria::MULTI_DATA);
    auto body = getSharedPtr(seeker.tryGet(bodyReference.get(), command.get()));
    if (body == 0)
      THROW_EXCEPTION(SyntaxErrorException, "Invalid if command's body.");
    auto thenBody = GenerateInnerSet(body);

    // Creates the IfStatement instruction and sets the line and column numbers.
    auto forStat = new ForStatement(init, cond, loop, thenBody);
    auto commandMetadata = command->getInterface<ParsingMetadataHolder>();
    if (commandMetadata != 0) {
      forStat->SetLineInCode(commandMetadata->getLine());
      forStat->SetColumnInCode(commandMetadata->getColumn());
    }
    return forStat;
  }

  //----------------------------------------------------------------------------

  WhileStatement *CodeGenerator::GenerateWhileStatement(SharedPtr<IdentifiableObject> const &command)
  {
    static ReferenceSeeker seeker;

    // The condition of the while statement.
    static SharedPtr<Reference> condReference = ReferenceParser::parseQualifier(
      STR("0~where(prodId=Expression.Exp)"), ReferenceUsageCriteria::MULTI_DATA);
    auto condAST = getSharedPtr(seeker.tryGet(condReference.get(), command.get())).io_cast<ParsedList>();
    if (exp == nullptr)
      THROW_EXCEPTION(SyntaxErrorException, "Invalid 'while' command's condition.");
    auto cond = GenerateExpression(condAST);

    // The body of the 'while' statement.
    static SharedPtr<Reference> bodyReference = ReferenceParser::parseQualifier(
      STR("1"), ReferenceUsageCriteria::MULTI_DATA);
    auto bodyAST = getSharedPtr(seeker.tryGet(bodyReference.get(), command.get()));
    if (bodyAST == nullptr)
      THROW_EXCEPTION(SyntaxErrorException, "Invalid 'while' command's body.");
    auto body = GenerateInnerSet(bodyAST);

    // Creates the IfStatement instruction and sets the line and column numbers.
    auto whileState = new WhileStatement(cond, body);
    auto commandMetadata = command->getInterface<ParsingMetadataHolder>();
    if (commandMetadata != 0) {
      whileState->SetLineInCode(commandMetadata->getLine());
      whileState->SetColumnInCode(commandMetadata->getColumn());
    }
    return whileState;
  }

  //----------------------------------------------------------------------------

  Expression *CodeGenerator::GenerateLinkStatement(SharedPtr<IdentifiableObject> const &command)
  {
    static ReferenceSeeker seeker;
    static SharedPtr<Reference> listReference = ReferenceParser::parseQualifier(
      STR("0~where(prodId=Expression.Exp)."
          "0~where(prodId=Subject.Subject1)."
          "0~where(prodId=Main.StatementList)"),
      ReferenceUsageCriteria::MULTI_DATA);
    static SharedPtr<Reference> exprReference = ReferenceParser::parseQualifier(
      STR("0~where(prodId=Expression.Exp)"), ReferenceUsageCriteria::MULTI_DATA);

    auto list = io_cast<ParsedList>(seeker.tryGet(listReference.get(), command.get()));
    if (list != 0)
    {
      auto exprList = new ExpressionList();
      for (auto i = 0; i < list->getCount(); i++)
      {
        FunctionLinkExpression funcLink(this, list->getShared(i));
        exprList->AppendExpression(funcLink.ToDeclareExtFunction());
      }
      auto commandMetadata = command->getInterface<ParsingMetadataHolder>();
      if (commandMetadata != 0) {
        exprList->SetLineInCode(commandMetadata->getLine());
        exprList->SetColumnInCode(commandMetadata->getColumn());
      }
      return exprList;
    }
    else
    {
      auto expr = getSharedPtr(seeker.tryGet(exprReference.get(), command.get())).io_cast<ParsedList>();
      if (expr != 0)
      {
        FunctionLinkExpression funcLink(this, expr);
        return funcLink.ToDeclareExtFunction();
      }
      else
        // TODO: Improve exception message.
        THROW_EXCEPTION(SyntaxErrorException, "Invalid link statement.");
    }
  }

  //----------------------------------------------------------------------------

  // TODO: Change the parameter type to ParsedRoute to instead.
  std::string CodeGenerator::ParseToken(SharedPtr<IdentifiableObject> const &item)
  {
    static ReferenceSeeker seeker;
    static SharedPtr<Reference> tokenReference = ReferenceParser::parseQualifier(
      STR("self~where(prodId=Subject.Subject1).0~where(prodId=Subject.Parameter)"),
      ReferenceUsageCriteria::MULTI_DATA);
    auto token = io_cast<ParsedToken>(seeker.tryGet(tokenReference.get(), item.get()));
    if (token == nullptr)
      // TODO: Add the index of the non-token to the exception message.
      THROW_EXCEPTION(InvalidArgumentException,
          "This parsed item doesn't contain a token.");
    return token->getText();
  }

  //----------------------------------------------------------------------------

  ValueTypeSpec *CodeGenerator::ParseVariableType(
      Core::Basic::SharedPtr<IdentifiableObject> const &item)
  {
    auto itemMetadata = item->getInterface<ParsingMetadataHolder>();
    if (itemMetadata == 0) {
      THROW_EXCEPTION(InvalidArgumentException, "Invalid variable type.");
    }
    std::string typeName;
    if (itemMetadata->getProdId() == parameterId)
    {
      auto typeName = item.s_cast<ParsedToken>()->getText();
      return new ValueTypeSpecByName(typeName);
    }
    if (itemMetadata->getProdId() == subjectId)
    {
      // TODO: Handle the exception potentially thrown by ParseToken.
      typeName = ParseToken(item);
      return new ValueTypeSpecByName(typeName);
    }
    else if (itemMetadata->getProdId() == functionalExpId)
    {
      // This is a compound type so we need to parse the modifier (ptr, arr, etc.)
      static ReferenceSeeker seeker;
      static SharedPtr<Reference> modifierReference = ReferenceParser::parseQualifier(
        STR("0~where(prodId=Subject.Subject1).0~where(prodId=Subject.Parameter)"),
        ReferenceUsageCriteria::MULTI_DATA);
      auto funcName = io_cast<ParsedToken>(seeker.tryGet(modifierReference.get(), item.get()));
      if (funcName == nullptr)
        THROW_EXCEPTION(SyntaxErrorException, "Invalid variable type.");

      else if (funcName->getText() == "ptr")
      {
        // TODO: Re-factor this if block into a separate function.
        // Pointer to a type.
        static SharedPtr<Reference> contentTypeReference = ReferenceParser::parseQualifier(
          STR("1~where(prodId=Expression.ParamPassExp).0~where(prodId=Expression.Exp).0"),
          ReferenceUsageCriteria::MULTI_DATA);
        auto typeAstRoot = getSharedPtr(seeker.tryGet(contentTypeReference.get(), item.get()));
        if (typeAstRoot == nullptr)
          THROW_EXCEPTION(SyntaxErrorException, "Invalid pointer type.");
        auto contentTypeSpec = ParseVariableType(typeAstRoot);
        return new PointerValueTypeSpec(contentTypeSpec);
      }
      else if (funcName->getText() == "arr")
      {
        // TODO: Re-factor this if block into a separate function.
        // Array of types
        static SharedPtr<Reference> elementTypeReference = ReferenceParser::parseQualifier(
          STR("1~where(prodId=Expression.ParamPassExp)."
              "0~where(prodId=Expression.Exp)."
              "0~where(prodId=Expression.ListExp)."
              "0"), ReferenceUsageCriteria::MULTI_DATA);
        static SharedPtr<Reference> arraySizeReference = ReferenceParser::parseQualifier(
          STR("1~where(prodId=Expression.ParamPassExp)."
              "0~where(prodId=Expression.Exp)."
              "0~where(prodId=Expression.ListExp)."
              "1~where(prodId=Subject.Subject1)."
              "0~where(prodId=Subject.Literal)"),
          ReferenceUsageCriteria::MULTI_DATA);
        auto elementTypeAst = getSharedPtr(seeker.tryGet(elementTypeReference.get(), item.get()));
        if (elementTypeAst == nullptr)
          THROW_EXCEPTION(SyntaxErrorException, "Invalid array type.");
        auto arraySizeAst = io_cast<ParsedToken>(seeker.tryGet(arraySizeReference.get(), item.get()));
        if (arraySizeAst == nullptr)
          THROW_EXCEPTION(SyntaxErrorException, "Invalid array type.");
        auto elementTypeSpec = ParseVariableType(elementTypeAst);
        auto arraySize = boost::lexical_cast<int>(arraySizeAst->getText());
        return new ArrayValueTypeSpec(elementTypeSpec, arraySize);
      }
      else
        THROW_EXCEPTION(SyntaxErrorException, "Invalid variable type.");
    }
    else
      THROW_EXCEPTION(InvalidArgumentException, "Invalid variable type.");
  }

  //----------------------------------------------------------------------------

  VariableDefinition CodeGenerator::ParseVariableDefinition(
      SharedPtr<IdentifiableObject> const &astBlockRoot)
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

    // Finds the name of the variable.
    static SharedPtr<Reference> nameReference = ReferenceParser::parseQualifier(
      STR("self~where(prodId=Expression.LowerLinkExp).0~where(prodId=Subject.Subject1)"),
      ReferenceUsageCriteria::MULTI_DATA);
    auto nameToken = getSharedPtr(seeker.tryGet(nameReference.get(), astBlockRoot.get()));
    if (nameToken == nullptr) {
      // TODO: Replace the exception with a build message.
      THROW_EXCEPTION(SyntaxErrorException, "Invalid variable definition.");
    }

    // Finds the type of the variable.
    static SharedPtr<Reference> typeReference = ReferenceParser::parseQualifier(
      STR("self~where(prodId=Expression.LowerLinkExp).2"),
      ReferenceUsageCriteria::MULTI_DATA);
    auto typeAst = getSharedPtr(seeker.tryGet(typeReference.get(), astBlockRoot.get()));
    if (typeAst == nullptr) {
      // TODO: Replace the exception with a build message.
      THROW_EXCEPTION(SyntaxErrorException, "Invalid variable definition.");
    }

    return VariableDefinition(ParseVariableType(typeAst), ParseToken(nameToken));
  }

  //----------------------------------------------------------------------------

  VariableDefinitionArray CodeGenerator::ParseFunctionArguments(
      SharedPtr<IdentifiableObject> const &astBlockRoot)
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
      THROW_EXCEPTION(SyntaxErrorException, "Invalid function argument list.");
    }
    auto id = astBlockRootMetadata->getProdId();

    VariableDefinitionArray args;
    if (id == expressionId) {
      return ParseFunctionArguments(astBlockRoot.s_cast_get<ParsedList>()
          ->getShared(0));
    } if (id == subjectId) {
      auto route = astBlockRoot.s_cast<ParsedRoute>();
      if (route->getData() == nullptr)
        return args;
      return ParseFunctionArguments(route->getData());
    } else if (id == listExpId) {
      auto list = astBlockRoot.s_cast<ParsedList>();
      for (int i = 0, e = (int)list->getCount(); i < e; ++i) {
        args.push_back(ParseVariableDefinition(list->getShared(i)));
      }
    } else if (id == lowerLinkExpId) {
      args.push_back(ParseVariableDefinition(astBlockRoot));
    }
    else
      THROW_EXCEPTION(SyntaxErrorException, "Invalid function argument list.");

    return args;
  }

} // Scg
