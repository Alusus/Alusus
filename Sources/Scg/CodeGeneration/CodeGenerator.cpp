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
  using Core::Basic::SharedPtr;
  using Core::Standard::ParsedItem;
  using Core::Standard::ParsedToken;
  using Core::Standard::ParsedList;
  using Core::Standard::ParsedRoute;
  using Core::Standard::ParsedDataBrowser;

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
    GENERATE_ID(identifierTokenId, "IDENTIFIER_TOKEN");

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

  Module *CodeGenerator::GenerateModule(const SharedPtr<Core::Data::Module> &srcModule)
  {
    Module *module = new Module("main");
    for (auto i = 0; i < srcModule->getCount(); i++)
    {
        auto item = srcModule->get(i).io_cast<ParsedItem>();
        if (item == 0) {
            THROW_EXCEPTION(SyntaxErrorException, "Invalid object type in def command.");
        }
        module->AppendExpression(GenerateStatement(item));
    }

    return module;
  }

  //----------------------------------------------------------------------------

  Expression *CodeGenerator::GenerateStatement(const SharedPtr<ParsedItem> &item)
  {
    auto id = item.io_cast_get<ParsedItem>()->getProdId();
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
      declExtFunc->SetLineInCode(item->getLine());
      declExtFunc->SetColumnInCode(item->getColumn());
      return declExtFunc;
    }
    else
      return GenerateExpression(item);
  }

  //----------------------------------------------------------------------------

  Block *CodeGenerator::GenerateSet(const SharedPtr<ParsedList> &list)
  {
    ExpressionArray blockExprs;
    for (auto i = 0; i < list->getElementCount(); i++)
    {
      auto element = list->getElement(i).s_cast<ParsedItem>();
      blockExprs.push_back(GenerateStatement(element));
    }

    return new Block(blockExprs);
  }

  //----------------------------------------------------------------------------

  Block *CodeGenerator::GenerateInnerSet(const SharedPtr<ParsedItem> &item)
  {
    static ParsedDataBrowser setBrowser(STR("Expression.Exp>"
                                              "0:Subject.Subject1>"
                                              "0:Main.StatementList"));
    auto set = setBrowser.getValue<ParsedItem>(item);
    if (set == 0) {
      ExpressionArray blockExprs;
      blockExprs.push_back(GenerateStatement(item));
      // Creates the block representing the inner set and sets its line and
      // column number.
      auto block = new Block(blockExprs);
      block->SetLineInCode(item->getLine());
      block->SetColumnInCode(item->getColumn());
      return block;
    } else {
      return GenerateSet(set.s_cast<ParsedList>());
    }
  }

  //----------------------------------------------------------------------------

  Expression *CodeGenerator::GenerateDefine(const SharedPtr<ParsedItem> &item)
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

    // Get the name of the definition.
    static ParsedDataBrowser nameBrowser(
        STR("0:Expression.Exp>"
            "0:Expression.LowerLinkExp>"
            "0:Subject.Subject1>"
            "0:Subject.Parameter"));
    auto nameToken = nameBrowser.getChildValue<ParsedToken>(item);
    if (nameToken == nullptr || nameToken->getId() != identifierTokenId)
      // TODO: Generate a build message instead of throwing an exception.
      THROW_EXCEPTION(SyntaxErrorException, "A 'def' command needs a definition name.");
    auto name = nameToken->getText();

    // Get the definee (after the colon).
    static ParsedDataBrowser defBrowser(
        STR("0:Expression.Exp>"
            "0:Expression.LowerLinkExp>"
            "2"));
    auto def = defBrowser.getChildValue<ParsedItem>(item);
    if (def == nullptr)
      // TODO: Generate a build message instead of throwing an exception.
      // TODO: We need to choose terms for the parts of a define command, e.g.
      // definition name, definition, etc.
      THROW_EXCEPTION(SyntaxErrorException, "A 'def' command needs a definition.");

    if (def->getProdId() == functionalExpId)
      // Defining a variable
      return GenerateDefineVariable(name, def);
    else if (def->getProdId() == subjectId)
    {
      auto route = def.s_cast<ParsedRoute>();
      auto routeData = route->getData().s_cast<ParsedItem>();
      if (routeData->getProdId() == parameterId)
        // Defining a variable
        return GenerateDefineVariable(name, routeData);
      else if (routeData->getProdId() == functionId)
        // Defining a function
        return GenerateDefineFunction(name, routeData);
      else if (routeData->getProdId() == structureId)
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
      const SharedPtr<ParsedItem> &expr)
  {
    // Parses the variable type.
    auto type = ParseVariableType(expr);
    // Creates the DefineVariable instruction and sets its line and column numbers.
    auto defVar = new DefineVariable(type, name);
    defVar->SetLineInCode(expr->getLine());
    defVar->SetColumnInCode(expr->getColumn());
    return defVar;
  }

  //----------------------------------------------------------------------------

  DefineFunction *CodeGenerator::GenerateDefineFunction(const std::string &name,
      const SharedPtr<ParsedItem> &item)
  {
    return FunctionAstBlock(this, item.s_cast<ParsedList>()).ToDefineFunction(name);
  }

  //----------------------------------------------------------------------------

  DefineStruct *CodeGenerator::GenerateDefineStructure(const std::string &name, const SharedPtr<ParsedItem> &item)
  {
    // Generate function body.
    static ParsedDataBrowser structBodyBrowser(STR("0:Main.StatementList"));
    auto bodyStmtList = structBodyBrowser.getChildValue<ParsedList>(item);
    if (bodyStmtList == nullptr)
      // TODO: Generate a build message instead of throwing an exception.
      THROW_EXCEPTION(SyntaxErrorException, "A structure definition expects a body.");
    auto structBody = GenerateSet(bodyStmtList);

    // Extract members names and types.
    VariableDefinitionArray fields;
    for (auto item : structBody->GetChildren())
    {
      // TODO: Don't use dynamic_cast.
      auto field = dynamic_cast<DefineVariable*>(item);
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
    defStruct->SetLineInCode(item->getLine());
    defStruct->SetColumnInCode(item->getColumn());
    return defStruct;
  }

  //----------------------------------------------------------------------------

  Return *CodeGenerator::GenerateReturn(const SharedPtr<ParsedItem> &item)
  {
    static ParsedDataBrowser expBrowser(STR("Expression.Exp"));
    auto exp = expBrowser.getChildValue<ParsedItem>(item.s_cast<ParsedItem>());
    if (exp == nullptr)
      THROW_EXCEPTION(SyntaxErrorException, "Invalid return argument.");

    // Creates the Return instruction and sets the line and column numbers.
    auto ret = new Return(GenerateExpression(exp));
    ret->SetLineInCode(item->getLine());
    ret->SetColumnInCode(item->getColumn());
    return ret;
  }

  //----------------------------------------------------------------------------

  Expression *CodeGenerator::GenerateExpression(const SharedPtr<ParsedItem> &item)
  {
    auto id = item->getProdId();

    Expression *expr;
    if (id == expressionId)
      expr = GenerateExpression(item.s_cast<ParsedList>()->getElement(0).s_cast<ParsedItem>());
    else if (id == subjectId)
      expr = GenerateExpression(item.s_cast<ParsedRoute>()->getData().s_cast<ParsedItem>());
    else if (id == parameterId)
      // Subject.Subject1>Subject.Parameter are variable references.
      expr = GenerateVariableRef(item);
    else if (id == literalId)
      // Literals are constants
      expr = GenerateConst(item.s_cast<ParsedToken>());
    else if (id == functionalExpId) {
      /*auto list = item.s_cast<ParsedList>();
      auto id2 = list->getElement(1).s_cast<ParsedItem>()->getProdId();
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

  Expression *CodeGenerator::GenerateVariableRef(const SharedPtr<ParsedItem> &parsedItem)
  {
    auto id = parsedItem->getProdId();

//    if (id == functionalExpId)
//    {
//      auto parsedList = parsedItem.s_cast<ParsedList>();
//      if (parsedList->getElementCount() != 2)
//        THROW_EXCEPTION(SyntaxErrorException, "Expression doesn't evaluate to a variable reference.");
//      auto varName = parsedList
//          ->getElement(0).s_cast<ParsedRoute>()
//          ->getData().s_cast<ParsedToken>()
//          ->getText();
//      auto postfix = parsedList->getElement(1).s_cast<ParsedItem>();
//      if (postfix->getProdId() != postfixTildeExpId)
//        THROW_EXCEPTION(SyntaxErrorException, "Expression doesn't evaluate to a variable reference.");
//      auto postfixType = postfix.s_cast<ParsedList>()
//          ->getElement(0).s_cast<ParsedItem>()->getProdId();
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

//  Expression *CodeGenerator::GenerateMemberAccess(const SharedPtr<IdentifiableObject> &data)
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
//      auto fieldExp = parsedList->getElement(i).s_cast<ParsedItem>();
//      if (fieldExp->getProdId() == linkExpId)
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
//            ->getElement(0).s_cast<ParsedItem>()->getProdId();
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

    if (literalId == ID_GENERATOR->getId("INT_LITERAL_TOKEN"))
      // Integral constant
      constant = new IntegerConst(boost::lexical_cast<int>(literalText));
    else if (literalId == ID_GENERATOR->getId("FLOAT_LITERAL_TOKEN"))
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
    else if (literalId == ID_GENERATOR->getId("STRING_LITERAL_TOKEN"))
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
    for (auto i = 0; i < listExpr->getElementCount(); i++)
      elements.push_back(GenerateExpression(listExpr->getElement(i).s_cast<ParsedItem>()));

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
    auto lhs = GenerateExpression(cmpExpr->getElement(0).s_cast<ParsedItem>());
    auto rhs = GenerateExpression(cmpExpr->getElement(2).s_cast<ParsedItem>());
    auto opText = cmpExpr->getElement(1).s_cast_get<ParsedToken>()->getText();
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
//    static ParsedDataBrowser nameBrowser;
//    if (!nameBrowser.isInitialized()) {
//      nameBrowser.initialize(STR("0:Subject.Subject1>Subject.Parameter"));
//    }
//    auto name = nameBrowser.getChildValue<ParsedToken>(functionalExpr);
//    if (name == 0) {
//      THROW_EXCEPTION(SyntaxErrorException, "A function call should have a name and parameter list.");
//    }
//    if (name->getId() != identifierTokenId) {
//      THROW_EXCEPTION(SyntaxErrorException, "A function call should have a name and parameter list.");
//    }
//    auto functionName = name->getText();

//    // Get the parameter.
//    static ParsedDataBrowser paramsBrowser;
//    if (!paramsBrowser.isInitialized()) {
//      paramsBrowser.initialize(STR("1:Expression.ParamPassExp>0"));
//    }
//    auto params = paramsBrowser.getChildValue<ParsedItem>(functionalExpr);
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

  IfStatement *CodeGenerator::GenerateIfStatement(const SharedPtr<ParsedItem> &command)
  {
    // The condition of the if statement.
    static ParsedDataBrowser expBrowser(STR("0:Expression.Exp"));

    auto exp = expBrowser.getChildValue<ParsedList>(command);
    if (exp == 0)
      THROW_EXCEPTION(SyntaxErrorException, "Invalid if command's condition.");
    auto condition = GenerateExpression(exp);

    // The body of the if statement.
    static ParsedDataBrowser bodyBrowser(STR("1"));
    auto body = bodyBrowser.getChildValue<ParsedItem>(command);
    if (body == 0)
      THROW_EXCEPTION(SyntaxErrorException, "Invalid if command's body.");
    auto thenBody = GenerateInnerSet(body);

    // Creates the IfStatement instruction and sets the line and column numbers.
    auto ifStat = new IfStatement(condition, thenBody, 0);
    ifStat->SetLineInCode(command->getLine());
    ifStat->SetColumnInCode(command->getColumn());
    return ifStat;
  }

  //----------------------------------------------------------------------------

  ForStatement *CodeGenerator::GenerateForStatement(const SharedPtr<ParsedItem> &command)
  {
    // The condition of the for statement.
    static ParsedDataBrowser expBrowser(STR("0:Expression.Exp"));
    auto exp = expBrowser.getChildValue<ParsedList>(command);
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
    static ParsedDataBrowser bodyBrowser(STR("1"));
    auto body = bodyBrowser.getChildValue<ParsedItem>(command);
    if (body == 0)
      THROW_EXCEPTION(SyntaxErrorException, "Invalid if command's body.");
    auto thenBody = GenerateInnerSet(body);

    // Creates the IfStatement instruction and sets the line and column numbers.
    auto forStat = new ForStatement(init, cond, loop, thenBody);
    forStat->SetLineInCode(command->getLine());
    forStat->SetColumnInCode(command->getColumn());
    return forStat;
  }

  //----------------------------------------------------------------------------

  WhileStatement *CodeGenerator::GenerateWhileStatement(const SharedPtr<ParsedItem> &command)
  {
    // The condition of the while statement.
    static ParsedDataBrowser condBrowser(STR("0:Expression.Exp"));
    auto condAST = condBrowser.getChildValue<ParsedList>(command);
    if (exp == nullptr)
      THROW_EXCEPTION(SyntaxErrorException, "Invalid 'while' command's condition.");
    auto cond = GenerateExpression(condAST);

    // The body of the 'while' statement.
    static ParsedDataBrowser bodyBrowser(STR("1"));
    auto bodyAST = bodyBrowser.getChildValue<ParsedItem>(command);
    if (bodyAST == nullptr)
      THROW_EXCEPTION(SyntaxErrorException, "Invalid 'while' command's body.");
    auto body = GenerateInnerSet(bodyAST);

    // Creates the IfStatement instruction and sets the line and column numbers.
    auto whileState = new WhileStatement(cond, body);
    whileState->SetLineInCode(command->getLine());
    whileState->SetColumnInCode(command->getColumn());
    return whileState;
  }

  //----------------------------------------------------------------------------

  Expression *CodeGenerator::GenerateLinkStatement(const SharedPtr<ParsedItem> &command)
  {
    static ParsedDataBrowser listBrowser(
        STR("0:Expression.Exp>"
            "0:Subject.Subject1>"
            "0:Main.StatementList"));
    static ParsedDataBrowser exprBrowser(
        STR("0:Expression.Exp"));

    auto list = listBrowser.getChildValue<ParsedList>(command);
    if (list != 0)
    {
      auto exprList = new ExpressionList();
      for (auto i = 0; i < list->getElementCount(); i++)
      {
        FunctionLinkExpression funcLink(this,
            list->getElement(i).s_cast<ParsedItem>());
        exprList->AppendExpression(funcLink.ToDeclareExtFunction());
      }
      exprList->SetLineInCode(command->getLine());
      exprList->SetColumnInCode(command->getColumn());
      return exprList;
    }
    else
    {
      auto expr = exprBrowser.getChildValue<ParsedList>(command);
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
  std::string CodeGenerator::ParseToken(const SharedPtr<ParsedItem> &item)
  {
    static ParsedDataBrowser tokenBrowser(STR(
        "Subject.Subject1>0:Subject.Parameter"));
    auto token = tokenBrowser.getValue<ParsedToken>(item);
    if (token == nullptr)
      // TODO: Add the index of the non-token to the exception message.
      THROW_EXCEPTION(InvalidArgumentException,
          "This parsed item doesn't contain a token.");
    return token->getText();
  }

  //----------------------------------------------------------------------------

  ValueTypeSpec *CodeGenerator::ParseVariableType(
      const Core::Basic::SharedPtr<ParsedItem> &item)
  {
    std::string typeName;
    if (item->getProdId() == parameterId)
    {
      auto typeName = item.s_cast<ParsedToken>()->getText();
      return new ValueTypeSpecByName(typeName);
    }
    if (item->getProdId() == subjectId)
    {
      // TODO: Handle the exception potentially thrown by ParseToken.
      typeName = ParseToken(item);
      return new ValueTypeSpecByName(typeName);
    }
    else if (item->getProdId() == functionalExpId)
    {
      // This is a compound type so we need to parse the modifier (ptr, arr, etc.)
      static ParsedDataBrowser modifierBrowser;
      if (!modifierBrowser.isInitialized()) {
        modifierBrowser.initialize(STR("0:Subject.Subject1>0:Subject.Parameter"));
      }
      auto funcName = modifierBrowser.getChildValue<ParsedToken>(item);
      if (funcName == nullptr)
        THROW_EXCEPTION(SyntaxErrorException, "Invalid variable type.");

      else if (funcName->getText() == "ptr")
      {
        // TODO: Re-factor this if block into a separate function.
        // Pointer to a type.
        static ParsedDataBrowser contentTypeBrowser;
        if (!contentTypeBrowser.isInitialized()) {
          contentTypeBrowser.initialize(STR("1:Expression.ParamPassExp>0:Expression.Exp>0"));
        }
        auto typeAstRoot = contentTypeBrowser.getChildValue<ParsedItem>(item);
        if (typeAstRoot == nullptr)
          THROW_EXCEPTION(SyntaxErrorException, "Invalid pointer type.");
        auto contentTypeSpec = ParseVariableType(typeAstRoot);
        return new PointerValueTypeSpec(contentTypeSpec);
      }
      else if (funcName->getText() == "arr")
      {
        // TODO: Re-factor this if block into a separate function.
        // Array of types
        static ParsedDataBrowser elementTypeBrowser(
              STR("1:Expression.ParamPassExp>"
                  "0:Expression.Exp>"
                  "0:Expression.ListExp>"
                  "0"));
        static ParsedDataBrowser arraySizeBrowser(
              STR("1:Expression.ParamPassExp>"
                  "0:Expression.Exp>"
                  "0:Expression.ListExp>"
                  "1:Subject.Subject1>"
                  "0:Subject.Literal"));
        auto elementTypeAst = elementTypeBrowser.getChildValue<ParsedItem>(item);
        if (elementTypeAst == nullptr)
          THROW_EXCEPTION(SyntaxErrorException, "Invalid array type.");
        auto arraySizeAst = arraySizeBrowser.getChildValue<ParsedToken>(item);
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
      const SharedPtr<ParsedItem> &astBlockRoot)
  {
    // Example of an AST block this function parses
    //
    // Expression.LowerLinkExp -- [LIST]:
    //  Subject.Subject1 -- [ROUTE]: 0
    //   Subject.Parameter -- [TOKEN]: IDENTIFIER_TOKEN ("i")
    //  [TOKEN]: CONSTANT_59 (":")
    //  Subject.Subject1 -- [ROUTE]: 0
    //   Subject.Parameter -- [TOKEN]: IDENTIFIER_TOKEN ("float")

    // Finds the name of the variable.
    static ParsedDataBrowser nameBrowser(
        STR("Expression.LowerLinkExp>0:Subject.Subject1"));
    auto nameToken = nameBrowser.getValue<ParsedItem>(astBlockRoot);
    if (nameToken == nullptr) {
      // TODO: Replace the exception with a build message.
      THROW_EXCEPTION(SyntaxErrorException, "Invalid variable definition.");
    }

    // Finds the type of the variable.
    static ParsedDataBrowser typeBrowser(
        STR("Expression.LowerLinkExp>2"));
    auto typeAst = typeBrowser.getValue<ParsedItem>(astBlockRoot);
    if (typeAst == nullptr) {
      // TODO: Replace the exception with a build message.
      THROW_EXCEPTION(SyntaxErrorException, "Invalid variable definition.");
    }

    return VariableDefinition(ParseVariableType(typeAst), ParseToken(nameToken));
  }

  //----------------------------------------------------------------------------

  VariableDefinitionArray CodeGenerator::ParseFunctionArguments(
      const SharedPtr<ParsedItem> &astBlockRoot)
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

    auto id = astBlockRoot->getProdId();

    VariableDefinitionArray args;
    if (id == expressionId) {
      return ParseFunctionArguments(astBlockRoot.s_cast<ParsedList>()
          ->getElement(0).s_cast<ParsedItem>());
    } if (id == subjectId) {
      auto route = astBlockRoot.s_cast<ParsedRoute>();
      if (route->getData() == nullptr)
        return args;
      return ParseFunctionArguments(route->getData().s_cast<ParsedItem>());
    } else if (id == listExpId) {
      auto list = astBlockRoot.s_cast<ParsedList>();
      for (int i = 0, e = (int)list->getElementCount(); i < e; ++i) {
        args.push_back(ParseVariableDefinition(list->getElement(i).s_cast<ParsedItem>()));
      }
    } else if (id == lowerLinkExpId) {
      args.push_back(ParseVariableDefinition(astBlockRoot));
    }
    else
      THROW_EXCEPTION(SyntaxErrorException, "Invalid function argument list.");

    return args;
  }

} // Scg
