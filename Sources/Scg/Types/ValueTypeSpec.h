/**
 * @file Scg/Types/ValueTypeSpec.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __ValueTypeSpec_h__
#define __ValueTypeSpec_h__

// SCG header files
#include <Expression.h>
#include <typedefs.h>

// LLVM forward declarations
#include <llvm_fwd.h>

using namespace Core;

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
  virtual ~ValueTypeSpec() {};

  // TODO: Should this be defined here or under ValueTypeSpecByName?
  /**
   * Determines whether this instance specifies a void type.
   * @return @c true or @c false.
   */
  virtual bool IsVoid() const { return false; }

  /**
   * Creates a copy of the type specification.
   * @return A pointer to the new instance..
   */
  virtual ValueTypeSpec *Clone() const = 0;

  /**
   * Retrieves the ValueType object having the specification specified by this
   * instance from the given module.
   * @param[in] module  A pointer to the module
   * @return A pointer to the ValueType.
   */
  virtual ValueType *ToValueType(const Module &module) const = 0;

  /**
   * Determines whether this value type specification is equal to the given
   * type specification.
   * @param[in] other The other type specification to compare to.
   * @return @c true or @false.
   */
  virtual bool IsEqualTo(const ValueTypeSpec *other) const = 0;

  /**
   * Determines whether the type specified. by this spec can be implicitly
   * casted to the type specified by the given spec.
   * @param[in] other The other type specification.
   * @return @c true or @false.
   */
  virtual TypeComparisonResult Compare(const Module &module,
      const ValueTypeSpec *other) const;

  /**
   * Gets the string representation of the type specification.
   * @return A string representation of the type specification.
   */
  virtual std::string ToString() const = 0;

public:
  static void *operator new[] (size_t size) = delete;
  static void operator delete[] (void *ptr) = delete;
  static void *operator new (size_t size);
  static void operator delete (void *ptr);
};


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
  ValueTypeSpecByName(Char const *name) : name(name) {}

  //! @copydoc ValueTypeSpec::IsVoid()
  virtual bool IsVoid() const
  {
    return name == "" || name == "void";
  }

  //! @copydoc ValueTypeSpec::Clone()
  virtual ValueTypeSpec *Clone() const
  {
    return new ValueTypeSpecByName(name.c_str());
  }

  /**
   * Retrieves the name of the type specified by this instance.
   * @return The name of the type.
   */
  const std::string &GetName() const { return name; }

  //! @copydoc ValueTypeSpec::ToValueType()
  virtual ValueType *ToValueType(const Module &module) const;

  //! @copydoc ValueTypeSpec::IsEqualTo()
  virtual bool IsEqualTo(const ValueTypeSpec *other) const
  {
    auto otherCasted = dynamic_cast<const ValueTypeSpecByName*>(other);
    return otherCasted != nullptr && name.compare(otherCasted->name) == 0;
  }

  //! @copydoc ValueTypeSpec::Compare()
  virtual TypeComparisonResult Compare(const Module &module, const ValueTypeSpec *other) const;

  //! @copydoc ValueTypeSpec::ToString()
  virtual std::string ToString() const;
};

//==============================================================================

/**
 * Specifies a pointer value type.
 */
class PointerValueTypeSpec : public ValueTypeSpec
{
  //! The type specification of the content of this pointer type specification.
  ValueTypeSpec *contentTypeSpec;

public:
  /**
   * Constructs a pointer value type specification.
   * @param[in] contentTypeSpec   The specification of the content type.
   */
  PointerValueTypeSpec(ValueTypeSpec *contentTypeSpec)
    : contentTypeSpec(contentTypeSpec) {}

  //! Class destructor.
  virtual ~PointerValueTypeSpec();

  //! @copydoc ValueTypeSpec::Clone()
  virtual ValueTypeSpec *Clone() const
  {
    return new PointerValueTypeSpec(contentTypeSpec->Clone());
  }

  //! @copydoc ValueTypeSpec::ToValueType()
  virtual ValueType *ToValueType(const Module &module) const;

  //! @copydoc ValueTypeSpec::IsEqualTo()
  virtual bool IsEqualTo(const ValueTypeSpec *other) const
  {
    auto otherCasted = dynamic_cast<const PointerValueTypeSpec*>(other);
    return otherCasted != nullptr &&
        contentTypeSpec->IsEqualTo(otherCasted->contentTypeSpec);
  }

  //! @copydoc ValueTypeSpec::ToString()
  virtual std::string ToString() const;
};

