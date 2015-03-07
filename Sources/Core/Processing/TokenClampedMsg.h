/**
 * @file Core/Processing/TokenClampedMsg.h
 * Contains the header of class Core::Processing::TokenClampedMsg.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef PROCESSING_TOKENCLAMPEDMSG_H
#define PROCESSING_TOKENCLAMPEDMSG_H

namespace Core { namespace Processing
{

/**
 * @brief A build message for the "Token Clamped" warning.
 * @ingroup processing_lexer
 *
 * This message class is for warning code L2001, which is raised when the
 * lexer clamps a very long token to fit into the input buffer.
 */
class TokenClampedMsg : public BuildMsg
{
  //============================================================================
  // Type Info

  TYPE_INFO(TokenClampedMsg, Processing::BuildMsg, "Core.Processing", "Core", "alusus.net");


  //============================================================================
  // Constructor / Destructor

  public: TokenClampedMsg(Data::SourceLocation const &sl) : BuildMsg(sl)
  {
  }

  public: virtual ~TokenClampedMsg()
  {
  }


  //============================================================================
  // Member Functions

  /// @sa Processing::BuildMsg::getCode()
  public: virtual Str const& getCode() const
  {
    static Str code("L2001");
    return code;
  }

  /// @sa Processing::BuildMsg::getSeverity()
  public: virtual Int getSeverity() const
  {
    return 2;
  }

  /// @sa Processing::BuildMsg::buildDescription()
  public: virtual void buildDescription(Str &str) const
  {
    str = STR("Warning: Input buffer is full. A single token is too long to fit in the input buffer. "
              "Some characters that are part of the token has been ignored.");
  }

}; // class

} } // namespace

#endif
