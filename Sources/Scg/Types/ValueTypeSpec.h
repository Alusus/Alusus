/**
* @file Scg/Types/ValueTypeSpec.h
*
* @copyright Copyright (C) 2016 Rafid Khalid Abdullah
*
* @license This file is released under Alusus Public License, Version 1.0.
* For details on usage and copying conditions read the full license in the
* accompanying license file or at <http://alusus.net/alusus_license_1_0>.
*/
//==============================================================================

#ifndef SCG_VALUETYPESPEC_H
#define SCG_VALUETYPESPEC_H

#include "core.h"
#include <typedefs.h>
#include <llvm_fwd.h>

namespace Scg
{

// TODO: This class has high usage frequency and thus need to be optimised as
// much as possible. So we need to override the "new" and "delete" operators to
// make it use a pre-allocated memory and thus avoid the cost of frequently
// allocating memory in the heap. Consider using
/**
 * Base class for all value type specifications. These are used to a specify
 * a soft link to a type (e.g. by name, as opposed to a passing the type
 * instance itself, which I call the hard link). This allows the user to deal
 * with instructions only without having to worry about the underlying objects
 * being created by the Intermediate Language.
 */
class ValueTypeSpec
{
public:
  virtual ~ValueTypeSpec()
  {
  }

  // TODO: Should this be defined here or under ValueTypeSpecByName?
  /**
  * Determines whether this instance specifies a void type.
  * @return @c true or @c false.
  */
  virtual bool isVoid() const
  {
    return false;
  }

  /**
  * Creates a copy of the type specification.
  * @return A pointer to the new instance..
  */
  virtual SharedPtr<ValueTypeSpec> clone() const = 0;

  /**
  * Retrieves the ValueType object having the specification specified by this
  * instance from the given module.
  * @param[in] module  A pointer to the module
  * @return A pointer to the ValueType.
  */
  virtual ValueType* toValueType(const Module &module) const = 0;

  /**
  * Determines whether this value type specification is equal to the given
  * type specification.
  * @param[in] other The other type specification to compare to.
  * @return @c true or @false.
  */
  virtual bool isEqualTo(const ValueTypeSpec *other) const = 0;

  /**
  * Determines whether the type specified. by this spec can be implicitly
  * casted to the type specified by the given spec.
  * @param[in] other The other type specification.
  * @return @c true or @false.
  */
  virtual TypeComparisonResult compare(const Module &module,
                                       const ValueTypeSpec *other) const;

  /**
  * Gets the string representation of the type specification.
  * @return A string representation of the type specification.
  */
  virtual std::string toString() const = 0;

};

//==============================================================================

/**
 * Specifies a type by name.
 */
class ValueTypeSpecByName : public ValueTypeSpec
{
  // TODO: Consider using a character array instead of std::string to eliminate
  // the cost of allocating memory in heap for the string.
  // Another option is to implement a fast memory allocator (e.g. LLVM's
  // BumpPtrAllocator) and pass to to std::basic_string.
  std::string name;

public:
  ValueTypeSpecByName(Core::Basic::Char const *name) : name(name)
  {
  }

  //! @copydoc ValueTypeSpec::isVoid()
  virtual bool isVoid() const
  {
    return name == "" || name == "void";
  }

  //! @copydoc ValueTypeSpec::clone()
  virtual SharedPtr<ValueTypeSpec> clone() const
  {
    return std::make_shared<ValueTypeSpecByName>(name.c_str());
  }

  /**
  * Retrieves the name of the type specified by this instance.
  * @return The name of the type.
  */
  const std::string &getName() const
  {
    return name;
  }

  //! @copydoc ValueTypeSpec::toValueType()
  virtual ValueType *toValueType(const Module &module) const;

  //! @copydoc ValueTypeSpec::isEqualTo()
  virtual bool isEqualTo(const ValueTypeSpec *other) const
  {
    auto otherCasted = dynamic_cast<const ValueTypeSpecByName *>(other);
    return otherCasted != nullptr && name.compare(otherCasted->name) == 0;
  }

  //! @copydoc ValueTypeSpec::compare()
  virtual TypeComparisonResult compare(const Module &module, const ValueTypeSpec *other) const;

  //! @copydoc ValueTypeSpec::toString()
  virtual std::string toString() const;
};

//==============================================================================

/**
* Specifies a pointer value type.
*/
class PointerValueTypeSpec : public ValueTypeSpec
{
  //! The type specification of the content of this pointer type specification.
  SharedPtr<ValueTypeSpec> contentTypeSpec;

public:
  /**
  * Constructs a pointer value type specification.
  * @param[in] contentTypeSpec   The specification of the content type.
  */
  PointerValueTypeSpec(SharedPtr<ValueTypeSpec> const &contentTypeSpec)
    : contentTypeSpec(contentTypeSpec)
  {
  }

  //! @copydoc ValueTypeSpec::clone()
  virtual SharedPtr<ValueTypeSpec> clone() const
  {
    return std::make_shared<PointerValueTypeSpec>(contentTypeSpec->clone());
  }

  //! @copydoc ValueTypeSpec::toValueType()
  virtual ValueType *toValueType(const Module &module) const;

  //! @copydoc ValueTypeSpec::isEqualTo()
  virtual bool isEqualTo(const ValueTypeSpec *other) const
  {
    auto otherCasted = dynamic_cast<const PointerValueTypeSpec *>(other);
    return otherCasted != nullptr &&
           contentTypeSpec->isEqualTo(otherCasted->contentTypeSpec.get());
  }

