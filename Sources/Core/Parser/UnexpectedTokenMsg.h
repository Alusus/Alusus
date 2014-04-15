/**
 * @file Core/Parser/UnexpectedTokenMsg.h
 * Contains the header of class Core::Parser::UnexpectedTokenMsg.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef PARSER_UNEXPECTED_TOKEN_MSG_H
#define PARSER_UNEXPECTED_TOKEN_MSG_H

namespace Core { namespace Parser
{

/**
 * @brief A build message for unexpected token error.
 * @ingroup parser
 *
 * This message class is for error code P2001, which is raised when the parser
 * unexpectedly receives a new token when it has already folded out of the
 * grammar tree (at which point it expects and endParsing call instead).
 */
class UnexpectedTokenMsg : public Common::BuildMsg
{
  //============================================================================
  // Type Info

  TYPE_INFO(UnexpectedTokenMsg, Common::BuildMsg, "Core.Parser", "Core", "alusus.net");


  //============================================================================
  // Constructor / Destructor

  public: UnexpectedTokenMsg(Int l, Int c) : Common::BuildMsg(l, c)
  {
  }

  public: virtual ~UnexpectedTokenMsg()
  {
  }


  //============================================================================
  // Member Functions

  /// @sa Common::BuildMsg::getCode()
  public: virtual const Str& getCode() const
  {
    static Str code("P1003");
    return code;
  }

  /// @sa Common::BuildMsg::getSeverity()
  public: virtual Int getSeverity() const
  {
    return 1;
  }

  /// @sa Common::BuildMsg::getCode()
  public: virtual void buildDescription(Str &str) const
  {
    str = STR("Parsing has already folded out to completion.");
  }

}; // class

} } // namespace

#endif
