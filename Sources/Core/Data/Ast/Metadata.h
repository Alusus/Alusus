/**
 * @file Core/Data/Ast/Metadata.h
 * Contains the header of interface Data::Ast::Metadata.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_METADATA_H
#define CORE_DATA_AST_METADATA_H

namespace Core { namespace Data { namespace Ast
{

// TODO: DOC

class Metadata : public TiInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(Metadata, TiInterface, "Core.Data.Ast", "Core", "alusus.net");


  //============================================================================
  // Abstract Functions

  /**
   * @brief Set the production id this list refers to.
   *
   * This value refers to the id of the production definition this element
   * represents. If this value is 0, then the element is an inner term, not
   * a production root.
   */
  public: virtual void setProdId(Word id) = 0;
  public: virtual void setProdId(TiWord const *id)
  {
    this->setProdId(id == 0 ? UNKNOWN_ID : id->get());
  }

  /**
   * @brief Get the production id this list refers to.
   *
   * This value refers to the id of the production definition this element
   * represents. If this value is 0, then the element is an inner term, not
   * a production root.
   */
  public: virtual TiWord& getProdId() = 0;
  public: virtual TiWord const& getProdId() const = 0;

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
  public: virtual void setSourceLocation(SourceLocation const &loc) = 0;
  public: virtual void setSourceLocation(SourceLocation const *loc)
  {
    this->setSourceLocation(loc == 0 ? SourceLocation(SharedPtr<Str>(), 0, 0) : *loc);
  }

  public: virtual SourceLocation& getThisSourceLocation() = 0;
  public: virtual SourceLocation const& getThisSourceLocation() const = 0;

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
    SourceLocation const &sl = this->getThisSourceLocation();
    if (sl.line == 0) {
      Container const *container = this->getTiObject()->getInterface<Container const>();
      if (container != 0) {
        for (Int i = 0; i < container->getCount(); ++i) {
          Metadata *ptr = ti_cast<Metadata>(container->get(i));
          if (ptr != 0) {
            SourceLocation const &sl2 = ptr->getSourceLocation();
            if (sl2.line != 0) return sl2;
          }
        }
      }
    }
    return sl;
  }

}; // class


//==============================================================================
// Macros

#define IMPLEMENT_METADATA(type) \
  private: Core::Basic::TiWord prodId = UNKNOWN_ID; \
  private: Core::Data::SourceLocation sourceLocation; \
  public: using Metadata::setProdId; \
  public: virtual void setProdId(Word id) \
  { \
    this->prodId = id; \
  } \
  public: virtual Core::Basic::TiWord& getProdId() \
  { \
    return this->prodId; \
  } \
  public: virtual Core::Basic::TiWord const& getProdId() const \
  { \
    return this->prodId; \
  } \
  public: using Metadata::setSourceLocation; \
  public: virtual void setSourceLocation(Core::Data::SourceLocation const &loc) \
  { \
    this->sourceLocation = loc; \
  } \
  public: virtual Core::Data::SourceLocation& getThisSourceLocation() \
  { \
    return this->sourceLocation; \
  } \
  public: virtual Core::Data::SourceLocation const& getThisSourceLocation() const \
  { \
    return this->sourceLocation; \
  }

} } } // namespace

#endif
