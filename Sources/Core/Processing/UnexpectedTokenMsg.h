/**
 * @file Core/Processing/UnexpectedTokenMsg.h
 * Contains the header of class Core::Processing::UnexpectedTokenMsg.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef PROCESSING_UNEXPECTEDTOKENMSG_H
#define PROCESSING_UNEXPECTEDTOKENMSG_H

namespace Core { namespace Processing
{

/**
 * @brief A build message for unexpected token error.
 * @ingroup processing_parser
 *
 * This message class is for error code P2001, which is raised when the parser
 * unexpectedly receives a new token when it has already folded out of the
 * grammar tree (at which point it expects and endParsing call instead).
 */
class UnexpectedTokenMsg : public Processing::BuildMsg
{
  //============================================================================
  // Type Info

  TYPE_INFO(UnexpectedTokenMsg, Processing::BuildMsg, "Core.Parser", "Core", "alusus.net");


  //============================================================================
  // Constructor / Destructor

  public: UnexpectedTokenMsg(Int l, Int c) : Processing::BuildMsg(l, c)
  {
  }

  public: virtual ~UnexpectedTokenMsg()
  {
  }


  //============================================================================
  // Member Functions

  /// @sa Processing::BuildMsg::getCode()
  public: virtual Str const& getCode() const
  {
    static Str code("P1003");
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
    str = STR("Parsing has already folded out to completion.");
  }

}; // class

} } // namespace

#endif
