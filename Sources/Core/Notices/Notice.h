/**
 * @file Core/Notices/Notice.h
 * Contains the header of class Core::Notices::Notice.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_NOTICES_NOTICE_H
#define CORE_NOTICES_NOTICE_H

namespace Core::Notices
{

/**
 * @brief The base of all notifications.
 * @ingroup core_notices
 *
 * Notices are messages raised during the build process containing an
 * error, a warning, or simply a notification of something. All notice
 * classes must derive from this class. This class provides a set of abstract
 * functions for obtaining the details of the msg including code, description,
 * severity, and source code location.
 */
class Notice : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(Notice, TiObject, "Core.Notices", "Core", "alusus.org");


  //============================================================================
  // Member Variables

  /// The source location at which the message was generated.
  private: SharedPtr<Data::SourceLocation> sourceLocation;


  //============================================================================
  // Constructor / Destructor

  public: Notice()
  {
  }

  public: Notice(SharedPtr<Data::SourceLocation> const &sl) : sourceLocation(sl)
  {
  }

  public: virtual ~Notice()
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
  public: virtual Str const& getCode() const = 0;

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
  public: Str const& getDescription() const
  {
    static thread_local Str tempBuf;
    this->buildDescription(tempBuf);
    return tempBuf;
  }

  /// Set the source location at which the message was generated.
  public: void setSourceLocation(SharedPtr<Data::SourceLocation> const &sl)
  {
    this->sourceLocation = sl;
  }

  /// Get the source location at which the message was generated.
  public: SharedPtr<Data::SourceLocation> const& getSourceLocation() const
  {
    return this->sourceLocation;
  }

}; // class

} // namespace

#endif
