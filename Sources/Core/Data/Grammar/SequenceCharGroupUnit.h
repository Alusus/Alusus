/**
 * @file Core/Data/Grammar/SequenceCharGroupUnit.h
 * Contains the header of class Core::Data::Grammar::SequenceCharGroupUnit.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_GRAMMAR_SEQUENCECHARGROUPUNIT_H
#define CORE_DATA_GRAMMAR_SEQUENCECHARGROUPUNIT_H

namespace Core::Data::Grammar
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

  TYPE_INFO(SequenceCharGroupUnit, CharGroupUnit, "Core.Data.Grammar", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  /// The character code of the start character in the sequence.
  private: WChar startCode;

  /// The character code of the end character in the sequence.
  private: WChar endCode;


  //============================================================================
  // Constructor / Destructor

  public: SequenceCharGroupUnit() : startCode(0), endCode(0)
  {
  }

  public: SequenceCharGroupUnit(Char const *s, Char const *e)
  {
    this->setStartCode(s);
    this->setEndCode(e);
    if (this->endCode < this->startCode) {
      throw EXCEPTION(InvalidArgumentException, S("s,e"), S("e should be >= s."));
    }
  }

  public: virtual ~SequenceCharGroupUnit()
  {
  }

  public: static SharedPtr<SequenceCharGroupUnit> create(Char const *s, Char const *e)
  {
    return std::make_shared<SequenceCharGroupUnit>(s, e);
  }


  //============================================================================
  // Member Functions

  /// Set the code of the start of the sequence.
  public: void setStartCode(Char const *s)
  {
    this->startCode = getWideCharFromUtf8(s);
  }

  /// Get the code of the start of the sequence.
  public: WChar getStartCode()
  {
    return this->startCode;
  }

  /// Set the code of the end of the sequence.
  public: void setEndCode(Char const *e)
  {
    this->endCode = getWideCharFromUtf8(e);
  }

  /// Get the code of the end of the sequence.
  public: WChar getEndCode()
  {
    return this->endCode;
  }

}; // class

} // namespace

#endif
