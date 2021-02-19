/**
 * @file Core/Processing/ParserModifierLevel.h
 * Contains the header of class Core::Processing::ParserModifierLevel.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_PARSERMODIFIERLEVEL_H
#define CORE_PROCESSING_PARSERMODIFIERLEVEL_H

namespace Core { namespace Processing
{

class ParserModifierLevel
{
  //============================================================================
  // Friend Classes

  friend class Parser;


  //============================================================================
  // Member Variables

  private: TioSharedPtr data;

  private: Int minProdIndex;

  private: Int maxProdIndex;


  //============================================================================
  // Constructors / Destructor

  public: ParserModifierLevel() : minProdIndex(-1), maxProdIndex(-1)
  {
  }

  public: ParserModifierLevel(TioSharedPtr const &d, Int min = -1, Int max = -1)
    : data(d), minProdIndex(min), maxProdIndex(max)
  {
  }

  public: ParserModifierLevel(ParserModifierLevel const &level) :
    data(level.data),
    minProdIndex(level.minProdIndex),
    maxProdIndex(level.maxProdIndex)
  {
  }

  public: ~ParserModifierLevel()
  {
  }


  //============================================================================
  // Member Functions

  protected: void setData(TioSharedPtr const &d)
  {
    this->data = d;
  }

  public: TioSharedPtr const& getData() const
  {
    return this->data;
  }

  protected: void setMinProdIndex(Int i)
  {
    this->minProdIndex = i;
  }

  public: Int getMinProdIndex() const
  {
    return this->minProdIndex;
  }

  protected: void setMaxProdIndex(Int i)
  {
    this->maxProdIndex = i;
  }

  public: Int getMaxProdIndex() const
  {
    return this->maxProdIndex;
  }

}; // class

} } // namespace

#endif
