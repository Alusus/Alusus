/**
 * @file Spp/CodeGen/code_gen.h
 * Contains the definitions and include statements of all types in the
 * CodeGen namespace.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_CODEGEN_H
#define SPP_CODEGEN_CODEGEN_H

namespace Spp::CodeGen
{

/**
 * @defgroup spp_codegen Code Generation
 * @ingroup spp
 * @brief Classes for code generation.
 */


//==============================================================================
// Types

class TargetGeneration;
class DestructionStack;

struct GenDeps
{
  TargetGeneration *tg;
  TiObject *tgContext;
  DestructionStack *destructionStack;
  TiObject *tgGlobalConstructionContext;
  DestructionStack *globalDestructionStack;
  TiObject *tgSelf;
  Ast::Type *astSelfType;

  GenDeps(
    TargetGeneration *tg, TiObject *tgc, DestructionStack *ds, TiObject *tgGcc, DestructionStack *gds
  ) : tg(tg), tgContext(tgc), destructionStack(ds)
    , tgGlobalConstructionContext(tgGcc), globalDestructionStack(gds)
    , tgSelf(0), astSelfType(0)
  {}

  GenDeps(GenDeps const &deps, TiObject *tgc)
    : tg(deps.tg), tgContext(tgc), destructionStack(deps.destructionStack)
    , tgGlobalConstructionContext(deps.tgGlobalConstructionContext), globalDestructionStack(deps.globalDestructionStack)
    , tgSelf(deps.tgSelf), astSelfType(deps.astSelfType)
  {}

  GenDeps(GenDeps const &deps, TiObject *tgc, DestructionStack *ds)
    : tg(deps.tg), tgContext(tgc), destructionStack(ds)
    , tgGlobalConstructionContext(deps.tgGlobalConstructionContext), globalDestructionStack(deps.globalDestructionStack)
    , tgSelf(deps.tgSelf), astSelfType(deps.astSelfType)
  {}

  GenDeps(GenDeps const &deps, TiObject *tgc, DestructionStack *ds, TiObject *tgs, Ast::Type *astst)
    : tg(deps.tg), tgContext(tgc), destructionStack(ds)
    , tgGlobalConstructionContext(deps.tgGlobalConstructionContext), globalDestructionStack(deps.globalDestructionStack)
    , tgSelf(tgs), astSelfType(astst)
  {}
};

struct GenResult
{
  TiObject *astNode = 0;
  Ast::Type *astType = 0;
  TioSharedPtr targetData;
};

s_enum(TerminalStatement, UNKNOWN, NO, YES);


//==============================================================================
// Global Constants

constexpr Char const* META_EXTRA_CODE_GEN_FAILED = S("codeGenFailed");
constexpr Char const* META_EXTRA_INIT_STMT_GEN_INDEX = S("initStmtGenIndex");


//==============================================================================
// Global Functions

// tryGetExtra

template <class DT, class OT,
          typename std::enable_if<std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline DT* tryGetExtra(OT *object, Char const *name)
{
  return object->getExtra(name).template ti_cast_get<DT>();
}

template <class DT, class OT,
          typename std::enable_if<!std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline DT* tryGetExtra(OT *object, Char const *name)
{
  auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(object);
  if (metadata == 0) return 0;
  return metadata->getExtra(name).template ti_cast_get<DT>();
}

// getExtra

template <class DT, class OT>
inline DT* getExtra(OT *object, Char const *name)
{
  auto result = tryGetExtra<DT, OT>(object, name);
  if (result == 0) {
    throw EXCEPTION(GenericException, S("Object is missing the generated data."));
  }
  return result;
}

// setExtra

template <class DT, class OT,
          typename std::enable_if<std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline void setExtra(OT *object, Char const *name, SharedPtr<DT> const &data)
{
  object->setExtra(name, data);
}

template <class DT, class OT,
          typename std::enable_if<!std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline void setExtra(OT *object, Char const *name, SharedPtr<DT> const &data)
{
  auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(object);
  if (metadata == 0) {
    throw EXCEPTION(InvalidArgumentException, S("object"), S("Object does not implement the MetaHaving interface."));
  }
  metadata->setExtra(name, data);
}

// removeExtra

template <class OT,
          typename std::enable_if<std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline void removeExtra(OT *object, Char const *name)
{
  object->removeExtra(name);
}

template <class OT,
          typename std::enable_if<!std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline void removeExtra(OT *object, Char const *name)
{
  auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(object);
  if (metadata == 0) {
    throw EXCEPTION(InvalidArgumentException, S("object"), S("Object does not implement the MetaHaving interface."));
  }
  metadata->removeExtra(name);
}

#define DEFINE_EXTRA_ACCESSORS(name) \
  template <class DT, class OT> inline DT* tryGet##name(OT *object) { return tryGetExtra<DT>(object, #name); } \
  template <class DT, class OT> inline DT* get##name(OT *object) { return getExtra<DT>(object, #name); } \
  template <class DT, class OT> inline void set##name(OT *object, SharedPtr<DT> const &data) { \
    setExtra(object, #name, data); \
  } \
  template <class OT> inline void remove##name(OT *object) { removeExtra(object, #name); }

