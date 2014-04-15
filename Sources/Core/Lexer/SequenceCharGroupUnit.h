/**
 * @file Core/Lexer/SequenceCharGroupUnit.h
 * Contains the header of class Core::Lexer::SequenceCharGroupUnit.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef LEXER_SEQUENCE_CHAR_GROUP_UNIT_H
#define LEXER_SEQUENCE_CHAR_GROUP_UNIT_H

namespace Core { namespace Lexer
{

/**
 * @brief A sequential character group unit.
 * @ingroup lexer_char_group_units
 *
 * Represents the sequence of characters between two given character codes
 * inclusive.
 */
class SequenceCharGroupUnit : public CharGroupUnit
{
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

  public: SequenceCharGroupUnit(Char s, Char e) : startCode(s), endCode(e)
  {
    if (e < s) {
      throw InvalidArgumentException(STR("s,e"), STR("Core::Lexer::SequenceCharGroupUnit::SequenceCharGroupUnit"),
                                     STR("e should be >= s."));
    }
  }

  public: virtual ~SequenceCharGroupUnit()
  {
  }


  //============================================================================
  // Member Functions

  /// Get the type identifier of this object.
  public: virtual CharGroupUnitType getType() const
  {
    return CharGroupUnitType::SEQUENCE;
  }

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
