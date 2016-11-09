/**
 * @file Core/Data/IdHolder.h
 * Contains the header of interface Data::IdHolder.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_IDHOLDER_H
#define CORE_DATA_IDHOLDER_H

namespace Core { namespace Data
{

// TODO: DOC

class IdHolder : public TiInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(IdHolder, TiInterface, "Core.Data", "Core", "alusus.net");


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

#define IMPLEMENT_IDHOLDER(type) \
  private: Core::Basic::TiWord id = UNKNOWN_ID; \
  using IdHolder::setId; \
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

} } // namespace

#endif
