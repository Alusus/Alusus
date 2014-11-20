/**
 * @file Core/Processing/UnexpectedEofMsg.h
 * Contains the header of class Core::Processing::UnexpectedEofMsg.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef PROCESSING_UNEXPECTEDEOFMSG_H
#define PROCESSING_UNEXPECTEDEOFMSG_H

namespace Core { namespace Processing
{

/**
 * @brief A build message for unexpected eof error.
 * @ingroup processing
 *
 * This message class is for error code P1002, which is raised when the parser
 * unexpectedly receives an endParsing call while still expecting more tokens.
 */
class UnexpectedEofMsg : public Processing::BuildMsg
{
  //============================================================================
  // Type Info

  TYPE_INFO(UnexpectedEofMsg, Processing::BuildMsg, "Core.Parser", "Core", "alusus.net");


  //============================================================================
  // Constructor / Destructor

  public: UnexpectedEofMsg()
  {
  }

  public: virtual ~UnexpectedEofMsg()
  {
  }


  //============================================================================
  // Member Functions

  /// @sa Processing::BuildMsg::getCode()
  public: virtual Str const& getCode() const
  {
    static Str code("P1002");
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
    str = STR("Parsing exited while needing more tokens.");
  }

}; // class

} } // namespace

#endif
