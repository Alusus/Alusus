/**
 * @file Core/Parser/AmbiguityMsg.h
 * Contains the header of class Core::Parser::AmbiguityMsg.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef PARSER_AMBIGUITY_MSG_H
#define PARSER_AMBIGUITY_MSG_H

namespace Core { namespace Parser
{

// TODO: DOC

class AmbiguityMsg : public Common::BuildMsg
{
  //============================================================================
  // Type Info

  TYPE_INFO(AmbiguityMsg, Common::BuildMsg, "Core.Parser", "Core", "alusus.net");


  //============================================================================
  // Constructor / Destructor

  public: AmbiguityMsg(Int l, Int c) : Common::BuildMsg(l, c)
  {
  }

  public: virtual ~AmbiguityMsg()
  {
  }


  //============================================================================
  // Member Functions

  /// @sa Common::BuildMsg::getCode()
  public: virtual const Str& getCode() const
  {
    static Str code("P2001");
    return code;
  }

  /// @sa Common::BuildMsg::getSeverity()
  public: virtual Int getSeverity() const
  {
    return 2;
  }

  /// @sa Common::BuildMsg::getCode()
  public: virtual void buildDescription(Str &str) const
  {
    str = STR("Ambiguity is causing state branching.");
  }

}; // class

} } // namespace

#endif
