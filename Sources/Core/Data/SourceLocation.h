/**
 * @file Core/Data/SourceLocation.h
 * Contains the header of class Core::Data::SourceLocation.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_SOURCELOCATION_H
#define CORE_DATA_SOURCELOCATION_H

namespace Core { namespace Data
{

/**
 * @brief The location within the source code.
 * @ingroup data
 *
 * This class holds the location data within the source code of a token or
 * a parsed data object. This includes, the name of the source file, and the
 * line and column within that file at which the token appeared.
 */
class SourceLocation : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(SourceLocation, TiObject, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Members

  public: SharedPtr<Str> filename;

  /**
   * @brief The line number within the source file.
   * The numbering starts from 1.
   */
  public: Int line = 0;

  /**
   * @brief The column number within the source file.
   * The numbering starts from 1.
   */
  public: Int column = 0;

  //============================================================================
  // Constructors and Operators

  public: SourceLocation()
  {
  }

  public: SourceLocation(SharedPtr<Str> const &fname, Int l, Int c) : filename(fname), line(l), column(c)
  {
  }

  public: Bool operator==(SourceLocation const &sl) const
  {
    return this->filename == sl.filename && this->line == sl.line && this->column == sl.column;
  }

}; // class

} } // namespace

#endif
