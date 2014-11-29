/**
 * @file Scg/CodeGeneration/FunctionLinkExpression.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <CodeGeneration/CodeGenerator.h>
#include <CodeGeneration/FunctionLinkExpression.h>
#include <CodeGeneration/FunctionalExpression.h>
#include <CodeGeneration/LowLinkExpression.h>
#include <Instructions/DeclareExtFunction.h>

namespace Scg
{
  using namespace Core::Basic;
  using namespace Core::Data;

  FunctionLinkExpression::FunctionLinkExpression(CodeGenerator *gen,
      const SharedPtr<IdentifiableObject> &astRoot)
  {
    auto astRootMeta = astRoot->getInterface<ParsingMetadataHolder>();
    if (astRootMeta == nullptr || astRootMeta->getProdId() != gen->GetLinkId())
      THROW_EXCEPTION(SystemException,
          "Function link expressions can be constructed from Main.Link only.");

    // Initialise tree browsers.
    static ReferenceSeeker seeker;
    static SharedPtr<Reference> funcExpNoRetReference = ReferenceParser::parseQualifier(
      STR("0~where(prodId=Expression.Exp)."
          "0~where(prodId=Expression.FunctionalExp)"),
      ReferenceUsageCriteria::MULTI_DATA);
    static SharedPtr<Reference> funcExpReference = ReferenceParser::parseQualifier(
      STR("0~where(prodId=Expression.Exp)."
          "0~where(prodId=Expression.LowLinkExp)"),
      ReferenceUsageCriteria::MULTI_DATA);

    // Try to parse a function link with no return value.
    SharedPtr<ParsedList> item;
    if ((item = getSharedPtr(seeker.tryGet(funcExpReference.get(), astRoot.get())).io_cast<ParsedList>()) != nullptr)
    {
      auto argsAndRet = LowLinkExpression(gen, item);
      if (argsAndRet.GetSeparator().compare("=>") != 0)
        THROW_EXCEPTION(SyntaxErrorException, "Invalid separator between the "
            "argument types and return value of a function link. Must use '=>'.");
      this->arguments = new FunctionalExpression(
            gen, argsAndRet.GetLHS().s_cast<ParsedList>());
      this->retType = gen->ParseVariableType(argsAndRet.GetRHS());
    }
    else if ((item = getSharedPtr(seeker.tryGet(funcExpNoRetReference.get(), astRoot.get())).io_cast<ParsedList>()) != nullptr)
      this->arguments = new FunctionalExpression(gen, item);

    // Stores the line and column numbers.
    this->lineInCode = item->getLine();
    this->columnInCode = item->getColumn();
  }

  //----------------------------------------------------------------------------

  FunctionLinkExpression::~FunctionLinkExpression()
  {
    if (this->arguments != 0) delete this->arguments;
  }

  //----------------------------------------------------------------------------

  DeclareExtFunction *FunctionLinkExpression::ToDeclareExtFunction()
  {
    auto declExtFunc = this->arguments->ToDeclareExtFunction(this->retType);
    declExtFunc->SetLineInCode(this->lineInCode);
    declExtFunc->SetColumnInCode(this->columnInCode);
    return declExtFunc;
  }
}
