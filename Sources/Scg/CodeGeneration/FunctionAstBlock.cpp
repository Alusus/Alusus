/**
 * @file Scg/CodeGeneration/FunctionAstBlock.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <Containers/Block.h>
#include <CodeGeneration/CodeGenerator.h>
#include <CodeGeneration/FunctionAstBlock.h>
#include <CodeGeneration/ListExpression.h>
#include <CodeGeneration/LowLinkExpression.h>
#include <Instructions/DefineFunction.h>

namespace Scg
{
using namespace Core::Standard;
using namespace Core::Basic;
using std::shared_ptr;

FunctionAstBlock::FunctionAstBlock(CodeGenerator *gen,
    const shared_ptr<ParsedList> &astRoot) : autoDelete(false)
{
  if (astRoot->getProdId() != gen->GetFunctionId())
    THROW_EXCEPTION(InvalidArgumentException,
        "Functions can be constructed from Function parsed items only.");

  // Initialises tree browsers.
  static ParsedDataBrowser sigWithRetBrowser( // Signature having return value
      STR("0:Expression.Exp>"
          "0:Expression.LowLinkExp"));
  static ParsedDataBrowser bodyBrowser(
      STR("-1:Main.StatementList"));

  // Parses the arguments and return types of the function.
  auto sigWithRet = sigWithRetBrowser.getChildValue<ParsedList>(astRoot);
  if (sigWithRet != nullptr) {
    // The function has return value.
    auto sigRet = LowLinkExpression(gen, sigWithRet);
    if (sigRet.GetSeparator().compare("=>") != 0)
      THROW_EXCEPTION(SyntaxErrorException, "Invalid separator between the "
          "arguments of the function and its return type. Must use '=>'.");
    this->arguments = gen->ParseFunctionArguments(sigRet.GetLHS());
    this->returnType = gen->ParseVariableType(sigRet.GetRHS());
  } else {
    // The function doesn't have a return value.
    this->arguments = gen->ParseFunctionArguments(
        astRoot->getElement(0).s_cast<ParsedItem>());
  }

  // Parses the body of the function.
  auto bodyRoot = bodyBrowser.getChildValue<ParsedList>(astRoot);
  if (bodyRoot != nullptr)
  {
    auto bodyAstBlock = ListExpression(gen, bodyRoot);
    ExpressionArray bodyStats;
    for (auto i = 0; i < bodyAstBlock.GetItemCount(); i++)
      bodyStats.push_back(gen->GenerateStatement(bodyAstBlock.GetItem(i)));
    this->body = new Block(bodyStats);
  }
  else
    THROW_EXCEPTION(InvalidArgumentException,
        "Function should have a statement list under the path -1:SubSubject.Subject1.");

  // Stores the line and column numbers.
  this->lineInCode = astRoot->getLine();
  this->columnInCode = astRoot->getColumn();
}

//------------------------------------------------------------------------------

FunctionAstBlock::~FunctionAstBlock()
{
  if (this->autoDelete)
    delete this->body;
}

//------------------------------------------------------------------------------

DefineFunction *FunctionAstBlock::ToDefineFunction(const std::string &name)
{
  // We don't own the body anymore, the new DefineFunction instruction does,
  // so we don't delete the body when this object is destroyed.
  this->autoDelete = false;
  // Creates the DefineFunction instruction and sets the line and column numbers.
  auto defFunc = new DefineFunction(name, this->returnType, this->arguments,
                                    this->body);
  defFunc->SetLineInCode(this->lineInCode);
  defFunc->SetColumnInCode(this->columnInCode);
  return defFunc;
}
}
