/**
 * @file Core/Data/Ast/MetadataHolder.h
 * Contains the header of interface Data::Ast::MetadataHolder.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_METADATAHOLDER_H
#define CORE_DATA_AST_METADATAHOLDER_H

namespace Core { namespace Data { namespace Ast
{

// TODO: DOC

class MetadataHolder : public AttributesHolder
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(MetadataHolder, AttributesHolder, "Core.Data.Ast", "Core", "alusus.net");


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

  public: MetadataHolder() : prodId(UNKNOWN_ID)
  {
  }

  public: MetadataHolder(Word pid) : prodId(pid)
  {
  }

  public: MetadataHolder(Word pid, SourceLocation const &loc) : prodId(pid), sourceLocation(loc)
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
    if (this->sourceLocation.line == 0) {
      Container const *container = this->getTiObject()->getInterface<Container const>();
      if (container != 0) {
        for (Int i = 0; i < container->getCount(); ++i) {
          MetadataHolder *ptr = tii_cast<MetadataHolder>(container->get(i));
          if (ptr != 0) {
            SourceLocation const &sl = ptr->getSourceLocation();
            if (sl.line != 0) return sl;
          }
        }
      }
    }
    return this->sourceLocation;
  }

  public: virtual TiObject* getAttribute(Char const *name)
  {
    if (SBSTR(name) == STR("prodId")) {
      return &this->prodId;
    } else if (SBSTR(name) == STR("sourceLocation")) {
      if (this->sourceLocation.line == 0) {
        Container *container = this->getTiObject()->getInterface<Container>();
        if (container != 0) {
          for (Int i = 0; i < container->getCount(); ++i) {
            MetadataHolder *ptr = tii_cast<MetadataHolder>(container->get(i));
            if (ptr != 0) {
              if (ptr->getSourceLocation().line != 0) return ptr->getAttribute(name);
            }
          }
        }
      }

      return &this->sourceLocation;
    } else {
      return 0;
    }
  }

}; // class

} } } // namespace

#endif
