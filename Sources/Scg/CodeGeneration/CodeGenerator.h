/**
 * @file Scg/CodeGeneration/CodeGenerator.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __CodeGenerator_h__
#define __CodeGenerator_h__

// Alusus Core header files
#include <core.h>

// Scg CodeGeneration type definitions.
#include <typedefs.h>
#include <Types/ValueTypeSpec.h>

using namespace Core;

namespace Scg
{
  class BinaryOperator;
  class Block;
  class CallFunction;
  class DeclareExtFunction;
  class DefineFunction;
  class DefineStruct;
  class DefineVariable;
  class Expression;
  class ForStatement;
  class IfStatement;
  class List;
  class Module;
  class Return;
  class ValueTypeSpec;
  class WhileStatement;

  class CodeGenerator
  {
  public:
    Word GetStatementListId() { return statementListId; }
    Word GetExpressionId() { return expressionId; }
    Word GetListExpId() { return listExpId; }
    Word GetLowLinkExpId() { return lowLinkExpId; }
    Word GetLowerLinkExpId() { return lowerLinkExpId; }
    Word GetLowestLinkExpId() { return lowestLinkExpId; }
    Word GetFunctionalExpId() { return functionalExpId; }
    Word GetSubjectId() { return subjectId; }
    Word GetSubSubjectId() { return subSubjectId; }
    Word GetParameterId() { return parameterId; }
    Word GetLiteralId() { return literalId; }
    Word GetComparisonExpId() { return comparisonExpId; }
    Word GetAssignmentExpId() { return assignmentExpId; }
    Word GetAddExpId() { return addExpId; }
    Word GetMulExpId() { return mulExpId; }
    Word GetParamPassId() { return paramPassId; }
    Word GetLinkExpId() { return linkExpId; }
    Word GetPostfixTildeExpId() { return postfixTildeExpId; }
    Word GetIdentifierTokenId() { return identifierTokenId; }

    Word GetDefId() { return defId; }
    Word GetReturnId() { return returnId; }
    Word GetIfId() { return ifId; }
    Word GetForId() { return forId; }
    Word GetWhileId() { return whileId; }
    Word GetFunctionId() { return functionId; }
    Word GetStructureId() { return structureId; }
    Word GetLinkId() { return linkId; }
    Word GetPointerTildeId() { return pointerTildeId; }
    Word GetContentTildeId() { return contentTildeId; }

    void SetBuildMsgStore(Core::Processing::BuildMsgStore *store) { this->buildMsgStore = store; }
    Core::Processing::BuildMsgStore* GetBuildMsgStore() { return this->buildMsgStore; }

    void SetAliasDictionary(Core::Data::SharedMap *dictionary) { this->aliasDictionary = dictionary; }

  private:
    Word statementListId;
    Word expressionId;
    Word listExpId;
    Word lowLinkExpId;
    Word lowerLinkExpId;
    Word lowestLinkExpId;
    Word functionalExpId;
    Word subjectId;
    Word subSubjectId;
    Word parameterId;
    Word literalId;
    Word comparisonExpId;
    Word assignmentExpId;
    Word addExpId;
    Word mulExpId;
    Word paramPassId;
    Word linkExpId;
    Word postfixTildeExpId;
    Word identifierTokenId;

    Word defId;
    Word returnId;
    Word ifId;
    Word forId;
    Word whileId;
    Word functionId;
    Word structureId;
    Word linkId;
    Word pointerTildeId;
    Word contentTildeId;

    Core::Processing::BuildMsgStore *buildMsgStore = 0;

    // TODO: Remove this when proper alias implementation is done.
    Core::Data::SharedMap *aliasDictionary = 0;

  public:
    CodeGenerator()
    {
      InitializeIds();
    }
    ~CodeGenerator() {}

  public:
    Module *GenerateModule(const std::string &name, const SharedPtr<Core::Data::Module> &srcModule);
    Block *GenerateSet(const SharedPtr<Core::Data::ParsedList> &list);
    Block *GenerateInnerSet(const SharedPtr<IdentifiableObject> &item);
    Expression *GenerateStatement(const SharedPtr<IdentifiableObject> &item);
    Expression *GenerateDefine(const SharedPtr<IdentifiableObject> &item);
    DefineVariable *GenerateDefineVariable(Char const *name, const SharedPtr<IdentifiableObject> &item);
    DefineFunction *GenerateDefineFunction(Char const *name, const SharedPtr<IdentifiableObject> &expr);
    DefineStruct *GenerateDefineStructure(Char const *name, const SharedPtr<IdentifiableObject> &expr);

    Expression *GenerateExpression(const SharedPtr<IdentifiableObject> &item);
    CallFunction *GeneratePrint(const SharedPtr<IdentifiableObject> &ptr);
    Return *GenerateReturn(const SharedPtr<IdentifiableObject> &ptr);

  private:
    void InitializeIds();
    Expression *GenerateVariableRef(const SharedPtr<IdentifiableObject> &param);
    //Expression *GenerateMemberAccess(SharedPtr<IdentifiableObject> const &param);
    Expression *GenerateConst(const SharedPtr<Core::Data::ParsedToken> &literal);
    List *GenerateList(const SharedPtr<Core::Data::ParsedList> &listExpr);
    Expression *GenerateBinaryOperator(const SharedPtr<Core::Data::ParsedList> &cmpExpr);
    //CallFunction *GenerateFunctionCall(const SharedPtr<Core::Data::ParsedList> &functionalExpr);
    List *GenerateOperand(SharedPtr<IdentifiableObject> const &functionalExpr);
    IfStatement *GenerateIfStatement(const SharedPtr<IdentifiableObject> &command);
    ForStatement *GenerateForStatement(const SharedPtr<IdentifiableObject> &command);
    WhileStatement *GenerateWhileStatement(const SharedPtr<IdentifiableObject> &command);

  public:
    /**
     * Parses an AST block that represents a token, e.g. variable name,
     * number, etc.
     *
     * @param[in] item  A pointer to the root of the AST block.
     */
    Char const* ParseToken(const SharedPtr<IdentifiableObject> &item);

    /**
     * Parses an AST block that is generated for a variable type, e.g. int,
     * float, ptr[int], ptr[ptr[double]], etc.
     *
     * @param[in] item      A pointer to the root of the AST block.
     *
     * @return A pointer to ValueTypeSpec specifying the parsed type. The
     * content of the pointer is allocated in the heap and should be freed by
     * the caller.
     */
    ValueTypeSpec *ParseVariableType(const SharedPtr<IdentifiableObject> &item);

    /**
     * Parses a colon-separated variable definition where the name of the
     * variable is specified before the colon and the type of the variable is
     * after the colon.
     *
     * @param[in] astBlockRoot  A pointer to the root of the AST block to be parsed.
     *
     * @return A VariableDefinition value specifying the name and type of the
     * variable.
     */
    VariableDefinition ParseVariableDefinition(const SharedPtr<IdentifiableObject> &astBlockRoot);

    /**
     * Parses a comma-separated list of variable definitions which is used
     * to specify the arguments of a function, where each argument is a
     * variable definition.
     *
     * @param[in] astBlockRoot  A pointer to the root of the AST block to be parsed.
     *
     * @return A VariableDefinitionArray containing the variable definitions.
     */
    VariableDefinitionArray ParseFunctionArguments(const SharedPtr<IdentifiableObject> &astBlockRoot);

    // TODO: Remove this when proper alias implementation is done.
    /**
     * @brief Translate the given name if it's found in the alias dictionary.
     * @return Returns the translated string. If no dictionary is found or the
     *         given name is not found the function will return the input as is.
     */
    Char const* TranslateAliasedName(Char const *name);

  }; // CodeGenerator
} // Scg

#endif // __CodeGenerator_h__