DEFINE_EXTRA_ACCESSORS(CodeGenData);
DEFINE_EXTRA_ACCESSORS(AutoCtor);
DEFINE_EXTRA_ACCESSORS(AutoDtor);

// didCodeGenFail

template <class OT, typename std::enable_if<std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline Bool didCodeGenFail(OT *object)
{
  auto f = object->getExtra(META_EXTRA_CODE_GEN_FAILED).template ti_cast_get<TiBool>();
  return f && f->get();
}

template <class OT, typename std::enable_if<!std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline Bool didCodeGenFail(OT *object)
{
  auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(object);
  if (metadata == 0) return false;
  auto f = metadata->getExtra(META_EXTRA_CODE_GEN_FAILED).template ti_cast_get<TiBool>();
  return f && f->get();
}

// setCodeGenFailed

template <class OT, typename std::enable_if<std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline void setCodeGenFailed(OT *object, Bool f)
{
  object->setExtra(META_EXTRA_CODE_GEN_FAILED, TiBool::create(f));
}

template <class OT, typename std::enable_if<!std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline void setCodeGenFailed(OT *object, Bool f)
{
  auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(object);
  if (metadata == 0) {
    throw EXCEPTION(InvalidArgumentException, S("object"), S("Object does not implement the MetaHaving interface."));
  }
  metadata->setExtra(META_EXTRA_CODE_GEN_FAILED, TiBool::create(f));
}

// resetCodeGenFailed

template <class OT, typename std::enable_if<std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline void resetCodeGenFailed(OT *object)
{
  object->removeExtra(META_EXTRA_CODE_GEN_FAILED);
}

template <class OT, typename std::enable_if<!std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline void resetCodeGenFailed(OT *object)
{
  auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(object);
  if (metadata == 0) {
    throw EXCEPTION(InvalidArgumentException, S("object"), S("Object does not implement the MetaHaving interface."));
  }
  metadata->removeExtra(META_EXTRA_CODE_GEN_FAILED);
}

// getInitStatementsGenIndex

template <class OT, typename std::enable_if<std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline Int getInitStatementsGenIndex(OT *object)
{
  auto i = object->getExtra(META_EXTRA_INIT_STMT_GEN_INDEX).template ti_cast_get<TiInt>();
  return i == 0 ? 0 : i->get();
}

template <class OT, typename std::enable_if<!std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline Int getInitStatementsGenIndex(OT *object)
{
  auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(object);
  if (metadata == 0) return false;
  auto i = metadata->getExtra(META_EXTRA_INIT_STMT_GEN_INDEX).template ti_cast_get<TiInt>();
  return i == 0 ? 0 : i->get();
}

// setInitStatementsGenIndex

template <class OT, typename std::enable_if<std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline void setInitStatementsGenIndex(OT *object, Int i)
{
  auto index = object->getExtra(META_EXTRA_INIT_STMT_GEN_INDEX).template ti_cast_get<TiInt>();
  if (index == 0) {
    object->setExtra(META_EXTRA_INIT_STMT_GEN_INDEX, TiInt::create(i));
  } else {
    index->set(i);
  }
}

template <class OT, typename std::enable_if<!std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline void setInitStatementsGenIndex(OT *object, Int i)
{
  auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(object);
  if (metadata == 0) {
    throw EXCEPTION(InvalidArgumentException, S("object"), S("Object does not implement the MetaHaving interface."));
  }
  auto index = metadata->getExtra(META_EXTRA_INIT_STMT_GEN_INDEX).template ti_cast_get<TiInt>();
  if (index == 0) {
    metadata->setExtra(META_EXTRA_INIT_STMT_GEN_INDEX, TiInt::create(i));
  } else {
    index->set(i);
  }
}

// resetInitStatementsGenIndex

template <class OT, typename std::enable_if<std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline void resetInitStatementsGenIndex(OT *object)
{
  object->removeExtra(META_EXTRA_INIT_STMT_GEN_INDEX);
}

template <class OT, typename std::enable_if<!std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline void resetInitStatementsGenIndex(OT *object)
{
  auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(object);
  if (metadata == 0) {
    throw EXCEPTION(InvalidArgumentException, S("object"), S("Object does not implement the MetaHaving interface."));
  }
  metadata->removeExtra(META_EXTRA_INIT_STMT_GEN_INDEX);
}

} // namespace


//==============================================================================
// Type Names

DEFINE_TYPE_NAME(Spp::CodeGen::GenDeps, "alusus.org/Spp/Spp.CodeGen.GenDeps");
DEFINE_TYPE_NAME(Spp::CodeGen::GenResult, "alusus.org/Spp/Spp.CodeGen.GenResult");
DEFINE_TYPE_NAME(Spp::CodeGen::TerminalStatement, "alusus.org/Spp/Spp.CodeGen.TerminalStatement");


//==============================================================================
// Classes

namespace Spp::CodeGen
{

class Generator;

}

// Helpers
#include "DestructionStack.h"

// Data
#include "IfTgContext.h"
#include "LoopTgContext.h"
#include "GlobalItemRepo.h"

// Interfaces
#include "TargetGeneration.h"
#include "Generation.h"

// Preprocessing
#include "AstProcessor.h"

// The Generator
#include "TypeGenerator.h"
#include "ExpressionGenerator.h"
#include "CommandGenerator.h"
#include "Generator.h"

#endif
