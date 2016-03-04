/**
 * @file Scg/CodeGeneration/FunctionAstBlock.cpp
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
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

FunctionAstBlock::FunctionAstBlock(CodeGenerator *gen, SharedPtr<PrtList> const &astRoot)
{
  if (astRoot->getProdId() != gen->getFunctionId())
    throw EXCEPTION(InvalidArgumentException,
                    "Functions can be constructed from Function parsed items only.");

  // Initialises tree browsers.
  static ReferenceSeeker seeker;
  // Signature having return value
  static SharedPtr<Reference> sigWithRetReference = REF_PARSER->parseQualifier(
        STR("1~where(prodId=Expression.Exp)."
            "0~where(prodId=Expression.LowLinkExp)"),
        ReferenceUsageCriteria::MULTI_DATA);
  static SharedPtr<Reference> bodyReference = REF_PARSER->parseQualifier(
        STR("(-1)~where(prodId=Main.StatementList)"),
        ReferenceUsageCriteria::MULTI_DATA);

  // Parses the arguments and return types of the function.
  auto sigWithRet = getSharedPtr(seeker.tryGet(sigWithRetReference.get(), astRoot.get())).io_cast<PrtList>();

  if (sigWithRet != nullptr) {
    // The function has return value.
    auto sigRet = LowLinkExpression(gen, sigWithRet);

    if (sigRet.getSeparator().compare("=>") != 0) {
      gen->getBuildMsgStore()->add(std::make_shared<Core::Processing::CustomBuildMsg>(
                                     STR("Invalid separator between the arguments of the function"
                                         " and its return type. Must use '=>'"),
                                     STR("SCG1022"), 1, astRoot->getSourceLocation(),
                                     sigRet.getSeparator().c_str()));
    }

    this->arguments = gen->parseFunctionArguments(sigRet.getLHS());
    this->returnType = gen->parseVariableType(sigRet.getRHS());
  } else {
    // The function doesn't have a return value.
    this->arguments = gen->parseFunctionArguments(
                        astRoot->getShared(1));
  }

  // Parses the body of the function.
  auto bodyRoot = getSharedPtr(seeker.tryGet(bodyReference.get(), astRoot.get())).io_cast<PrtList>();

  if (bodyRoot != nullptr) {
    auto bodyAstBlock = ListExpression(gen, bodyRoot);
    this->body = std::make_shared<Block>();

    for (auto i = 0; i < bodyAstBlock.getItemCount(); i++)
      for (auto stat : gen->generateStatement(bodyAstBlock.getItem(i))) {
        this->body->appendNode(stat);
      }
  } else
    throw EXCEPTION(InvalidArgumentException,
                    "Function should have a statement list under the path -1:SubSubject.Subject1.");

  // Stores the line and column numbers.
  this->sourceLocation = astRoot->getSourceLocation();
}


SharedPtr<DefineFunction> FunctionAstBlock::toDefineFunction(Char const *name)
{
  // Creates the DefineFunction instruction and sets the line and column numbers.
  auto defFunc = std::make_shared<DefineFunction>(name, this->returnType, this->arguments, this->body);
  defFunc->setSourceLocation(this->sourceLocation);
  return defFunc;
}

} // namespace
