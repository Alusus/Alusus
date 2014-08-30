/**
 * @file Core/Processing/BuildMsg.h
 * Contains the header of class Core::Processing::BuildMsg.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef PROCESSING_BUILD_MSG_H
#define PROCESSING_BUILD_MSG_H

namespace Core { namespace Processing
{

/**
 * @brief The base of all build messages.
 * @ingroup processing
 *
 * Build messages are messages raised during the build process containing an
 * error, a warning, or simply a notification of something. All build msg
 * classes must derive from this class. This class provides a set of abstract
 * functions for obtaining the details of the msg including code, description,
 * severity, and source code location.
 */
class BuildMsg : public IdentifiableObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(BuildMsg, IdentifiableObject, "Core.Processing", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  /**
   * @brief The source file for which the message is generated.
   * This is the full path and filename of the source file related to this
   * message.
   */
  private: Str filename;

  /**
   * @brief The number of the line at which the message was raised.
   * This line number refers to the line within the source file at which the
   * first character in the text appeared. The lines should be numbered from
   * 1, not 0.
   */
  private: Int line;

  /**
   * @brief The number of the column at which the message was raised.
   * This column number refers to the column within the source code at which
   * the first character in the text appeared. The columns should be numbered
   * from 1, not 0.
   */
  private: Int column;

  /**
   * @brief A temporary buffer to hold the generated message description.
   * @sa getDescription()
   */
  private: static Str tempBuf;


  //============================================================================
  // Constructor / Destructor

  public: BuildMsg() : line(0), column(0)
  {
  }

  public: BuildMsg(Int l, Int c) : line(l), column(c)
  {
  }

  public: BuildMsg(Int l, Int c, Char const *fn) : line(l), column(c), filename(fn)
  {
  }

  public: virtual ~BuildMsg()
  {
  }


  //============================================================================
  // Member Functions

  /**
   * @brief Get the message code.
   *
   * The message code is any code that can uniquely identify the message.
   * Ideally this consists of a prefix character (or group of characters)
   * identifying the unit that generated the message (for example L for Lexer
   * and P for Parser) followed by a number identifying the message
   * within that unit. For example, a parsing error code can have a value of
   * P23.
   */
  public: virtual const Str& getCode() const = 0;

  /**
   * @brief Get the severity of the message.
   *
   * The severity specifies how safe it is to ignore the message. The lower
   * the value, the less safe it is, with the value of 0 meaning it's a
   * blocker error. The following are the possible values:
   *
   * 0: This is a blocker error. Compilation can't continue, or results are
   *    unpredictable if compilation continues.<br>
   * 1: This is an error, but compilation can continue.<br>
   * 2: This is a critical warning, user should pay attention to it.<br>
   * 3: This is a warning that the user can ignore.<br>
   * 4: This is just a notification, no user action is required.
   * The default implementation returns 0.
   */
  public: virtual Int getSeverity() const
  {
    return 0;
  }

  /**
   * @brief Build a human readable description of the message.
   *
   * The description does not include the source code location associated with
   * the message (filename, line number, etc).
   *
   * @param str A reference to a string that will receive the text.
   */
  public: virtual void buildDescription(Str &str) const = 0;

  /// Get a human readable description of the message.
  public: const Str& getDescription() const;

  /**
   * @brief Set the source file for which the message is generated.
   *
   * This is the full path and filename of the source file associated with the
   * message.
   */
  public: void setFileName(Char const *fn)
  {
    this->filename = fn;
  }

  /**
   * @brief Get the source file for which the message is generated.
   *
   * This is the full path and filename of the source file associated with the
   * message.
   */
  public: const Str& getFileName()
  {
    return this->filename;
  }

  /**
   * @brief Set the source code line number associated with the message.
   *
   * This number is the line number of the first character in the source code
   * text related to the message.
   *
   * @param l The line number of the first character in the source text. Line
   *          numbering starts from 1, not 0.
   */
  public: void setLine(Int l)
  {
    this->line = l;
  }

  /**
   * @brief Get the source code line number associated with the message.
   *
   * This number is the line number of the first character in the source code
   * text related to the message.
   *
   * @return The line number of the first character in the source text. Line
   *         numbering starts from 1, not 0.
   */
  public: Int getLine() const
  {
    return this->line;
  }

  /**
   * @brief Set the source code column number associated with the message.
   *
   * This number is the column number of the first character in the source
   * code text related to the message.
   *
   * @param c The column number of the first character in the source text.
   *          Column numbering starts from 1, not 0.
   */
  public: void setColumn(Int c)
  {
    this->column = c;
  }

  /**
   * @brief Get the source code column number associated with the message.
   *
   * This number is the column number of the first character in the source
   * code text related to the message.
   *
   * @return The column number of the first character in the source text.
   *         Column numbering starts from 1, not 0.
   */
  public: Int getColumn() const
  {
    return this->column;
  }

}; // class

} } // namespace

#endif
