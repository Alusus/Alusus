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
  using namespace Core::Standard;
  using namespace Core::Basic;

  FunctionLinkExpression::FunctionLinkExpression(CodeGenerator *gen,
      const SharedPtr<ParsedItem> &astRoot)
  {
    if (astRoot->getProdId() != gen->GetLinkId())
      THROW_EXCEPTION(SystemException,
          "Function link expressions can be constructed from Main.Link only.");

    // Initialise tree browsers.
    static ParsedDataBrowser funcExpNoRetBrowser(
        STR("0:Expression.Exp>"
            "0:Expression.FunctionalExp"));
    static ParsedDataBrowser funcExpBrowser(
        STR("0:Expression.Exp>"
            "0:Expression.LowLinkExp"));

    // Try to parse a function link with no return value.
    auto item = funcExpBrowser.getChildValue<ParsedList>(astRoot);
    if ((item = funcExpBrowser.getChildValue<ParsedList>(astRoot)) != nullptr)
    {
      auto argsAndRet = LowLinkExpression(gen, item);
      if (argsAndRet.GetSeparator().compare("=>") != 0)
        THROW_EXCEPTION(SyntaxErrorException, "Invalid separator between the "
            "argument types and return value of a function link. Must use '=>'.");
      this->arguments = new FunctionalExpression(
            gen, argsAndRet.GetLHS().s_cast<ParsedList>());
      this->retType = gen->ParseVariableType(argsAndRet.GetRHS().s_cast<ParsedItem>());
    }
    else if ((item = funcExpNoRetBrowser.getChildValue<ParsedList>(astRoot)) != nullptr)
      this->arguments = new FunctionalExpression(gen, item.s_cast<ParsedList>());

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
