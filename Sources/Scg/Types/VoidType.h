/**
* @file Scg/Types/VoidType.h
*
* @copyright Copyright (C) 2014 Rafid Khalid Abdullah
*
* @license This file is released under Alusus Public License, Version 1.0.
* For details on usage and copying conditions read the full license in the
* accompanying license file or at <http://alusus.net/alusus_license_1_0>.
*/
//==============================================================================

#ifndef __VoidType_h__
#define __VoidType_h__

// Scg header files
#include <Types/ValueType.h>
#include <Types/ValueTypeSpec.h>

// LLVM forward declarations
#include <llvm_fwd.h>

namespace Scg
{
/**
* Represent a variable type.
*/
class VoidType : public ValueType
{
  friend class LlvmContainer;

  static VoidType *s_singleton;
  ValueTypeSpecByName typeSpec;

private:
  /**
  * Constructs a void type.
  */
  VoidType();

  /**
  * Class destructor.
  */
  virtual ~VoidType()
  {
  }

protected:
  //! @copydoc ValueType::initCastingTargets()
  virtual void initCastingTargets() const override;

public:
  //! @copydoc ValueType::getName()
  virtual const std::string getName() const
  {
    return "void";
  }

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
    // TODO: Should we throw InvalidOperationException here?
    return dynamic_cast<const VoidType *>(other) != nullptr;
  }

  static VoidType *get();
};

}

#endif // __VoidType_h__
