/**
 * @file Core/Data/Ast/MetaHaving.h
 * Contains the header of interface Data::Ast::MetaHaving.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_METAHAVING_H
#define CORE_DATA_AST_METAHAVING_H

namespace Core::Data::Ast
{

// TODO: DOC

class MetaHaving : public TiInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(MetaHaving, TiInterface, "Core.Data.Ast", "Core", "alusus.org");


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
  public: virtual void setSourceLocation(SharedPtr<SourceLocation> const &loc) = 0;
  public: virtual void setSourceLocation(SourceLocation *loc)
  {
    this->setSourceLocation(getSharedPtr(loc));
  }

  public: virtual SharedPtr<SourceLocation> const& getSourceLocation() const = 0;

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
  public: virtual SharedPtr<SourceLocation> const& findSourceLocation() const
  {
    SharedPtr<SourceLocation> const &sl = this->getSourceLocation();
    if (sl == 0) {
      Containing<TiObject> const *container = this->getTiObject()->getInterface<Containing<TiObject> const>();
      if (container != 0) {
        for (Int i = 0; i < container->getElementCount(); ++i) {
          MetaHaving *ptr = ti_cast<MetaHaving>(container->getElement(i));
          if (ptr != 0) {
            SharedPtr<SourceLocation> const &sl2 = ptr->findSourceLocation();
            if (sl2 != 0) return sl2;
          }
        }
      }
    }
    return sl;
  }

  public: virtual void setExtra(Char const *name, TioSharedPtr const &obj) = 0;
  public: virtual void removeExtra(Char const *name) = 0;
  public: virtual TioSharedPtr const& getExtra(Char const *name) const = 0;

}; // class


//==============================================================================
// Macros

#define IMPLEMENT_METAHAVING(type) \
  private: Core::Basic::TiWord prodId = UNKNOWN_ID; \
  private: Core::Basic::SharedPtr<Core::Data::SourceLocation> sourceLocation; \
  private: Core::Basic::SharedMap<Core::Basic::TiObject> extras; \
  public: using MetaHaving::setProdId; \
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
  public: using MetaHaving::setSourceLocation; \
  public: virtual void setSourceLocation(Core::Basic::SharedPtr<Core::Data::SourceLocation> const &loc) \
  { \
    this->sourceLocation = loc; \
  } \
  public: virtual Core::Basic::SharedPtr<Core::Data::SourceLocation> const& getSourceLocation() const \
  { \
    return this->sourceLocation; \
  } \
  public: virtual void setExtra(Char const *name, TioSharedPtr const &obj) \
  { \
    this->extras.set(name, obj); \
  } \
  public: virtual void removeExtra(Char const *name) \
  { \
    auto index = this->extras.findIndex(name); \
    if (index != -1) this->extras.remove(index); \
  } \
  public: virtual TioSharedPtr const& getExtra(Char const *name) const \
  { \
    auto index = this->extras.findIndex(name); \
    if (index == -1) return TioSharedPtr::null; \
    else return this->extras.get(index); \
  }

} // namespace

#endif
