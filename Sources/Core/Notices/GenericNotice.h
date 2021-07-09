/**
 * @file Core/Notices/GenericNotice.h
 * Contains the header of class Core::Notices::GenericNotice.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_NOTICES_GENERICNOTICE_H
#define CORE_NOTICES_GENERICNOTICE_H

namespace Core::Notices
{

class GenericNotice : public Core::Notices::Notice
{
  TYPE_INFO(GenericNotice, Core::Notices::Notice, "Core.Notices", "Core", "alusus.org");

  //============================================================================
  // Member Variables

  private: Str code;
  private: Int severity;


  //============================================================================
  // Constructor / Destructor

  public: GenericNotice(Char const *c, Int s): code(c), severity(s)
  {
  }

  public: GenericNotice(Char const *c, Int s, SharedPtr<Data::SourceLocation> const &sl)
    : code(c), severity(s), Notice(sl)
  {
  }


  //============================================================================
  // Member Functions

  public: virtual Str const& getCode() const
  {
    return this->code;
  }

  public: virtual Int getSeverity() const
  {
    return this->severity;
  }

  public: virtual void buildDescription(Str &str) const
  {
    str = Core::Notices::L18nDictionary::getSingleton()->get(this->getCode());
  }

}; // class

} // namespace

#endif
