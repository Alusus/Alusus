/**
 * @file Core/Data/Integer.h
 * Contains the header of class Core::Data::Integer.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_INTEGER_H
#define CORE_DATA_INTEGER_H

namespace Core { namespace Data
{

/**
 * @brief An identifiable object that holds an integer number.
 * @ingroup data
 *
 * This TiObject derived class is used to hold integer data used in
 * the grammar. This is simply a wrappar around Int that provides runtime type
 * info, which is needed for grammar data.
 */
class Integer : public Node
{
  //============================================================================
  // Type Info

  TYPE_INFO(Integer, Node, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: Int value;


  //============================================================================
  // Constructor

  public: Integer(Int v=0) : value(v)
  {
  }

  public: Integer(Word v) : value(static_cast<Int>(v))
  {
  }

  public: static SharedPtr<Integer> create(Int v)
  {
    return std::make_shared<Integer>(v);
  }

  public: static SharedPtr<Integer> create(Word v)
  {
    return std::make_shared<Integer>(static_cast<Int>(v));
  }


  //============================================================================
  // Operators

  public: Integer& operator=(Int v)
  {
    this->value = v;
    return *this;
  }

  public: Integer& operator=(Word v)
  {
    this->value = static_cast<Int>(v);
    return *this;
  }

  public: operator Int() const
  {
    return this->value;
  }

  public: operator Word() const
  {
    return static_cast<Word>(this->value);
  }

  //============================================================================
  // Member Functions

  public: void set(Int v)
  {
    this->value = v;
  }

  public: void setUnsigned(Word v)
  {
    this->value = static_cast<Int>(v);
  }

  public: Int get() const
  {
    return this->value;
  }

  public: Word getUnsigned() const
  {
    return static_cast<Word>(this->value);
  }

}; // class

} } // namespace

#endif