  //! @copydoc ValueTypeSpec::toString()
  virtual std::string toString() const;
};

//==============================================================================

/**
* Specifies an array value type.
*/
class ArrayValueTypeSpec : public ValueTypeSpec
{
  //! The type specification of the elements specified by this type specification.
  SharedPtr<ValueTypeSpec> elementsTypeSpec;
  //! The size of the array specified by this type specification.
  int arraySize;

public:
  /**
  * Constructs an array value type specification.
  * @param[in] elementsTypeSpec  The specification of the elements type.
  * @param[in] arraySize         The size of the array type specification.
  */
  ArrayValueTypeSpec(SharedPtr<ValueTypeSpec> const &elementsTypeSpec, int arraySize)
    : elementsTypeSpec(elementsTypeSpec), arraySize(arraySize)
  {
  }

  //! @copydoc ValueTypeSpec::clone()
  virtual SharedPtr<ValueTypeSpec> clone() const
  {
    return std::make_shared<ArrayValueTypeSpec>(elementsTypeSpec->clone(), arraySize);
  }

  //! @copydoc ValueTypeSpec::toValueType()
  virtual ValueType *toValueType(const Module &module) const;

  //! @copydoc ValueTypeSpec::isEqualTo()
  virtual bool isEqualTo(const ValueTypeSpec *other) const
  {
    auto otherCasted = dynamic_cast<const ArrayValueTypeSpec *>(other);
    return otherCasted != nullptr &&
           arraySize == otherCasted->arraySize &&
           elementsTypeSpec->isEqualTo(otherCasted->elementsTypeSpec.get());
  }

  //! @copydoc ValueTypeSpec::toString()
  virtual std::string toString() const;
};

//==============================================================================

// TODO: Ensure that this is used everywhere we pass a type name and a pointer
// level.
/**
* A pair of a variable type and a variable name.
*
* @note This class assumes the pointer to ValueTypeSpec to be allocated in heap
* and it will take the responsibility of deallocating the memory.
*/
class VariableDefinition : public std::pair<SharedPtr<ValueTypeSpec>, std::string>
{
public:
  /**
  * Default constructor.
  */
  VariableDefinition() : std::pair<SharedPtr<ValueTypeSpec>, std::string>(nullptr, "")
  {
  }

  // TODO: Need to avoid using default memory allocation here because it is slow.
  /**
  * Constructs a variable definition from the given value type name
  * and name, setting the pointer level to 0.
  *
  * @param[in] typeName  The name of the variable type.
  * @param[in] name      The name of the variable.
  */
  VariableDefinition(const std::string &typeName, const std::string &name)
    : std::pair<SharedPtr<ValueTypeSpec>, std::string>(std::make_shared<ValueTypeSpecByName>(typeName.c_str()), name)
  {
  }

  /**
  * Construct a variable definition from the given type specification and name.
  *
  * @param[in] typeSpec  The specification of the type of the variable. This
  *                      should be dynamically allocated as it gets deleted by
  *                      this class at destruction time.
  * @param[in] name      The name of the variable.
  */
  VariableDefinition(SharedPtr<ValueTypeSpec> const &typeSpec, const std::string &name)
    : std::pair<SharedPtr<ValueTypeSpec>, std::string>(typeSpec, name)
  {
  }

  /**
  * Class destructor.
  */
  ~VariableDefinition()
  {
    // Let this class take the responsibility of deleting ValueTypeSpec*.
    /*if (first != nullptr)
      delete first;*/
  }

  /**
  * Gets the type specification of the variable.
  * @return The type specification of the variable.
  */
  SharedPtr<ValueTypeSpec> const& getTypeSpec() const
  {
    return this->first;
  }

  /**
  * Gets the name of the variable.
  * @return A string containing the name of the variable.
  */
  std::string const& getVariableName() const
  {
    return this->second;
  }

  /**
  * Sets the name of the variable.
  * @return A string containing the name of the variable.
  */
  void setVariableName(std::string const &varName)
  {
    this->second = varName;
  }

  /**
  * Returns a string representation of the type ID/name pairs.
  * @return The string representation of the type ID/name pairs.
  */
  const std::string toString() const
  {
    return getVariableName() + ":" + getTypeSpec()->toString();
  }
};

//==============================================================================

/**
* An array of value type specifications.
*/
class ValueTypeSpecArray : public std::vector<SharedPtr<ValueTypeSpec>>
{
  using std::vector<SharedPtr<ValueTypeSpec>>::vector;

  /**
  * Determines whether this array of value type specifications is equal to the
  * given array.
  * @param[in] other The other array to compare to.
  * @return @c true or @false.
  */
  public: bool isEqualTo(const ValueTypeSpecArray *other, int sizeLimit = 0) const;
};

/**
* An array of value type ID/name pairs.
*/
class VariableDefinitionArray : public std::vector < VariableDefinition >
{
  using std::vector<VariableDefinition>::vector;

  /**
  * Determines whether the types in this array of value type specifications
  * are equal to the given array of value type specifications.
  * @param[in] other The array of value type specifications to compare with.
  * @return @c true or @false.
  */
  public: bool areTypesEqualTo(const ValueTypeSpecArray *other, int sizeLimit = 0) const;
};

} // namespace

#endif
