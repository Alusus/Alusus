/**
 * @file Core/Processing/CustomBuildMsg.h
 *  Contains the header of class Core::Processing::CustomBuildMsg.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef PROCESSING_CUSTOM_BUILD_MSG_H
#define PROCESSING_CUSTOM_BUILD_MSG_H

namespace Core { namespace Processing
{

// TODO: DOC

class CustomBuildMsg : public BuildMsg
{
  //============================================================================
  // Type Info

  TYPE_INFO(CustomBuildMsg, BuildMsg, "Core.Processing", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: Str code;
  private: Str msg;
  private: Str param;
  private: Int severity;


  //============================================================================
  // Constructor / Destructor

  public: CustomBuildMsg(Char const *msg, Data::SourceLocation const &sl, Char const *p=0) :
    BuildMsg(sl), msg(msg), code(STR("G1002")), severity(1)
  {
    if (p) this->param = p;
  }

  public: CustomBuildMsg(Char const *msg, Char const *code, Int severity, Data::SourceLocation const &sl, Char const *p=0) :
    BuildMsg(sl), msg(msg), code(code), severity(severity)
  {
    if (p) this->param = p;
  }

  public: virtual ~CustomBuildMsg()
  {
  }


  //============================================================================
  // Member Functions

  /// @sa BuildMsg::getCode()
  public: virtual Str const& getCode() const
  {
    return this->code;
  }

  /// @sa BuildMsg::getSeverity()
  public: virtual Int getSeverity() const
  {
    return this->severity;
  }

  /// @sa BuildMsg::getCode()
  public: virtual void buildDescription(Str &str) const
  {
    str = this->msg;
    if (this->param.size() > 0) {
      str += STR(": ");
      str += param;
    }
  }

}; // class

} } // namespace

#endif
