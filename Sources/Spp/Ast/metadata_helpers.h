/**
 * @file Spp/Ast/metadata_helpers.h
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_METADATAHELPERS_H
#define SPP_AST_METADATAHELPERS_H

namespace Spp::Ast
{

//==============================================================================
// Global Constants

constexpr Char const* META_EXTRA_AST_TYPE = S("astType");


//==============================================================================
// Global Functions

// tryGetAstType

template <class OT>
inline Type* tryGetAstType(OT *object)
{
  auto box = object->getExtra(META_EXTRA_AST_TYPE).template ti_cast_get<TiBox<Type*>>();
  if (box == 0) return 0;
  else return box->get();
}

// getAstType

template <class OT>
inline Type* getAstType(OT *object)
{
  auto result = tryGetAstType(object);
  if (result == 0) {
    throw EXCEPTION(GenericException, S("Object is missing the AST type."));
  }
  return result;
}

// setAstType

template <class OT>
inline void setAstType(OT *object, SharedPtr<Type> const &type)
{
  object->setExtra(META_EXTRA_AST_TYPE, TiBox<Type*>::create(type.get()));
}

template <class OT>
inline void setAstType(OT *object, Type *type)
{
  object->setExtra(META_EXTRA_AST_TYPE, TiBox<Type*>::create(type));
}

} // namespace

#endif
