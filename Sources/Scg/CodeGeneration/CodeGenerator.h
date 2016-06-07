/**
 * @file Scg/CodeGeneration/CodeGenerator.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_CODEGENERATOR_H
#define SCG_CODEGENERATOR_H

// Alusus Core forward declarations.
#include "core.h"

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
class AstNode;
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
  Core::Basic::Word getStatementListId()
  {
    return statementListId;
  }
  Core::Basic::Word getExpressionId()
  {
    return expressionId;
  }
  Core::Basic::Word getListExpId()
  {
    return listExpId;
  }
  Core::Basic::Word getLowLinkExpId()
  {
    return lowLinkExpId;
  }
  Core::Basic::Word getLowerLinkExpId()
  {
    return lowerLinkExpId;
  }
  Core::Basic::Word getLowestLinkExpId()
  {
    return lowestLinkExpId;
  }
  Core::Basic::Word getFunctionalExpId()
  {
    return functionalExpId;
  }
  Core::Basic::Word getSubjectId()
  {
    return subjectId;
  }
  Core::Basic::Word getSubSubjectId()
  {
    return subSubjectId;
  }
  Core::Basic::Word getParameterId()
  {
    return parameterId;
  }
  Core::Basic::Word getLiteralId()
  {
    return literalId;
  }
  Core::Basic::Word getComparisonExpId()
  {
    return comparisonExpId;
  }
  Core::Basic::Word getAssignmentExpId()
  {
    return assignmentExpId;
  }
  Core::Basic::Word getAddExpId()
  {
    return addExpId;
  }
  Core::Basic::Word getMulExpId()
  {
    return mulExpId;
  }
  Core::Basic::Word getParamPassId()
  {
    return paramPassId;
  }
  Core::Basic::Word getLinkExpId()
  {
    return linkExpId;
  }
  Core::Basic::Word getPostfixTildeExpId()
  {
    return postfixTildeExpId;
  }
  Core::Basic::Word getIdentifierTokenId()
  {
    return identifierTokenId;
  }

  Core::Basic::Word getDefId()
  {
    return defId;
  }
  Core::Basic::Word getReturnId()
  {
    return returnId;
  }
  Core::Basic::Word getIfId()
  {
    return ifId;
  }
  Core::Basic::Word getForId()
  {
    return forId;
  }
  Core::Basic::Word getWhileId()
  {
    return whileId;
  }
  Core::Basic::Word getFunctionId()
  {
    return functionId;
  }
  Core::Basic::Word getStructureId()
  {
    return structureId;
  }
  Core::Basic::Word getLinkId()
  {
    return linkId;
  }
  Core::Basic::Word getCastTildeId()
  {
    return castTildeId;
  }
  Core::Basic::Word getPointerTildeId()
  {
    return pointerTildeId;
  }
  Core::Basic::Word getContentTildeId()
  {
    return contentTildeId;
  }
  Core::Basic::Word getSizeTildeId()
  {
    return sizeTildeId;
  }
  Core::Basic::Word getUnaryExpId()
  {
    return unaryExpId;
  }

  void setBuildMsgStore(Core::Processing::BuildMsgStore *store)
  {
    this->buildMsgStore = store;
  }
  Core::Processing::BuildMsgStore* getBuildMsgStore()
  {
    return this->buildMsgStore;
  }

  void setAliasDictionary(Core::Data::SharedMap *dictionary)
  {
    this->aliasDictionary = dictionary;
  }

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
  Core::Basic::Word sizeTildeId;

  Core::Processing::BuildMsgStore *buildMsgStore = nullptr;

  // TODO: Remove this when proper alias implementation is done.
  Core::Data::SharedMap *aliasDictionary = nullptr;

public:
  CodeGenerator()
  {
    initializeIds();
  }
  ~CodeGenerator() {}

public:
  SharedPtr<Block> generateSet(const Core::Basic::SharedPtr<Core::Data::Ast::List> &list);
  SharedPtr<Block> generateInnerSet(const Core::Basic::SharedPtr<Core::Basic::TiObject> &item);
  AstNodeSharedArray generateStatement(const Core::Basic::SharedPtr<Core::Basic::TiObject> &item);
  AstNodeSharedArray generateDefine(const Core::Basic::SharedPtr<Core::Basic::TiObject> &item);
  AstNodeSharedArray generateDefineVariable(Core::Basic::Char const *name,
                                      const Core::Basic::SharedPtr<Core::Basic::TiObject> &item,
                                      bool isAssignment);
  SharedPtr<DefineFunction> generateDefineFunction(Core::Basic::Char const *name,
                                                   Core::Basic::SharedPtr<Core::Basic::TiObject> const &expr);
  SharedPtr<DefineStruct> generateDefineStructure(Core::Basic::Char const *name,
                                                  Core::Basic::SharedPtr<Core::Basic::TiObject> const &expr);

  SharedPtr<AstNode> generateExpression(const Core::Basic::SharedPtr<Core::Basic::TiObject> &item);
  SharedPtr<Return> generateReturn(const Core::Basic::SharedPtr<Core::Basic::TiObject> &ptr);

private:
  void initializeIds();
  SharedPtr<AstNode> generateVariableRef(const Core::Basic::SharedPtr<Core::Basic::TiObject> &param);
  SharedPtr<AstNode> generateConst(const Core::Basic::SharedPtr<Core::Data::Ast::Token> &literal);
  SharedPtr<List> generateList(const Core::Basic::SharedPtr<Core::Data::Ast::List> &listExpr);
  SharedPtr<AstNode> generateBinaryOperator(const Core::Basic::SharedPtr<Core::Data::Ast::List> &cmpExpr);
  SharedPtr<AstNode> generateUnaryOperator(const Core::Basic::SharedPtr<Core::Data::Ast::List> &cmpExpr);
  SharedPtr<IfStatement> generateIfStatement(const Core::Basic::SharedPtr<Core::Basic::TiObject> &command);
  SharedPtr<ForStatement> generateForStatement(const Core::Basic::SharedPtr<Core::Basic::TiObject> &command);
  SharedPtr<WhileStatement> generateWhileStatement(const Core::Basic::SharedPtr<Core::Basic::TiObject> &command);

public:
  /**
   * Parses an AST block that represents a token, e.g. variable name,
   * number, etc.
   *
   * @param[in] item  A pointer to the root of the AST block.
   */
  Core::Basic::Char const* parseToken(SharedPtr<Core::Basic::TiObject> const &item);

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
  SharedPtr<ValueTypeSpec> parseVariableType(SharedPtr<Core::Basic::TiObject> const &item);

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
  VariableDefinition parseVariableDefinition(SharedPtr<Core::Basic::TiObject> const &astBlockRoot);

  /**
   * Parses a comma-separated list of variable definitions which is used
   * to specify the arguments of a function, where each argument is a
   * variable definition.
   *
   * @param[in] astBlockRoot  A pointer to the root of the AST block to be parsed.
   *
   * @return A VariableDefinitionArray containing the variable definitions.
   */
  VariableDefinitionArray parseFunctionArguments(SharedPtr<Core::Basic::TiObject> const &astBlockRoot);

  // TODO: Remove this when proper alias implementation is done.
  /**
   * @brief Translate the given name if it's found in the alias dictionary.
   * @return Returns the translated string. If no dictionary is found or the
   *         given name is not found the function will return the input as is.
   */
  Core::Basic::Char const* translateAliasedName(Core::Basic::Char const *name);

}; // class

} // namespace

#endif
