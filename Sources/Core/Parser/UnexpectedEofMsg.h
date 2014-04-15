/**
 * @file Core/Parser/UnexpectedEofMsg.h
 * Contains the header of class Core::Parser::UnexpectedEofMsg.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef PARSER_UNEXPECTED_EOF_MSG_H
#define PARSER_UNEXPECTED_EOF_MSG_H

namespace Core { namespace Parser
{

/**
 * @brief A build message for unexpected eof error.
 * @ingroup parser
 *
 * This message class is for error code P1002, which is raised when the parser
 * unexpectedly receives an endParsing call while still expecting more tokens.
 */
class UnexpectedEofMsg : public Common::BuildMsg
{
  //============================================================================
  // Type Info

  TYPE_INFO(UnexpectedEofMsg, Common::BuildMsg, "Core.Parser", "Core", "alusus.net");


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

  /// @sa Common::BuildMsg::getCode()
  public: virtual const Str& getCode() const
  {
    static Str code("P1002");
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
    str = STR("Parsing exited while needing more tokens.");
  }

}; // class

} } // namespace

#endif
