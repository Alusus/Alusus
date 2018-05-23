/**
 * @file Core/Data/SourceLocationRecord.h
 * Contains the header of class Core::Data::SourceLocationRecord.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
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
 * @brief The base of source code location classes.
 * @ingroup core_data
 */
class SourceLocation : public TiObject
{
  TYPE_INFO(SourceLocation, TiObject, "Core.Data", "Core", "alusus.net");
};


/**
 * @brief The location within the source code.
 * @ingroup core_data
 *
 * This class holds the location data within the source code of a token or
 * a parsed data object. This includes, the name of the source file, and the
 * line and column within that file at which the token appeared.
 */
class SourceLocationRecord : public SourceLocation
{
  //============================================================================
  // Type Info

  TYPE_INFO(SourceLocationRecord, SourceLocation, "Core.Data", "Core", "alusus.net");


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

  public: SourceLocationRecord()
  {
  }

  public: SourceLocationRecord(SharedPtr<Str> const &fname, Int l, Int c) : filename(fname), line(l), column(c)
  {
  }

  public: Bool operator==(SourceLocationRecord const &sl) const
  {
    return this->filename == sl.filename && this->line == sl.line && this->column == sl.column;
  }

}; // class


typedef SharedList<SourceLocationRecord, SourceLocation> _SourceLocationStackBase;


/**
 * @brief A stack of SourceLocationRecord objects.
 * @ingroup core_data
 */
class SourceLocationStack : public _SourceLocationStackBase
{
  //============================================================================
  // Type Info

  TYPE_INFO(
    SourceLocationStack, _SourceLocationStackBase, "Core.Data", "Core", "alusus.net"
  );


  //============================================================================
  // Constructors

  public: using _SourceLocationStackBase::SharedList;


  //============================================================================
  // Member Functions

  public: void push(SourceLocation *sl);

  public: void pop(Word count = 1);

};

} } // namespace

#endif
