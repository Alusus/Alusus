/**
 * @file Core/Lexer/TokenClampedMsg.h
 * Contains the header of class Core::Lexer::TokenClampedMsg.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef LEXER_TOKEN_CLAMPED_MSG_H
#define LEXER_TOKEN_CLAMPED_MSG_H

namespace Core { namespace Lexer
{

/**
 * @brief A build message for the "Token Clamped" warning.
 * @ingroup lexer
 *
 * This message class is for warning code L2001, which is raised when the
 * lexer clamps a very long token to fit into the input buffer.
 */
class TokenClampedMsg : public Common::BuildMsg
{
  //============================================================================
  // Type Info

  TYPE_INFO(TokenClampedMsg, Common::BuildMsg, "Core.Lexer", "Core", "alusus.net");


  //============================================================================
  // Constructor / Destructor

  public: TokenClampedMsg(Int l, Int c) : Common::BuildMsg(l, c)
  {
  }

  public: virtual ~TokenClampedMsg()
  {
  }


  //============================================================================
  // Member Functions

  /// @sa Common::BuildMsg::getCode()
  public: virtual const Str& getCode() const
  {
    static Str code("L2001");
    return code;
  }

  /// @sa Common::BuildMsg::getSeverity()
  public: virtual Int getSeverity() const
  {
    return 2;
  }

  /// @sa Common::BuildMsg::buildDescription()
  public: virtual void buildDescription(Str &str) const
  {
    str = STR("Warning: Input buffer is full. A single token is too long to fit in the input buffer. "
              "Some characters that are part of the token has been ignored.");
  }

}; // class

} } // namespace

#endif