//==============================================================================

/**
 * Specifies an array value type.
 */
class ArrayValueTypeSpec : public ValueTypeSpec
{
  //! The type specification of the elements specified by this type specification.
  ValueTypeSpec *elementsTypeSpec;
  //! The size of the array specified by this type specification.
  int arraySize;

public:
  /**
   * Constructs an array value type specification.
   * @param[in] elementsTypeSpec  The specification of the elements type.
   * @param[in] arraySize         The size of the array type specification.
   */
  ArrayValueTypeSpec(ValueTypeSpec *elementsTypeSpec, int arraySize)
    : elementsTypeSpec(elementsTypeSpec), arraySize(arraySize) {}

  //! Class destructor.
  virtual ~ArrayValueTypeSpec();

  //! @copydoc ValueTypeSpec::Clone()
  virtual ValueTypeSpec *Clone() const
  {
    return new ArrayValueTypeSpec(elementsTypeSpec->Clone(), arraySize);
  }

  //! @copydoc ValueTypeSpec::ToValueType()
  virtual ValueType *ToValueType(const Module &module) const;

  //! @copydoc ValueTypeSpec::IsEqualTo()
  virtual bool IsEqualTo(const ValueTypeSpec *other) const
  {
    auto otherCasted = dynamic_cast<const ArrayValueTypeSpec*>(other);
    return otherCasted != nullptr &&
        arraySize == otherCasted->arraySize &&
        elementsTypeSpec->IsEqualTo(otherCasted->elementsTypeSpec);
  }

  //! @copydoc ValueTypeSpec::ToString()
  virtual std::string ToString() const;
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
class VariableDefinition : public std::pair<const ValueTypeSpec*, std::string>
{
public:
  /**
   * Default constructor.
   */
  VariableDefinition() : std::pair<const ValueTypeSpec*, std::string>(nullptr, "") {}

  // TODO: Need to avoid using default memory allocation here because it is slow.
  /**
   * Constructs a variable definition from the given value type name
   * and name, setting the pointer level to 0.
   *
   * @param[in] typeName  The name of the variable type.
   * @param[in] name      The name of the variable.
   */
  VariableDefinition(const std::string &typeName, const std::string &name)
      : std::pair<const ValueTypeSpec*, std::string>(new ValueTypeSpecByName(typeName.c_str()), name) {}

  /**
   * Construct a variable definition from the given type specification and name.
   *
   * @param[in] typeSpec  The specification of the type of the variable. This
   *                      should be dynamically allocated as it gets deleted by
   *                      this class at destruction time.
   * @param[in] name      The name of the variable.
   */
  VariableDefinition(const ValueTypeSpec *typeSpec, const std::string &name)
      : std::pair<const ValueTypeSpec*, std::string>(typeSpec, name) {}

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
  const ValueTypeSpec *GetTypeSpec() const { return first; }

  /**
   * Gets the name of the variable.
   * @return A string containing the name of the variable.
   */
  const std::string &GetVariableName() const { return second; }

  /**
   * Sets the name of the variable.
   * @return A string containing the name of the variable.
   */
  void SetVariableName(const std::string &varName) { second = varName; }

  /**
   * Returns a string representation of the type ID/name pairs.
   * @return The string representation of the type ID/name pairs.
   */
  const std::string ToString() const
  {
    return GetVariableName() + ":" + GetTypeSpec()->ToString();
  }
};

//==============================================================================

/**
 * An array of value type specifications.
 */
class ValueTypeSpecArray : public std::vector<ValueTypeSpec*>
{
  using std::vector<ValueTypeSpec*>::vector;

public:
  /**
   * Determines whether this array of value type specifications is equal to the
   * given array.
   * @param[in] other The other array to compare to.
   * @return @c true or @false.
   */
  bool IsEqualTo(const ValueTypeSpecArray *other, int sizeLimit = 0) const;

};

/**
 * An array of value type ID/name pairs.
 */
class VariableDefinitionArray : public std::vector<VariableDefinition>
{
  using std::vector<VariableDefinition>::vector;

public:
  /**
   * Determines whether the types in this array of value type specifications
   * are equal to the given array of value type specifications.
   * @param[in] other The array of value type specifications to compare with.
   * @return @c true or @false.
   */
  bool AreTypesEqualTo(const ValueTypeSpecArray *other, int sizeLimit = 0) const;
};

}

#endif // __ValueTypeSpec_h__
