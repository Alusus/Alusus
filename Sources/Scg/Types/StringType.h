/**
* @file Scg/Types/StringType.h
*
* @copyright Copyright (C) 2014 Rafid Khalid Abdullah
*
* @license This file is released under Alusus Public License, Version 1.0.
* For details on usage and copying conditions read the full license in the
* accompanying license file or at <http://alusus.net/alusus_license_1_0>.
*/
//==============================================================================

#ifndef __StringType_h__
#define __StringType_h__

// Scg header files
#include <Types/ValueType.h>
#include <Types/ValueTypeSpec.h>

// LLVM forward declarations
#include <llvm_fwd.h>

namespace Scg
{
class Module;

/**
* Represent a variable type.
*/
class StringType : public ValueType
{
  friend class LlvmContainer;

  static StringType *s_singleton;
  ValueTypeSpecByName typeSpec;

private:
  //! Constructs a string type.
  StringType();

  //! Class destructor.
  virtual ~StringType()
  {
  }

protected:
  //! @copydoc ValueType::initCastingTargets()
  virtual void initCastingTargets() const override;

public:
  //! @copydoc ValueType::getName()
  virtual const std::string getName() const
  {
    return "string";
  }

  /**
  * Get an constant value of string type.
  * @param[in] value The value of the constant.
  * @param[in] value The value of the constant.
  */
  // TODO: Can we find some way to remove module?
  llvm::Constant *getLlvmConstant(Module *module, const std::string &value) const;

  //! @copydoc ValueType::getDefaultLLVMValue()
  virtual llvm::Constant *getDefaultLLVMValue() const
  {
    throw EXCEPTION(NotImplementedException, "Not implemented yet!");
  }

  //! @copydoc ValueType::getValueTypeSpec()
  virtual const ValueTypeSpec *getValueTypeSpec() const override
  {
    return &typeSpec;
  }

  //! @copydoc ValueType::isEqualTo()
  virtual bool isEqualTo(const ValueType *other) const
  {
    return dynamic_cast<const StringType *>(other) != nullptr;
  }

  static StringType *get();
};
}

#endif // __StringType_h__
