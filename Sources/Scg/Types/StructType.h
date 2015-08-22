/**
* @file Scg/Types/StructType.h
*
* @copyright Copyright (C) 2014 Rafid Khalid Abdullah
*
* @license This file is released under Alusus Public License, Version 1.0.
* For details on usage and copying conditions read the full license in the
* accompanying license file or at <http://alusus.net/alusus_license_1_0>.
*/
//==============================================================================

#ifndef __StructureType_h__
#define __StructureType_h__

// Scg header files
#include <Types/ValueType.h>
#include <Types/ValueTypeSpec.h>

// LLVM forward declarations
#include <llvm_fwd.h>

namespace Scg
{

class DefineStruct;

/**
 * Represents a structure type with custom fields.
 */
class StructType : public ValueType
{
  friend class DefineStruct;

  std::string name;
  ValueTypeNameArray fields;
  ValueTypeSpecByName typeSpec;

protected:
  /**
   * Constructs a new structure type.
   * @param[in] name    The name of the structure.
   */
  StructType(const std::string &name);

  /**
   * Sets the fields of the structure.
   * @param[in] fields  An array of (type ID, name) pairs specifying the
   *                    fields of the structure.
   */
  void setFields(const ValueTypeNameArray &fields);

protected:
  //! @copydoc ValueType::initCastingTargets()
  virtual void initCastingTargets() const override;

public:
  //! @copydoc ValueType::GetName()
  virtual const std::string getName() const
  {
    return name;
  }

  /**
   * Retrieves the fields of the structure as an array of type-name pairs.
   * @return The array of type-name pairs defining the fields of the structure.
   */
  const ValueTypeNameArray &getFields() const
  {
    return fields;
  }

  /**
   * Retrieves the type-name pair of the field having the giver index.
   * @param[in] name  The index of the field.
   * @return Type-name pair.
   */
  const ValueTypeNamePair getFieldByIndex(int index) const
  {
    return fields[index];
  }

  /**
   * Retrieves the type-name pair of the field having the giver name.
   * @param[in] name  The name of the field.
   * @return Type-name pair.
   */
  const ValueTypeNamePair getFieldByName(const std::string &name)
  {
    return getFieldByIndex(getFieldIndex(name));
  }

  /**
   * Finds the index of the field having the given name.
   * @param[in] name  The name of the field.
   * @return The index of the field in the structure.
   */
  int getFieldIndex(const std::string &name)
  {
    for (auto nameType = fields.begin(); nameType != fields.end(); nameType++)
      if ((*nameType).second == name)
        return (int)(nameType - fields.begin());

    return -1;
  }

  //! @copydoc ValueType::GetDefaultLLVMValue()
  virtual llvm::Constant *getDefaultLLVMValue() const
  {
    throw EXCEPTION(NotImplementedException, "Not implemented yet!");
  }

  //! @copydoc ValueType::GetValueTypeSpec()
  virtual const ValueTypeSpec *getValueTypeSpec() const override
  {
    return &typeSpec;
  }

  //! @copydoc ValueType::IsEqualTo()
  virtual bool isEqualTo(const ValueType *other) const;

};

}

#endif // __StructureType_h__
