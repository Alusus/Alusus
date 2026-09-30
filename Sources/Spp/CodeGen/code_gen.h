/**
 * @file Spp/CodeGen/code_gen.h
 * Contains the definitions and include statements of all types in the
 * CodeGen namespace.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
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
  Core::Ast::Node *astNode = 0;
  Ast::Type *astType = 0;
  TioSharedPtr targetData;
};

s_enum(TerminalStatement, UNKNOWN, NO, YES);

s_enum(AstProcessingState,
  NOT_STARTED = 0,
  PROCESSING = 1,
  PROCESSED = 2
);

s_enum(GlobalVarState,
  INITIALIZING = 1,
  INITIALIZED = 2,
  TERMINATING = 4,
  TERMINATED = 8
);


//==============================================================================
// Global Functions

// tryGetCustomData

template <class DT, class OT>
inline DT* tryGetCustomData(OT *object, Char const *name)
{
  return object->getCustomData(name).template ti_cast_get<DT>();
}

// getCustomData

template <class DT, class OT>
inline DT* getCustomData(OT *object, Char const *name)
{
  auto result = tryGetCustomData<DT, OT>(object, name);
  if (result == 0) {
    throw EXCEPTION(GenericException, S("Object is missing the generated data."));
  }
  return result;
}

// Ast Related Accessors

#define DEFINE_FLAG_ACCESSORS(name) \
  template <class OT> inline Bool is##name(OT *object) { \
    auto f = tryGetCustomData<TiBool>(object, #name); return f && f->get(); \
  } \
  template <class OT> inline void set##name(OT *object, Bool f) { \
    object->setCustomData(#name, TiBool::create(f)); \
  } \
  template <class OT> inline void reset##name(OT *object) { object->removeCustomData(#name); }

#define DEFINE_STR_ACCESSORS(name) \
  template <class OT> inline void set##name(OT *object, Str f) { \
    object->setCustomData(#name, TiStr::create(f)); \
  } \
  template <class OT> inline Str get##name(OT *object) { \
    auto s = tryGetCustomData<TiStr>(object, #name); return s != 0 ? s->getStr() : Str(); \
  } \
  template <class OT> inline void reset##name(OT *object) { object->removeCustomData(#name); }

DEFINE_FLAG_ACCESSORS(Executed);
DEFINE_STR_ACCESSORS(MangledName);

// Ast Processing State
template <class OT> inline Int getAstProcessingState(OT *object) {
  auto f = tryGetCustomData<TiInt>(object, "AstProcessing");
  return f ? f->get() : AstProcessingState::NOT_STARTED;
}
template <class OT> inline void setAstProcessingState(OT *object, Int s) {
  object->setCustomData("AstProcessing", TiInt::create(s));
}

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
#include "CustomDataAccessor.h"
#include "DestructionNode.h"
#include "DestructionStack.h"
#include "DependencyList.h"
#include "DependencyInfo.h"
#include "GlobalCtorDtorInfo.h"
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
