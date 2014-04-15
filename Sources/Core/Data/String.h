/**
 * @file Core/Data/String.h
 * Contains the header of class Core::Data::String.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_STRING_H
#define DATA_STRING_H

namespace Core { namespace Data
{

/**
 * @brief An identifiable object that holds a string value.
 * @ingroup data
 *
 * This IdentifiableObject derived class is used to hold string data used in
 * the grammar. This is simply a wrappar around Str that provides runtime type
 * info, which is needed for grammar data.
 */
class String : public IdentifiableObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(String, IdentifiableObject, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: Str value;


  //============================================================================
  // Constructors

  public: String()
  {
  }

  public: String(const Char *v) : value(v)
  {
  }

  public: String(const Char *v, Word c) : value(v, c)
  {
  }


  //============================================================================
  // Operators

  public: String& operator=(const Char *v)
  {
    this->value = v;
    return *this;
  }

  public: operator const Char*() const
  {
    return this->value.c_str();
  }


  //============================================================================
  // Member Functions

  public: void set(const Char *v)
  {
    this->value = v;
  }

  public: void set(const Char *v, Word c)
  {
    this->value.assign(v, c);
  }

  public: const Char* get() const
  {
    return this->value.c_str();
  }

  public: const Str& getStr() const
  {
    return this->value;
  }

}; // class

} } // namespace

#endif
