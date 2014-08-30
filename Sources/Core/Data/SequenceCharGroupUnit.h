/**
 * @file Core/Data/SequenceCharGroupUnit.h
 * Contains the header of class Core::Data::SequenceCharGroupUnit.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_SEQUENCECHARGROUPUNIT_H
#define DATA_SEQUENCECHARGROUPUNIT_H

namespace Core { namespace Data
{

/**
 * @brief A sequential character group unit.
 * @ingroup data_char_group_units
 *
 * Represents the sequence of characters between two given character codes
 * inclusive.
 */
class SequenceCharGroupUnit : public CharGroupUnit
{
  //============================================================================
  // Type Info

  TYPE_INFO(SequenceCharGroupUnit, CharGroupUnit, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  /// The character code of the start character in the sequence.
  private: Char startCode;

  /// The character code of the end character in the sequence.
  private: Char endCode;


  //============================================================================
  // Constructor / Destructor

  public: SequenceCharGroupUnit() : startCode(0), endCode(0)
  {
  }

  public: SequenceCharGroupUnit(Char s=0, Char e=0) : startCode(s), endCode(e)
  {
    if (e < s) {
      throw InvalidArgumentException(STR("s,e"), STR("Core::Data::SequenceCharGroupUnit::SequenceCharGroupUnit"),
                                     STR("e should be >= s."));
    }
  }

  public: virtual ~SequenceCharGroupUnit()
  {
  }

  public: static SharedPtr<SequenceCharGroupUnit> create(Char s=0, Char e=0)
  {
    return std::make_shared<SequenceCharGroupUnit>(s, e);
  }


  //============================================================================
  // Member Functions

  /// Set the code of the start of the sequence.
  public: void setStartCode(Char s)
  {
    this->startCode = s;
  }

  /// Get the code of the start of the sequence.
  public: Char getStartCode()
  {
    return this->startCode;
  }

  /// Set the code of the end of the sequence.
  public: void setEndCode(Char e)
  {
    this->endCode = e;
  }

  /// Get the code of the end of the sequence.
  public: Char getEndCode()
  {
    return this->endCode;
  }

}; // class

} } // namespace

#endif
