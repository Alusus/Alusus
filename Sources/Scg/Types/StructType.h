/**
* @file Scg/Types/StructType.h
*
* @copyright Copyright (C) 2016 Rafid Khalid Abdullah
*
* @license This file is released under Alusus Public License, Version 1.0.
* For details on usage and copying conditions read the full license in the
* accompanying license file or at <http://alusus.net/alusus_license_1_0>.
*/
//==============================================================================

#ifndef SCG_STRUCTTYPE_H
#define SCG_STRUCTTYPE_H

#include <Types/ValueType.h>
#include <Types/ValueTypeSpec.h>
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

  //============================================================================
  // Type Info

  TYPE_INFO(StructType, ValueType, "Scg", "Scg", "alusus.net");


  //============================================================================
  // Member Variables

  private: std::string name;
  private: ValueTypeNameArray fields;
  private: SharedPtr<ValueTypeSpec> typeSpec;


  //============================================================================
  // Constructors & Destructor

  /**
   * Constructs a new structure type.
   * @param[in] name    The name of the structure.
   */
  public: StructType(const std::string &name);


  //============================================================================
  // Member Functions

  /**
   * Sets the fields of the structure.
   * @param[in] fields  An array of (type ID, name) pairs specifying the
   *                    fields of the structure.
   */
  protected: void setFields(const ValueTypeNameArray &fields);

  //! @copydoc ValueType::initCastingTargets()
  protected: virtual void initCastingTargets() const override;

  //! @copydoc ValueType::GetName()
  public: virtual std::string const& getName() const
  {
    return this->name;
  }

  /**
   * Retrieves the fields of the structure as an array of type-name pairs.
   * @return The array of type-name pairs defining the fields of the structure.
   */
  public: ValueTypeNameArray const& getFields() const
  {
    return this->fields;
  }

  /**
   * Retrieves the type-name pair of the field having the giver index.
   * @param[in] name  The index of the field.
   * @return Type-name pair.
   */
  public: ValueTypeNamePair const& getFieldByIndex(int index) const
  {
    return fields[index];
  }

  /**
   * Retrieves the type-name pair of the field having the giver name.
   * @param[in] name  The name of the field.
   * @return Type-name pair.
   */
  public: ValueTypeNamePair const& getFieldByName(const std::string &name)
  {
    return this->getFieldByIndex(getFieldIndex(name));
  }

  /**
   * Finds the index of the field having the given name.
   * @param[in] name  The name of the field.
   * @return The index of the field in the structure.
   */
  public: int getFieldIndex(std::string const &name)
  {
    for (auto nameType = fields.begin(); nameType != fields.end(); nameType++)
      if ((*nameType).second == name)
        return (int)(nameType - fields.begin());

    return -1;
  }

  //! @copydoc ValueType::GetDefaultLLVMValue()
  public: virtual llvm::Constant* getDefaultLLVMValue() const
  {
    throw EXCEPTION(NotImplementedException, "Not implemented yet!");
  }

  //! @copydoc ValueType::GetValueTypeSpec()
  public: virtual SharedPtr<ValueTypeSpec> const& getValueTypeSpec() const override
  {
    return this->typeSpec;
  }

  //! @copydoc ValueType::IsEqualTo()
  public: virtual bool isEqualTo(const ValueType *other) const;

}; // class

} // namespace

#endif
