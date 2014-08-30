/**
 * @file Core/Data/Integer.h
 * Contains the header of class Core::Data::Integer.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_NUMBER_H
#define DATA_NUMBER_H

namespace Core { namespace Data
{

/**
 * @brief An identifiable object that holds an integer number.
 * @ingroup data
 *
 * This IdentifiableObject derived class is used to hold integer data used in
 * the grammar. This is simply a wrappar around Int that provides runtime type
 * info, which is needed for grammar data.
 */
class Integer : public IdentifiableObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(Integer, IdentifiableObject, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: Int value;


  //============================================================================
  // Constructor

  public: Integer(Int v=0) : value(v)
  {
  }

  public: static SharedPtr<Integer> create(Int v)
  {
    return std::make_shared<Integer>(v);
  }


  //============================================================================
  // Operators

  public: Integer& operator=(Int v)
  {
    this->value = v;
    return *this;
  }

  public: operator Int() const
  {
    return this->value;
  }


  //============================================================================
  // Member Functions

  public: void set(Int v)
  {
    this->value = v;
  }

  public: Int get() const
  {
    return this->value;
  }

}; // class

} } // namespace

#endif
