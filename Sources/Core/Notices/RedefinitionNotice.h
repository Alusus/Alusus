/**
 * @file Core/Notices/RedefinitionNotice.h
 * Contains the header of class Core::Notices::RedefinitionNotice.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_NOTICES_REDEFINITIONNOTICE_H
#define CORE_NOTICES_REDEFINITIONNOTICE_H

namespace Core::Notices
{

// TODO: DOC

class RedefinitionNotice : public Notice
{
  //============================================================================
  // Type Info

  TYPE_INFO(RedefinitionNotice, Notice, "Core.Notices", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: Str name;


  //============================================================================
  // Constructor / Destructor

  public: RedefinitionNotice(Char const *n, SharedPtr<Data::SourceLocation> const &sl) :
    Notice(sl), name(n)
  {
  }

  public: virtual ~RedefinitionNotice()
  {
  }


  //============================================================================
  // Member Functions

  /// @sa Notice::getCode()
  public: virtual Str const& getCode() const
  {
    static Str code(S("CG1004"));
    return code;
  }

  /// @sa Notice::getSeverity()
  public: virtual Int getSeverity() const
  {
    return 1;
  }

  /// Get the name of the duplicate definition.
  public: Str const& getDefinitionName() const
  {
    return this->name;
  }

  /// @sa Notice::getCode()
  public: virtual void buildDescription(Str &str) const
  {
    auto format = L18nDictionary::getSingleton()->get(this->getCode().c_str(), S("Duplicate definition. [%s]."));
    str = formatString(format, this->name.c_str());
  }

}; // class

} // namespace

#endif
