/**
 * @file Core/Data/CustomNotice.h
 *  Contains the header of class Core::Data::CustomNotice.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_CUSTOMNOTICE_H
#define CORE_DATA_CUSTOMNOTICE_H

namespace Core { namespace Data
{

// TODO: DOC

class CustomNotice : public Notice
{
  //============================================================================
  // Type Info

  TYPE_INFO(CustomNotice, Notice, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: Str code;
  private: Str msg;
  private: Str param;
  private: Int severity;


  //============================================================================
  // Constructor / Destructor

  public: CustomNotice(Char const *msg, Data::SourceLocation const &sl, Char const *p=0) :
    Notice(sl), msg(msg), code(STR("G1002")), severity(1)
  {
    if (p) this->param = p;
  }

  public: CustomNotice(Char const *msg, Char const *code, Int severity, Data::SourceLocation const &sl, Char const *p=0)
    : Notice(sl), msg(msg), code(code), severity(severity)
  {
    if (p) this->param = p;
  }

  public: virtual ~CustomNotice()
  {
  }


  //============================================================================
  // Member Functions

  /// @sa Notice::getCode()
  public: virtual Str const& getCode() const
  {
    return this->code;
  }

  /// @sa Notice::getSeverity()
  public: virtual Int getSeverity() const
  {
    return this->severity;
  }

  /// @sa Notice::getCode()
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
