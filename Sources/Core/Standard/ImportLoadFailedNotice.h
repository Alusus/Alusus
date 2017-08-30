/**
 * @file Core/Standard/ImportLoadFailedNotice.h
 * Contains the header of class Core::Standard::ImportLoadFailedNotice.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_STANDARD_IMPORTLOADFAILEDNOTICE_H
#define CORE_STANDARD_IMPORTLOADFAILEDNOTICE_H

namespace Core { namespace Standard
{

// TODO: DOC

class ImportLoadFailedNotice : public Data::Notice
{
  //============================================================================
  // Type Info

  TYPE_INFO(ImportLoadFailedNotice, Data::Notice, "Core.Standard", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: Str fileName;
  private: Str errorDetails;


  //============================================================================
  // Constructor / Destructor

  public: ImportLoadFailedNotice(Char const *fileName, Char const *errorDetails, Data::SourceLocation const &sl) :
    Data::Notice(sl), fileName(fileName), errorDetails(errorDetails)
  {
  }

  public: ImportLoadFailedNotice(
    Char const *fileName, Char const *errorDetails, std::vector<Data::SourceLocation> const &sl
  ) : Data::Notice(sl), fileName(fileName), errorDetails(errorDetails)
  {
  }

  public: ImportLoadFailedNotice(
    Char const *fileName, Char const *errorDetails, std::vector<Data::SourceLocation> *sl
  ) : Data::Notice(sl), fileName(fileName), errorDetails(errorDetails)
  {
  }

  public: virtual ~ImportLoadFailedNotice()
  {
  }


  //============================================================================
  // Member Functions

  /// @sa Data::Notice::getCode()
  public: virtual Str const& getCode() const
  {
    static Str code("IMP1001");
    return code;
  }

  /// @sa Data::Notice::getSeverity()
  public: virtual Int getSeverity() const
  {
    return 1;
  }

  /// @sa Data::Notice::getCode()
  public: virtual void buildDescription(Str &str) const
  {
    str = STR("Importing Failed. Could not load requested file: ") + fileName + STR(".");
    if (!this->errorDetails.empty()) {
      str += STR("\n");
      str += this->errorDetails;
    }
  }

}; // class

} } // namespace

#endif
