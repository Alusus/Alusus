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

// Alusus Core forward declarations.
#include <Basic/SharedPtr.h>
#include <Basic/basic.h>
#include <core_fwd.h>

// Scg CodeGeneration type definitions.
#include <typedefs.h>
#include <Types/ValueTypeSpec.h>

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
    Core::Basic::Word GetStatementListId() { return statementListId; }
    Core::Basic::Word GetExpressionId() { return expressionId; }
    Core::Basic::Word GetListExpId() { return listExpId; }
    Core::Basic::Word GetLowLinkExpId() { return lowLinkExpId; }
    Core::Basic::Word GetLowerLinkExpId() { return lowerLinkExpId; }
    Core::Basic::Word GetLowestLinkExpId() { return lowestLinkExpId; }
    Core::Basic::Word GetFunctionalExpId() { return functionalExpId; }
    Core::Basic::Word GetSubjectId() { return subjectId; }
    Core::Basic::Word GetSubSubjectId() { return subSubjectId; }
    Core::Basic::Word GetParameterId() { return parameterId; }
    Core::Basic::Word GetLiteralId() { return literalId; }
    Core::Basic::Word GetComparisonExpId() { return comparisonExpId; }
    Core::Basic::Word GetAssignmentExpId() { return assignmentExpId; }
    Core::Basic::Word GetAddExpId() { return addExpId; }
    Core::Basic::Word GetMulExpId() { return mulExpId; }
    Core::Basic::Word GetParamPassId() { return paramPassId; }
    Core::Basic::Word GetLinkExpId() { return linkExpId; }
    Core::Basic::Word GetPostfixTildeExpId() { return postfixTildeExpId; }
    Core::Basic::Word GetIdentifierTokenId() { return identifierTokenId; }

    Core::Basic::Word GetDefId() { return defId; }
    Core::Basic::Word GetReturnId() { return returnId; }
    Core::Basic::Word GetIfId() { return ifId; }
    Core::Basic::Word GetForId() { return forId; }
    Core::Basic::Word GetWhileId() { return whileId; }
    Core::Basic::Word GetFunctionId() { return functionId; }
    Core::Basic::Word GetStructureId() { return structureId; }
    Core::Basic::Word GetLinkId() { return linkId; }
    Core::Basic::Word GetCastTildeId() { return castTildeId; }
    Core::Basic::Word GetPointerTildeId() { return pointerTildeId; }
    Core::Basic::Word GetContentTildeId() { return contentTildeId; }
    Core::Basic::Word GetUnaryExpId() { return unaryExpId; }

    void SetBuildMsgStore(Core::Processing::BuildMsgStore *store) { this->buildMsgStore = store; }
    Core::Processing::BuildMsgStore* GetBuildMsgStore() { return this->buildMsgStore; }

    void SetAliasDictionary(Core::Data::SharedMap *dictionary) { this->aliasDictionary = dictionary; }

  private:
    Core::Basic::Word statementListId;
    Core::Basic::Word expressionId;
    Core::Basic::Word listExpId;
    Core::Basic::Word lowLinkExpId;
    Core::Basic::Word lowerLinkExpId;
    Core::Basic::Word lowestLinkExpId;
    Core::Basic::Word functionalExpId;
    Core::Basic::Word subjectId;
    Core::Basic::Word subSubjectId;
    Core::Basic::Word parameterId;
    Core::Basic::Word literalId;
    Core::Basic::Word comparisonExpId;
    Core::Basic::Word assignmentExpId;
    Core::Basic::Word addExpId;
    Core::Basic::Word mulExpId;
    Core::Basic::Word paramPassId;
    Core::Basic::Word linkExpId;
    Core::Basic::Word postfixTildeExpId;
    Core::Basic::Word unaryExpId;
    Core::Basic::Word identifierTokenId;

    Core::Basic::Word defId;
    Core::Basic::Word returnId;
    Core::Basic::Word ifId;
    Core::Basic::Word forId;
    Core::Basic::Word whileId;
    Core::Basic::Word functionId;
    Core::Basic::Word structureId;
    Core::Basic::Word linkId;
    Core::Basic::Word castTildeId;
    Core::Basic::Word pointerTildeId;
    Core::Basic::Word contentTildeId;

    Core::Processing::BuildMsgStore *buildMsgStore = nullptr;

    // TODO: Remove this when proper alias implementation is done.
    Core::Data::SharedMap *aliasDictionary = nullptr;

  public:
    CodeGenerator()
    {
      InitializeIds();
    }
    ~CodeGenerator() {}

  public:
    Module *GenerateModule(const std::string &name, const Core::Basic::SharedPtr<Core::Data::Module> &srcModule);
    Block *GenerateSet(const Core::Basic::SharedPtr<Core::Data::ParsedList> &list);
    Block *GenerateInnerSet(const Core::Basic::SharedPtr<Core::Basic::IdentifiableObject> &item);
    ExpressionArray GenerateStatement(const Core::Basic::SharedPtr<Core::Basic::IdentifiableObject> &item);
    ExpressionArray GenerateDefine(const Core::Basic::SharedPtr<Core::Basic::IdentifiableObject> &item);
    ExpressionArray GenerateDefineVariable(Core::Basic::Char const *name, const Core::Basic::SharedPtr<Core::Basic::IdentifiableObject> &item, bool isAssignment);
    DefineFunction *GenerateDefineFunction(Core::Basic::Char const *name, const Core::Basic::SharedPtr<Core::Basic::IdentifiableObject> &expr);
    DefineStruct *GenerateDefineStructure(Core::Basic::Char const *name, const Core::Basic::SharedPtr<Core::Basic::IdentifiableObject> &expr);

    Expression *GenerateExpression(const Core::Basic::SharedPtr<Core::Basic::IdentifiableObject> &item);
    Return *GenerateReturn(const Core::Basic::SharedPtr<Core::Basic::IdentifiableObject> &ptr);

  private:
    void InitializeIds();
    Expression *GenerateVariableRef(const Core::Basic::SharedPtr<Core::Basic::IdentifiableObject> &param);
    Expression *GenerateConst(const Core::Basic::SharedPtr<Core::Data::ParsedToken> &literal);
    List *GenerateList(const Core::Basic::SharedPtr<Core::Data::ParsedList> &listExpr);
    Expression *GenerateBinaryOperator(const Core::Basic::SharedPtr<Core::Data::ParsedList> &cmpExpr);
    Expression *GenerateUnaryOperator(const Core::Basic::SharedPtr<Core::Data::ParsedList> &cmpExpr);
    IfStatement *GenerateIfStatement(const Core::Basic::SharedPtr<Core::Basic::IdentifiableObject> &command);
    ForStatement *GenerateForStatement(const Core::Basic::SharedPtr<Core::Basic::IdentifiableObject> &command);
    WhileStatement *GenerateWhileStatement(const Core::Basic::SharedPtr<Core::Basic::IdentifiableObject> &command);

  public:
    /**
     * Parses an AST block that represents a token, e.g. variable name,
     * number, etc.
     *
     * @param[in] item  A pointer to the root of the AST block.
     */
    Core::Basic::Char const* ParseToken(const Core::Basic::SharedPtr<Core::Basic::IdentifiableObject> &item);

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
    ValueTypeSpec *ParseVariableType(const Core::Basic::SharedPtr<Core::Basic::IdentifiableObject> &item);

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
    VariableDefinition ParseVariableDefinition(const Core::Basic::SharedPtr<Core::Basic::IdentifiableObject> &astBlockRoot);

    /**
     * Parses a comma-separated list of variable definitions which is used
     * to specify the arguments of a function, where each argument is a
     * variable definition.
     *
     * @param[in] astBlockRoot  A pointer to the root of the AST block to be parsed.
     *
     * @return A VariableDefinitionArray containing the variable definitions.
     */
    VariableDefinitionArray ParseFunctionArguments(const Core::Basic::SharedPtr<Core::Basic::IdentifiableObject> &astBlockRoot);

    // TODO: Remove this when proper alias implementation is done.
    /**
     * @brief Translate the given name if it's found in the alias dictionary.
     * @return Returns the translated string. If no dictionary is found or the
     *         given name is not found the function will return the input as is.
     */
    Core::Basic::Char const* TranslateAliasedName(Core::Basic::Char const *name);

  }; // CodeGenerator
} // Scg

#endif // __CodeGenerator_h__
