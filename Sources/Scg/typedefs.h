/**
 * @file Scg/typedefs.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __typedefs_h__
#define __typedefs_h__

// STL header files
#include <map>
#include <set>
#include <string>
#include <vector>
#include <sstream>

// LLVM forward declarations
#include <llvm_fwd.h>

namespace Scg
{
class Expression;
class UserDefinedFunction;
class Module;
class StructType;
class ValueType;
class ValueTypeSpec;
class ValueTypeSpecArray;
class Variable;

typedef std::vector<Scg::UserDefinedFunction *> FunctionArray;
typedef std::map<std::string, Scg::UserDefinedFunction *> FunctionMap;
typedef std::vector<Scg::ValueType*> ValueTypeArray;
typedef std::map<std::string, Scg::ValueType*> ValueTypeMap;
typedef std::map<std::string, Scg::Variable*> VariableMap;
typedef std::vector<Scg::Expression*> ExpressionArray;
typedef std::vector<Scg::Module*> ModuleArray;
typedef std::vector<Scg::Variable*> VariableArray;

/**
 * A pair of a value type and a name.
 */
class ValueTypeNamePair: public std::pair<ValueType*, std::string>
{
public:
  //! Default constructor.
  ValueTypeNamePair() :
          std::pair<ValueType*, std::string>()
  {
  }

  /**
   * Constructs a (value type, name) pair from the given values.
   * @param[in] valueType   A pointer to the value type.
   * @param[in] name        A string containing the name.
   */
  ValueTypeNamePair(ValueType *valueType, const std::string &name) :
          std::pair<ValueType*, std::string>(valueType, name)
  {
  }

  /**
   * Retrieves the value type of this pair.
   * @return A pointer to the value type of this pair.
   */
  const ValueType *GetValueType() const
  {
    return first;
  }
  ValueType *GetValueType()
  {
    return first;
  }

  /**
   * Retrieves the name of this pair.
   * @return A string containing the name.
   */
  const std::string &GetName() const
  {
    return second;
  }
};
typedef std::vector<ValueTypeNamePair> ValueTypeNameArray;

typedef std::vector<std::string> StringArray;
typedef std::pair<std::string, std::string> StringPair;
typedef std::vector<StringPair> StringPairArray;
typedef std::set<std::string> StringSet;

// TODO: Re-factor into a different place?
enum class TypeComparisonResult
{
  NotEquivalent = -1,
  Equivalent = 0,
  ImplicitlyEquivalent = 1,
  ExplicitlyEquivalent = 2,
};

}

#endif // __typedefs_h__
