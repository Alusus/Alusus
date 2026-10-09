/**
 * @file Core/Grammar/IdHaving.h
 * Contains the header of interface Core::Grammar::IdHaving.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_GRAMMAR_IDHAVING_H
#define CORE_GRAMMAR_IDHAVING_H

namespace Core::Grammar
{

// TODO: DOC

class IdHaving : public TiInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(IdHaving, TiInterface, "Core.Grammar", "Core", "alusus.org");


  //============================================================================
  // Abstract Functions

  public: virtual void setId(Word id) = 0;
  public: virtual void setId(TiWord const *id)
  {
    this->setId(id == 0 ? UNKNOWN_ID : id->get());
  }

  public: virtual TiWord& getId() = 0;
  public: virtual TiWord const& getId() const = 0;

  public: virtual Str const& getIdString() const
  {
    return ID_GENERATOR->getDesc(this->getId());
  }

}; // class


//==============================================================================
// Macros

#define IMPLEMENT_IDHAVING(type) \
  private: Core::Basic::TiWord id = UNKNOWN_ID; \
  using Core::Grammar::IdHaving::setId; \
  public: virtual void setId(Word id) \
  { \
    this->id = id; \
  } \
  public: virtual Core::Basic::TiWord& getId() \
  { \
    return this->id; \
  } \
  public: virtual Core::Basic::TiWord const& getId() const \
  { \
    return this->id; \
  }

} // namespace

#endif
