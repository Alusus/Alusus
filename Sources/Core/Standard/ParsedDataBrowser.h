/**
 * @file Core/Standard/ParsedDataBrowser.h
 * Contains the header of class Core::Standard::ParsedDataBrowser.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef STANDARD_PARSED_DATA_BROWSER_H
#define STANDARD_PARSED_DATA_BROWSER_H

namespace Core { namespace Standard
{

// TODO: DOC

class ParsedDataBrowser
{
  //============================================================================
  // Data Types

  private: struct Node
  {
    Word id;
    Int index;
    Node(Word i, Int d) : id(i), index(d) {}
  };


  //============================================================================
  // Member Variables

  private: std::vector<Node> nodes;

  private: static const Int invalidIndex;

  private: Str qualifier;


  //============================================================================
  // Constructor & Destructor

  public: ParsedDataBrowser()
  {
  }

  public: ParsedDataBrowser(Char const *qualifier)
  {
    this->initialize(qualifier);
  }

  public: ~ParsedDataBrowser()
  {
  }


  //============================================================================
  // Member Functions

  public: void initialize(Char const *qualifier);

  public: Bool isInitialized()
  {
    return !this->nodes.empty();
  }

  public: template <class T> SharedPtr<T> getValue(const SharedPtr<ParsedItem> &item) const
  {
    // Validation.
    if (this->nodes.empty()) {
      throw GeneralException(STR("Object is not initialized yet."),
                             STR("Core::Standard::ParsedDataBrowser::getValue"));
    }
    if (item == 0) {
      throw InvalidArgumentException(STR("item"), STR("Core::Standard::ParsedDataBrowser::getValue"),
                                     STR("Cannot be null."));
    }

    if (this->nodes.front().id == UNKNOWN_ID) {
      throw GeneralException(STR("Cannot use getValue when the object's qualifier starts with a number."),
                             STR("Core::Standard::ParsedDataBrowser::getValue"));
    } else if (this->nodes.front().id != item->getProdId()) {
      return SharedPtr<T>(0);
    }

    // Check if we are done, or we need more recursion.
    if (this->nodes.size() == 1) {
      return item.io_cast<T>();
    } else {
      return ParsedDataBrowser::_getValue(1, item).io_cast<T>();
    }
  }

  public: template <class T> SharedPtr<T> getChildValue(const SharedPtr<ParsedItem> &parent) const
  {
    return this->_getValue(Int(0), parent).io_cast<T>();
  }

  public: SharedPtr<IdentifiableObject> _getValue(Int index, const SharedPtr<ParsedItem> &item) const;

  public: template <class T> static SharedPtr<T> getValue(Char const *qualifier,
                                                          const SharedPtr<ParsedItem> &item)
  {
    // Validation.
    if (qualifier == 0) {
      throw InvalidArgumentException(STR("qualifier"), STR("Core::Standard::ParsedDataBrowser::getValue"),
                                     STR("Cannot be null."));
    }
    if (item == 0) {
      throw InvalidArgumentException(STR("item"), STR("Core::Standard::ParsedDataBrowser::getValue"),
                                     STR("Cannot be null."));
    }
    // Parse this qualifier segment.
    Data::Integer num;
    Data::String txt;
    IdentifiableObject *id;
    Char const *originalQualifier = qualifier;
    id = ParsedDataBrowser::parseQualifierSegment(qualifier, txt, num);
    // Now we use the id to find the right child.
    if (id->isA<Data::Integer>()) {
      throw InvalidArgumentException(STR("qualifier"), STR("Core::Standard::ParsedDataBrowser::getValue"),
                                     STR("Invalid qualifier."), originalQualifier);
    } else if (ID_GENERATOR->getId(txt.get()) != item->getProdId()) {
      return SharedPtr<T>(0);
    }
    // Check if we are done, or we need more recursion.
    if (*qualifier == CHR('\0')) {
      return item.io_cast<T>();
    } else if (*qualifier == CHR('>')) {
      return ParsedDataBrowser::_getValue(qualifier+1, item).io_cast<T>();
    } else {
      throw InvalidArgumentException(STR("qualifier"), STR("Core::Standard::ParsedDataBrowser::getValue"),
                                     STR("Invalid qualifier."), originalQualifier);
    }
  }

  public: template <class T> static SharedPtr<T> getChildValue(Char const *qualifier,
                                                                 const SharedPtr<ParsedItem> &parent)
  {
    return ParsedDataBrowser::_getValue(qualifier, parent).io_cast<T>();
  }

  private: static SharedPtr<IdentifiableObject> _getValue(Char const *qualifier,
                                                          const SharedPtr<ParsedItem> &item);

  private: static IdentifiableObject* parseQualifierSegment(Char const *&qualifier, Data::String &txt,
                                                            Data::Integer &num);

  public: const Str &getQualifier() const { return qualifier; }

}; // class

} } // namespace

#endif
