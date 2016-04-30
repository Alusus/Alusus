/**
 * @file Core/Standard/ImportLoadFailedMsg.h
 * Contains the header of class Core::Standard::ImportLoadFailedMsg.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_STANDARD_IMPORTLOADFAILEDMSG_H
#define CORE_STANDARD_IMPORTLOADFAILEDMSG_H

namespace Core { namespace Standard
{

// TODO: DOC

class ImportLoadFailedMsg : public Processing::BuildMsg
{
  //============================================================================
  // Type Info

  TYPE_INFO(ImportLoadFailedMsg, Processing::BuildMsg, "Core.Standard", "Core", "alusus.net");


  //============================================================================
  // Constructor / Destructor

  Str fileName;

  public: ImportLoadFailedMsg(Char const * fileName, Data::SourceLocation const &sl) :
    Processing::BuildMsg(sl), fileName(fileName)
  {
  }

  public: virtual ~ImportLoadFailedMsg()
  {
  }


  //============================================================================
  // Member Functions

  /// @sa Processing::BuildMsg::getCode()
  public: virtual Str const& getCode() const
  {
    static Str code("IMP1001");
    return code;
  }

  /// @sa Processing::BuildMsg::getSeverity()
  public: virtual Int getSeverity() const
  {
    return 1;
  }

  /// @sa Processing::BuildMsg::getCode()
  public: virtual void buildDescription(Str &str) const
  {
    str = STR("Importing Failed. Could not load requested file: " + fileName + ".");
  }

}; // class

} } // namespace

#endif
