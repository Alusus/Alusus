/**
 * @file Core/Processing/BufferFullMsg.h
 * Contains the header of class Core::Processing::BufferFullMsg.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_BUFFERFULLMSG_H
#define CORE_PROCESSING_BUFFERFULLMSG_H

namespace Core { namespace Processing
{

/**
 * @brief A build message for the "Buffer Full" error.
 * @ingroup processing_lexer
 *
 * This message class is for error code L1002, which is raised when the
 * lexer's internal buffer gets full. This may cause a token to be broken into
 * two.
 */
class BufferFullMsg : public Processing::BuildMsg
{
  //============================================================================
  // Type Info

  TYPE_INFO(BufferFullMsg, Processing::BuildMsg, "Core.Processing", "Core", "alusus.net");


  //============================================================================
  // Constructor / Destructor

  public: BufferFullMsg(Data::SourceLocation const &sl) : Processing::BuildMsg(sl)
  {
  }

  public: virtual ~BufferFullMsg()
  {
  }


  //============================================================================
  // Member Functions

  /// @sa Processing::BuildMsg::getCode()
  public: virtual Str const& getCode() const
  {
    static Str code("L1002");
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
    str = STR("Lexer Error: Input buffer is full. A single token is too long to fit in the input buffer. "
              "The token may have been broken into more than one token.");
  }

}; // class

} } // namespace

#endif
