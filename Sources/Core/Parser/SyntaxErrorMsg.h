/**
 * @file Core/Parser/SyntaxErrorMsg.h
 * Contains the header of class Core::Parser::SyntaxErrorMsg.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef PARSER_SYNTAX_ERROR_MSG_H
#define PARSER_SYNTAX_ERROR_MSG_H

namespace Core { namespace Parser
{

/**
 * @brief A build message for Syntax Error.
 * @ingroup parser
 *
 * This message class is for error code P1001, which is raised when the parser
 * is faced with a token that is not recognized by the grammar.
 */
class SyntaxErrorMsg : public Common::BuildMsg
{
  //============================================================================
  // Type Info

  TYPE_INFO(SyntaxErrorMsg, Common::BuildMsg, "Core.Parser", "Core", "alusus.net");


  //============================================================================
  // Constructor / Destructor

  public: SyntaxErrorMsg(Int l, Int c) : Common::BuildMsg(l, c)
  {
  }

  public: virtual ~SyntaxErrorMsg()
  {
  }


  //============================================================================
  // Member Functions

  /// @sa Common::BuildMsg::getCode()
  public: virtual const Str& getCode() const
  {
    static Str code("P1001");
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
    str = STR("Parser Syntax Error.");
  }

}; // class

} } // namespace

#endif
