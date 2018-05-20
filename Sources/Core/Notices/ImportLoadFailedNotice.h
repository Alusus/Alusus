/**
 * @file Core/Notices/ImportLoadFailedNotice.h
 * Contains the header of class Core::Notices::ImportLoadFailedNotice.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_NOTICES_IMPORTLOADFAILEDNOTICE_H
#define CORE_NOTICES_IMPORTLOADFAILEDNOTICE_H

namespace Core::Notices
{

// TODO: DOC

class ImportLoadFailedNotice : public Notice
{
  //============================================================================
  // Type Info

  TYPE_INFO(ImportLoadFailedNotice, Notice, "Core.Notices", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: Str fileName;
  private: Str errorDetails;


  //============================================================================
  // Constructor / Destructor

  public: ImportLoadFailedNotice(
    Char const *fileName, Char const *errorDetails, SharedPtr<Data::SourceLocation> const &sl
  ) : Notice(sl), fileName(fileName), errorDetails(errorDetails)
  {
  }

  public: virtual ~ImportLoadFailedNotice()
  {
  }


  //============================================================================
  // Member Functions

  /// @sa Notice::getCode()
  public: virtual Str const& getCode() const
  {
    static Str code("CG1003");
    return code;
  }

  /// @sa Notice::getSeverity()
  public: virtual Int getSeverity() const
  {
    return 1;
  }

  /// @sa Notice::getCode()
  public: virtual void buildDescription(Str &str) const
  {
    str = STR("Importing Failed. Could not load requested file: ") + fileName + STR(".");
    if (!this->errorDetails.empty()) {
      str += STR("\n");
      str += this->errorDetails;
    }
  }

}; // class

} // namespace

#endif
