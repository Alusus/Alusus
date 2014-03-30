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

  public:
    //! Constructs a string type.
    StringType();

    //! Class destructor.
    virtual ~StringType() {}

    //! @copydoc ValueType::GetName()
    virtual const std::string GetName() const { return "string"; }

    /**
     * Get an constant value of string type.
     * @param[in] value The value of the constant.
     * @param[in] value The value of the constant.
     */
    // TODO: Can we find some way to remove module?
    llvm::Constant *GetLlvmConstant(Module *module, const std::string &value) const;

    //! @copydoc ValueType::GetDefaultLLVMValue()
    virtual llvm::Constant *GetDefaultLLVMValue() const
    {
      THROW_EXCEPTION(NotImplementedException, "Not implemented yet!");
    }

    //! @copydoc ValueType::GetValueTypeSpec()
    virtual const ValueTypeSpec *GetValueTypeSpec() const
    {
      return &typeSpec;
    }

    static StringType *GetSingleton() { return s_singleton; }
  };
}

#endif // __StringType_h__
