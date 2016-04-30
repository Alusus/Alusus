/**
 * @file Core/Data/ParsingMetadataHolder.h
 * Contains the header of interface Data::ParsingMetadataHolder.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_PARSINGMETADATAHOLDER_H
#define CORE_DATA_PARSINGMETADATAHOLDER_H

namespace Core { namespace Data
{

// TODO: DOC

class ParsingMetadataHolder : public AttributesHolder
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(ParsingMetadataHolder, AttributesHolder, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  /**
   * @brief The production id this item refers to.
   *
   * This value refers to the id of the production definition this item
   * represents. If this value is UNKNOWN_ID, then the item is an
   * inner term, not a production root.
   */
  private: Integer prodId;

  /**
   * @brief The location at which the token appeared in the source.
   *
   * Although this refers to the token, it actually applies to all item types
   * since an error reporter needs to report a source code location no matter
   * at which item the error was detected. In the case of non-token items,
   * this refers to the location of the first token detected inside that term.
   */
  private: SourceLocation sourceLocation;


  //============================================================================
  // Constructors

  public: ParsingMetadataHolder() : prodId(UNKNOWN_ID)
  {
  }

  public: ParsingMetadataHolder(Int pid) : prodId(pid)
  {
  }

  public: ParsingMetadataHolder(Int pid, SourceLocation const &loc) : prodId(pid), sourceLocation(loc)
  {
  }


  //============================================================================
  // Abstract Functions

  /**
   * @brief Set the production id this list refers to.
   *
   * This value refers to the id of the production definition this element
   * represents. If this value is 0, then the element is an inner term, not
   * a production root.
   */
  public: virtual void setProdId(Word id)
  {
    this->prodId = id;
  }

  /**
   * @brief Get the production id this list refers to.
   *
   * This value refers to the id of the production definition this element
   * represents. If this value is 0, then the element is an inner term, not
   * a production root.
   */
  public: virtual Word getProdId() const
  {
    return this->prodId;
  }

  /**
   * @brief Set the token's location within the source code.
   *
   * Set the location at which the token appeared in the source code. This
   * value refers to the location of the first character in the token.
   * Although this refers to the token, it actually applies to all item types
   * since an error reporter needs to report a source code location no matter
   * at which item the error was detected. In the case of non-token items,
   * this refers to the location of the first token detected inside that term.
   */
  public: virtual void setSourceLocation(SourceLocation const &loc)
  {
    this->sourceLocation = loc;
  }

  /**
   * @brief Get the token's locaiton within the source code.
   *
   * Get the location at which the token appeared in the source code. This
   * value refers to the location of the first character in the token.
   * Although this refers to the token, it actually applies to all item types
   * since an error reporter needs to report a source code location no matter
   * at which item the error was detected. In the case of non-token items,
   * this refers to the location of the first token detected inside that term.
   *
   * @return The line number of the first character in the token.
   */
  public: virtual SourceLocation const& getSourceLocation() const
  {
    return this->sourceLocation;
  }

  public: virtual IdentifiableObject* getAttribute(Char const *name)
  {
    if (SBSTR(name) == STR("prodId")) {
      return &this->prodId;
    } else if (SBSTR(name) == STR("sourceLocation")) {
      return &this->sourceLocation;
    } else {
      return 0;
    }
  }

}; // class

} } // namespace

#endif
