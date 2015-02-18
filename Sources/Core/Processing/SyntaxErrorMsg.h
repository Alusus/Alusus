/**
 * @file Core/Processing/SyntaxErrorMsg.h
 * Contains the header of class Core::Processing::SyntaxErrorMsg.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef PROCESSING_SYNTAXERRORMSG_H
#define PROCESSING_SYNTAXERRORMSG_H

namespace Core { namespace Processing
{

/**
 * @brief A build message for Syntax Error.
 * @ingroup processing_parser
 *
 * This message class is for error code P1001, which is raised when the parser
 * is faced with a token that is not recognized by the grammar.
 */
class SyntaxErrorMsg : public Processing::BuildMsg
{
  //============================================================================
  // Type Info

  TYPE_INFO(SyntaxErrorMsg, Processing::BuildMsg, "Core.Parser", "Core", "alusus.net");


  //============================================================================
  // Constructor / Destructor

  public: SyntaxErrorMsg(Int l, Int c) : Processing::BuildMsg(l, c)
  {
  }

  public: virtual ~SyntaxErrorMsg()
  {
  }


  //============================================================================
  // Member Functions

  /// @sa Processing::BuildMsg::getCode()
  public: virtual Str const& getCode() const
  {
    static Str code("P1001");
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
    str = STR("Parser Syntax Error.");
  }

}; // class

} } // namespace

#endif
