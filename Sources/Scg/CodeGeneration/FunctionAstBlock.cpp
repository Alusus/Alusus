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
using namespace Core::Basic;
using namespace Core::Data;
using std::shared_ptr;

FunctionAstBlock::FunctionAstBlock(CodeGenerator *gen,
    const shared_ptr<ParsedList> &astRoot) : autoDelete(false)
{
  if (astRoot->getProdId() != gen->GetFunctionId())
    throw EXCEPTION(InvalidArgumentException,
        "Functions can be constructed from Function parsed items only.");

  // Initialises tree browsers.
  static ReferenceSeeker seeker;
  // Signature having return value
  static SharedPtr<Reference> sigWithRetReference = ReferenceParser::parseQualifier(
    STR("1~where(prodId=Expression.Exp)."
        "0~where(prodId=Expression.LowLinkExp)"),
    ReferenceUsageCriteria::MULTI_DATA);
  static SharedPtr<Reference> bodyReference = ReferenceParser::parseQualifier(
    STR("(-1)~where(prodId=Main.StatementList)"),
    ReferenceUsageCriteria::MULTI_DATA);

  // Parses the arguments and return types of the function.
  auto sigWithRet = getSharedPtr(seeker.tryGet(sigWithRetReference.get(), astRoot.get())).io_cast<ParsedList>();
  if (sigWithRet != nullptr) {
    // The function has return value.
    auto sigRet = LowLinkExpression(gen, sigWithRet);
    if (sigRet.GetSeparator().compare("=>") != 0)
      throw EXCEPTION(SyntaxErrorException, "Invalid separator between the "
          "arguments of the function and its return type. Must use '=>'.");
    this->arguments = gen->ParseFunctionArguments(sigRet.GetLHS());
    this->returnType = gen->ParseVariableType(sigRet.GetRHS());
  } else {
    // The function doesn't have a return value.
    this->arguments = gen->ParseFunctionArguments(
        astRoot->getShared(1));
  }

  // Parses the body of the function.
  auto bodyRoot = getSharedPtr(seeker.tryGet(bodyReference.get(), astRoot.get())).io_cast<ParsedList>();
  if (bodyRoot != nullptr)
  {
    auto bodyAstBlock = ListExpression(gen, bodyRoot);
    ExpressionArray bodyStats;
    for (auto i = 0; i < bodyAstBlock.GetItemCount(); i++)
      bodyStats.push_back(gen->GenerateStatement(bodyAstBlock.GetItem(i)));
    this->body = new Block(bodyStats);
  }
  else
    throw EXCEPTION(InvalidArgumentException,
        "Function should have a statement list under the path -1:SubSubject.Subject1.");

  // Stores the line and column numbers.
  this->sourceLocation = astRoot->getSourceLocation();
}

//------------------------------------------------------------------------------

FunctionAstBlock::~FunctionAstBlock()
{
  if (this->autoDelete)
    delete this->body;
}

//------------------------------------------------------------------------------

DefineFunction *FunctionAstBlock::ToDefineFunction(Char const *name)
{
  // We don't own the body anymore, the new DefineFunction instruction does,
  // so we don't delete the body when this object is destroyed.
  this->autoDelete = false;
  // Creates the DefineFunction instruction and sets the line and column numbers.
  auto defFunc = new DefineFunction(name, this->returnType, this->arguments,
                                    this->body);
  defFunc->setSourceLocation(this->sourceLocation);
  return defFunc;
}
}
