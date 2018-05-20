/**
 * @file Spp/CodeGen/code_gen.h
 * Contains the definitions and include statements of all types in the
 * CodeGen namespace.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_CODEGEN_H
#define SPP_CODEGEN_CODEGEN_H

namespace Spp { namespace CodeGen
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


//==============================================================================
// Global Constants

constexpr Char const* META_EXTRA_CODE_GEN = STR("codeGen");


//==============================================================================
// Global Functions

// tryGetCodeGenData

template <class DT, class OT,
          typename std::enable_if<std::is_base_of<Core::Data::Ast::Metadata, OT>::value, int>::type = 0>
inline DT* tryGetCodeGenData(OT *object)
{
  return object->getExtra(META_EXTRA_CODE_GEN).template ti_cast_get<DT>();
}

template <class DT, class OT,
          typename std::enable_if<!std::is_base_of<Core::Data::Ast::Metadata, OT>::value, int>::type = 0>
inline DT* tryGetCodeGenData(OT *object)
{
  auto metadata = ti_cast<Core::Data::Ast::Metadata>(object);
  if (metadata == 0) return 0;
  return metadata->getExtra(META_EXTRA_CODE_GEN).template ti_cast_get<DT>();
}

// getCodeGenData

template <class DT, class OT>
inline DT* getCodeGenData(OT *object)
{
  auto result = tryGetCodeGenData<DT>(object);
  if (result == 0) {
    throw EXCEPTION(GenericException, STR("Object is missing the generated data."));
  }
  return result;
}

// setCodeGenData

template <class DT, class OT,
          typename std::enable_if<std::is_base_of<Core::Data::Ast::Metadata, OT>::value, int>::type = 0>
inline void setCodeGenData(OT *object, Core::Basic::SharedPtr<DT> const &data)
{
  object->setExtra(META_EXTRA_CODE_GEN, data);
}

template <class DT, class OT,
          typename std::enable_if<!std::is_base_of<Core::Data::Ast::Metadata, OT>::value, int>::type = 0>
inline void setCodeGenData(OT *object, Core::Basic::SharedPtr<DT> const &data)
{
  auto metadata = ti_cast<Core::Data::Ast::Metadata>(object);
  if (metadata == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("object"), STR("Object does not implement the Metadata interface."));
  }
  metadata->setExtra(META_EXTRA_CODE_GEN, data);
}

} } // namespace


//==============================================================================
// Type Names

DEFINE_TYPE_NAME(Spp::CodeGen::GenResult, "alusus.net/Spp/Spp.CodeGen.GenResult");


//==============================================================================
// Classes

namespace Spp { namespace CodeGen
{

class Generator;

} }

// Data
#include "IfTgContext.h"
#include "LoopTgContext.h"

// Interfaces
#include "TargetGeneration.h"
#include "Generation.h"

// The Generator
#include "NoOpTargetGenerator.h"
#include "TypeGenerator.h"
#include "ExpressionGenerator.h"
#include "CommandGenerator.h"
#include "Generator.h"

#endif
