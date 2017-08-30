/**
 * @file Core/Standard/RedefinitionNotice.h
 * Contains the header of class Core::Processing::RedefinitionNotice.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_STANDARD_REDEFINITIONNOTICE_H
#define CORE_STANDARD_REDEFINITIONNOTICE_H

namespace Core { namespace Standard
{

// TODO: DOC

class RedefinitionNotice : public Data::Notice
{
  //============================================================================
  // Type Info

  TYPE_INFO(RedefinitionNotice, Data::Notice, "Core.Standard", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: Str name;


  //============================================================================
  // Constructor / Destructor

  public: RedefinitionNotice(Char const *n, Data::SourceLocation const &sl) :
    name(n), Data::Notice(sl)
  {
  }

  public: virtual ~RedefinitionNotice()
  {
  }


  //============================================================================
  // Member Functions

  /// @sa Data::Notice::getCode()
  public: virtual Str const& getCode() const
  {
    static Str code("S1001");
    return code;
  }

  /// @sa Data::Notice::getSeverity()
  public: virtual Int getSeverity() const
  {
    return 1;
  }

  /// Get the name of the duplicate definition.
  public: Str const& getDefinitionName() const
  {
    return this->name;
  }

  /// @sa Data::Notice::getCode()
  public: virtual void buildDescription(Str &str) const
  {
    str = STR("Duplicate definition. [");
    str += this->name;
    str += "]";
  }

}; // class

} } // namespace

#endif
