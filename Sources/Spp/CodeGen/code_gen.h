/**
 * @file Spp/CodeGen/code_gen.h
 * Contains the definitions and include statements of all types in the
 * CodeGen namespace.
 *
 * @copyright Copyright (C) 2023 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
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

struct GenResult
{
  TiObject *astNode = 0;
  Ast::Type *astType = 0;
  TioSharedPtr targetData;
};

s_enum(TerminalStatement, UNKNOWN, NO, YES);


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

// Ast Related Accessors

#define DEFINE_FLAG_ACCESSORS(name) \
  template <class OT> inline Bool is##name(OT *object) { \
    auto f = tryGetExtra<TiBool>(object, #name); return f && f->get(); \
  } \
  template <class OT> inline void set##name(OT *object, Bool f) { \
    setExtra(object, #name, TiBool::create(f)); \
  } \
  template <class OT> inline void reset##name(OT *object) { removeExtra(object, #name); }

DEFINE_FLAG_ACCESSORS(AstProcessed);
DEFINE_FLAG_ACCESSORS(Executed);

} // namespace


//==============================================================================
// Type Names

DEFINE_TYPE_NAME(Spp::CodeGen::GenResult, "alusus.org/Spp/Spp.CodeGen.GenResult");
DEFINE_TYPE_NAME(Spp::CodeGen::TerminalStatement, "alusus.org/Spp/Spp.CodeGen.TerminalStatement");


//==============================================================================
// Classes

namespace Spp::CodeGen
{

class Generator;
class TargetGeneration;

}

// Helpers
#include "ExtraDataAccessor.h"
#include "DestructionNode.h"
#include "DestructionStack.h"
#include "Session.h"

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
