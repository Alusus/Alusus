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

  public: String(Char const *v) : value(v)
  {
  }

  public: String(Char const *v, Word c) : value(v, c)
  {
  }

  public: static SharedPtr<String> create(Char const *v)
  {
    return std::make_shared<String>(v);
  }

  public: static SharedPtr<String> create(Char const *v, Word c)
  {
    return std::make_shared<String>(v, c);
  }


  //============================================================================
  // Operators

  public: String& operator=(Char const *v)
  {
    this->value = v;
    return *this;
  }

  public: operator Char const*() const
  {
    return this->value.c_str();
  }


  //============================================================================
  // Member Functions

  public: void set(Char const *v)
  {
    this->value = v;
  }

  public: void set(Char const *v, Word c)
  {
    this->value.assign(v, c);
  }

  public: Char const* get() const
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
