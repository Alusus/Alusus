/**
 * @file Core/Common/UnrecognizedErrorMsg.h
 * Contains the header of class Core::Common::UnrecognizedErrorMsg.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef COMMON_UNRECOGNIZED_ERROR_MSG_H
#define COMMON_UNRECOGNIZED_ERROR_MSG_H

namespace Core { namespace Common
{

// TODO: DOC

class UnrecognizedErrorMsg : public BuildMsg
{
  //============================================================================
  // Type Info

  TYPE_INFO(UnrecognizedErrorMsg, BuildMsg, "Core.Common", "Core", "alusus.net");


  //============================================================================
  // Constructor / Destructor

  public: UnrecognizedErrorMsg(Int l, Int c) : BuildMsg(l, c)
  {
  }

  public: virtual ~UnrecognizedErrorMsg()
  {
  }


  //============================================================================
  // Member Functions

  /// @sa BuildMsg::getCode()
  public: virtual const Str& getCode() const
  {
    static Str code("G1001");
    return code;
  }

  /// @sa BuildMsg::getSeverity()
  public: virtual Int getSeverity() const
  {
    return 1;
  }

  /// @sa BuildMsg::getCode()
  public: virtual void buildDescription(Str &str) const
  {
    str = STR("Unrecognized Error.");
  }

}; // class

} } // namespace

#endif
