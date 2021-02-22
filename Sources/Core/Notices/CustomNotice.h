/**
 * @file Core/Notices/CustomNotice.h
 * Contains the header of class Core::Notices::CustomNotice.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_NOTICES_CUSTOMNOTICE_H
#define CORE_NOTICES_CUSTOMNOTICE_H

namespace Core::Notices
{

// TODO: DOC

class CustomNotice : public Notice
{
  //============================================================================
  // Type Info

  TYPE_INFO(CustomNotice, Notice, "Core.Notices", "Core", "alusus.org");


  //============================================================================
  // Member Variables

  private: Str code;
  private: Str msg;
  private: Str param;
  private: Int severity;


  //============================================================================
  // Constructor / Destructor

  public: CustomNotice(Char const *msg, SharedPtr<Data::SourceLocation> const &sl, Char const *p=0) :
    Notice(sl), msg(msg), code(S("G1002")), severity(1)
  {
    if (p) this->param = p;
  }

  public: CustomNotice(
    Char const *msg, Char const *code, Int severity, SharedPtr<Data::SourceLocation> const &sl, Char const *p=0
  ) : Notice(sl), msg(msg), code(code), severity(severity)
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
    str = L18nDictionary::getSingleton()->get(this->getCode(), this->msg);
    if (this->param.getLength() > 0) {
      str += S(": ");
      str += param;
    }
  }

}; // class

} // namespace

#endif
