/**
 * @file Core/Standard/ParsedDataBrowser.cpp
 * Contains the implementation of class Standard::ParsedDataBrowser.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Standard
{

//==============================================================================
// Static Variables

const Int ParsedDataBrowser::invalidIndex = 1000000000;


//==============================================================================
// Member Functions

void ParsedDataBrowser::initialize(const Char *qualifier)
{
  if (qualifier == 0) {
    throw InvalidArgumentException(STR("qualifier"), STR("Core::Standard::ParsedDataBrowser::initialize"),
                                   STR("Cannot be null."));
  }

  this->nodes.clear();
  Data::Integer num;
  Data::String txt;
  IdentifiableObject *id;

  while (true) {
    id = ParsedDataBrowser::parseQualifierSegment(qualifier, txt, num);
    if (id->isA<Data::Integer>()) {
      Int d = num.get();
      Word i = UNKNOWN_ID;
      if (*qualifier == CHR(':')) {
        ++qualifier;
        id = ParsedDataBrowser::parseQualifierSegment(qualifier, txt, num);
        if (id->isA<Data::Integer>()) {
          throw InvalidArgumentException(STR("qualifier"),
                                         STR("Core::Standard::ParsedDataBrowser::initialize"),
                                         STR("Invalid qualifier."), qualifier);
        }
        i = ID_GENERATOR->getId(txt.get());
      }
      this->nodes.push_back(Node(i, d));
    } else {
      this->nodes.push_back(Node(ID_GENERATOR->getId(txt.get()), ParsedDataBrowser::invalidIndex));
    }
    if (*qualifier == CHR('>')) ++qualifier;
    else if (*qualifier == CHR('\0')) break;
    else {
      throw InvalidArgumentException(STR("qualifier"), STR("Core::Standard::ParsedDataBrowser::initialize"),
                                     STR("Invalid qualifier."), qualifier);
    }
  }
  this->qualifier = qualifier;
}


SharedPtr<IdentifiableObject> ParsedDataBrowser::_getValue(Int index, const SharedPtr<ParsedItem> &item) const
{
  // Validation.
  ASSERT(static_cast<Word>(index) < this->nodes.size());
  ASSERT(item != 0);
  // Now we use the id to find the right child.
  SharedPtr<IdentifiableObject> val;
  if (this->nodes[index].index != ParsedDataBrowser::invalidIndex) {
    if (item->isA<ParsedList>()) {
      Int i = this->nodes[index].index;
      ParsedList *list = item.s_cast_get<ParsedList>();
      if (i >= 0 && i < static_cast<Int>(list->getElementCount())) {
        val = list->getElement(i);
      } else if (i < 0 && i >= -static_cast<Int>(list->getElementCount())) {
        val = list->getElement(i + static_cast<Int>(list->getElementCount()));
      }
    } else if (item->isA<ParsedRoute>() && this->nodes[index].index == 0) {
      val = item.s_cast_get<ParsedRoute>()->getData();
    }
    if (this->nodes[index].id != UNKNOWN_ID) {
      if (val != 0 && this->nodes[index].id != val.s_cast_get<ParsedItem>()->getProdId()) {
        val = SharedPtr<IdentifiableObject>(0);
      }
    }
  } else {
    if (item->isA<ParsedList>()) {
      ParsedList *list = item.s_cast_get<ParsedList>();
      Word i;
      for (i = 0; i < list->getElementCount(); ++i) {
        val = list->getElement(i).io_cast<ParsedItem>();
        if (val != 0 && this->nodes[index].id == val.s_cast_get<ParsedItem>()->getProdId()) break;
      }
      if (i == list->getElementCount()) val = SharedPtr<IdentifiableObject>(0);
    } else if (item->isA<ParsedRoute>()) {
      val = item.s_cast_get<ParsedRoute>()->getData().io_cast<ParsedItem>();
      if (val != 0 && this->nodes[index].id != val.s_cast_get<ParsedItem>()->getProdId()) {
        val = SharedPtr<IdentifiableObject>(0);
      }
    }
  }
  // Check if we are done, or we need more recursion.
  if (val == 0 || static_cast<Word>(index) == this->nodes.size()-1) {
    return val;
  } else {
    if (val->isDerivedFrom<ParsedItem>()) {
      return ParsedDataBrowser::_getValue(index+1, val.s_cast<ParsedItem>());
    } else {
      return SharedPtr<IdentifiableObject>(0);
    }
  }
}


SharedPtr<IdentifiableObject> ParsedDataBrowser::_getValue(const Char *qualifier,
                                                           const SharedPtr<ParsedItem> &item)
{
  // Validation.
  ASSERT(qualifier != 0);
  ASSERT(item != 0);
  // Parse this qualifier segment.
  Data::Integer num;
  Data::String txt;
  IdentifiableObject *id;
  id = ParsedDataBrowser::parseQualifierSegment(qualifier, txt, num);
  // Now we use the id to find the right child.
  SharedPtr<IdentifiableObject> val;
  if (id->isA<Data::Integer>()) {
    if (item->isA<ParsedList>()) {
      Int i = num.get();
      ParsedList *list = item.s_cast_get<ParsedList>();
      if (i >= 0 && i < static_cast<Int>(list->getElementCount())) {
        val = list->getElement(i);
      } else if (i < 0 && i >= -static_cast<Int>(list->getElementCount())) {
        val = list->getElement(i + static_cast<Int>(list->getElementCount()));
      }
    } else if (item->isA<ParsedRoute>() && num.get() == 0) {
      val = item.s_cast_get<ParsedRoute>()->getData();
    }
    if (*qualifier == CHR(':')) {
      ++qualifier;
      id = ParsedDataBrowser::parseQualifierSegment(qualifier, txt, num);
      if (id->isA<Data::Integer>()) {
        throw InvalidArgumentException(STR("qualifier"),
                                       STR("Core::Standard::ParsedDataBrowser::getValue"),
                                       STR("Invalid qualifier."), qualifier);
      }
      if (val != 0 && ID_GENERATOR->getId(txt.get()) != val.s_cast_get<ParsedItem>()->getProdId()) {
        val = SharedPtr<IdentifiableObject>(0);
      }
    }
  } else {
    if (item->isA<ParsedList>()) {
      ParsedList *list = item.s_cast_get<ParsedList>();
      Word i;
      for (i = 0; i < list->getElementCount(); ++i) {
        val = list->getElement(i).io_cast<ParsedItem>();
        if (val != 0 &&
            ID_GENERATOR->getId(txt.get()) == val.s_cast_get<ParsedItem>()->getProdId()) break;
      }
      if (i == list->getElementCount()) val = SharedPtr<IdentifiableObject>(0);
    } else if (item->isA<ParsedRoute>()) {
      val = item.s_cast_get<ParsedRoute>()->getData().io_cast<ParsedItem>();
      if (val != 0 && ID_GENERATOR->getId(txt.get()) != val.s_cast_get<ParsedItem>()->getProdId()) {
        val = SharedPtr<IdentifiableObject>(0);
      }
    }
  }
  // Check if we are done, or we need more recursion.
  if (val == 0 || *qualifier == CHR('\0')) {
    return val;
  } else if (*qualifier == CHR('>')) {
    if (val->isDerivedFrom<ParsedItem>()) {
      return ParsedDataBrowser::_getValue(qualifier+1, val.s_cast<ParsedItem>());
    }
  }
  throw InvalidArgumentException(STR("qualifier"), STR("Core::Standard::ParsedDataBrowser::getValue"),
                                 STR("Invalid qualifier."), qualifier);
}


IdentifiableObject* ParsedDataBrowser::parseQualifierSegment(const Char *&qualifier, Data::String &txt,
                                                             Data::Integer &num)
{
  Int i;
  // Validation.
  if (qualifier == 0) {
    throw InvalidArgumentException(STR("qualifier"),
                                   STR("Core::Standard::ParsedDataBrowser::parseQualifierSegment"),
                                   STR("Cannot be null."));
  }

  if ((*qualifier >= CHR('a') && *qualifier <= CHR('z')) ||
      (*qualifier >= CHR('A') && *qualifier <= CHR('Z')) ||
      *qualifier == CHR('_')) {
    // Parse a string id.
    i = 0;
    while ((qualifier[i] >= CHR('a') && qualifier[i] <= CHR('z')) ||
           (qualifier[i] >= CHR('A') && qualifier[i] <= CHR('Z')) ||
           (qualifier[i] >= CHR('0') && qualifier[i] <= CHR('9')) ||
           qualifier[i] == CHR('_') || qualifier[i] == CHR('.')) i++;
    txt.set(qualifier, i);
    qualifier += i;
    return &txt;
  } else if (*qualifier == CHR('-') || (*qualifier >= CHR('0') && *qualifier <= CHR('9'))) {
    // Parse an integer id.
    Int sign;
    if (*qualifier==CHR('-')) {
      sign = -1;
      ++qualifier;
    } else {
      sign = 1;
    }
    i = 0;
    Int j = 0;
    while (*qualifier >= CHR('0') && *qualifier <= CHR('9')) {
      j *= 10;
      j += static_cast<Int>(*qualifier - CHR('0'));
      ++qualifier;
    }
    num.set(j*sign);
    return &num;
  } else {
    throw InvalidArgumentException(STR("qualifier"),
                                   STR("Core::Standard::ParsedDataBrowser::parseQualifierSegment"),
                                   STR("Invalid qualifier."));
  }
}

} } // namespace
