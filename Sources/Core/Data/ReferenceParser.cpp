/**
 * @file Core/Data/ReferenceParser.cpp
 * Contains the implementation of class Core::Data::ReferenceParser.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Data
{

//==============================================================================
// Construction & Destruction

ReferenceParser::CharGroupStorage* ReferenceParser::getSharedCharGroupStorage()
{
  static ReferenceParser::CharGroupStorage *charGroupStorage = 0;
  if (charGroupStorage == 0) {
    charGroupStorage = reinterpret_cast<ReferenceParser::CharGroupStorage*>
                       (GLOBAL_STORAGE->getObject(STR("Core::Data::ReferenceParser::charGroupStorage")));
    if (charGroupStorage == 0) {
      charGroupStorage = new ReferenceParser::CharGroupStorage;
      GLOBAL_STORAGE->setObject(STR("Core::Data::ReferenceParser::charGroupStorage"),
                                reinterpret_cast<void*>(charGroupStorage));
    }
  }
  return charGroupStorage;
}


//==============================================================================
// Char Group Functions

void ReferenceParser::setSharedLetterCharGroup(SharedPtr<CharGroupUnit> const group)
{
  ReferenceParser::CharGroupStorage *storage = ReferenceParser::getSharedCharGroupStorage();
  storage->letterCharGroup = group;
}


void ReferenceParser::setSharedNumberCharGroup(SharedPtr<CharGroupUnit> const group)
{
  ReferenceParser::CharGroupStorage *storage = ReferenceParser::getSharedCharGroupStorage();
  storage->numberCharGroup = group;
}


CharGroupUnit* ReferenceParser::getLetterCharGroup() const
{
  ReferenceParser::CharGroupStorage *storage = ReferenceParser::getSharedCharGroupStorage();
  if (storage->letterCharGroup == 0) {
    storage->letterCharGroup =
      UnionCharGroupUnit::create({
        SequenceCharGroupUnit::create(STR("a"), STR("z")),
        SequenceCharGroupUnit::create(STR("A"), STR("Z")),
        SequenceCharGroupUnit::create(STR("_"), STR("_"))
      });
  }
  return storage->letterCharGroup.get();
}


CharGroupUnit* ReferenceParser::getNumberCharGroup() const
{
  ReferenceParser::CharGroupStorage *storage = ReferenceParser::getSharedCharGroupStorage();
  if (storage->numberCharGroup == 0) {
    storage->numberCharGroup =
      SequenceCharGroupUnit::create(STR("0"), STR("9"));
  }
  return storage->numberCharGroup.get();
}


//==============================================================================
// Qualifier Building Functions

/**
 * Fills the provided StrStream object with the textual representation of this
 * reference.
 * @sa parseQualifier()
 */
void ReferenceParser::buildQualifier(Reference const *ref, StrStream &qualifier)
{
  if (ref == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("ref"), STR("Cannot be null."));
  }

  if (ref->isA<ScopeReference>()) {
    qualifier << static_cast<ScopeReference const*>(ref)->getScope() << STR(":");
  } else {
    if (ref->isA<IndexReference>()) {
      qualifier << CHR('(') << static_cast<IndexReference const*>(ref)->getIndex() << CHR(')');
    } else if (ref->isA<StrKeyReference>()) {
      qualifier << static_cast<StrKeyReference const*>(ref)->getKey();
    } else if (ref->isA<IndirectReference>()) {
      qualifier << ReferenceParser::EVAL_KEYWORD;
      this->buildQualifier(static_cast<IndirectReference const*>(ref)->getKey().get(), qualifier);
      qualifier << CHR('}');
    } else if (ref->isA<RawIndirectReference>()) {
      RawIndirectReference const *iref = static_cast<RawIndirectReference const*>(ref);
      qualifier << ReferenceParser::EVAL_KEYWORD << Str(iref->getQualifier(), iref->getSize()) << CHR('}');
    } else if (ref->isA<SearchReference>()) {
      SearchReference const *iref = static_cast<SearchReference const*>(ref);
      qualifier << ReferenceParser::FIND_KEYWORD;
      StrAttributeValidator *validator = iref->getSearchValidator().tio_cast_get<StrAttributeValidator>();
      if (validator == 0) {
        throw EXCEPTION(GenericException, STR("Provided reference contains a search segment with an invalid validator."));
      }
      qualifier << validator->getName() << CHR('=') << validator->getValue();
      if (iref->getMatchLimitation() != -1) {
        qualifier << STR(", ") << iref->getMatchLimitation();
      }
      if (!iref->getSearchDirection()) qualifier << STR("backwards");
      qualifier << CHR("}");
    } else if (ref->isA<SelfReference>()) {
      qualifier << ReferenceParser::SELF_KEYWORD;
    } else {
      throw EXCEPTION(GenericException, STR("Provided reference contains an invalid segment type."));
    }
    if (ref->getResultValidator() != 0) {
      StrAttributeValidator *validator = ref->getResultValidator().tio_cast_get<StrAttributeValidator>();
      if (validator == 0) {
        throw EXCEPTION(GenericException,
                        STR("Provided reference contains a segment with an invalid result validator."));
      }
      qualifier << ReferenceParser::WHERE_KEYWORD;
      qualifier << validator->getName() << CHR('=') << validator->getValue();
      qualifier << CHR(')');
    }
    if (ref->getNext() != 0) qualifier << STR(".");
  }

  if (ref->getNext() != 0) this->buildQualifier(ref->getNext().get(), qualifier);
}


//==============================================================================
// Qualifier Parsing Functions

/**
 * This private function is called by the public parseQualifier function
 * to perform the actual parsing.
 * Parses the given string based qualifier into a valid chain of Reference
 * objects. This function performs the conversion recursively by calling itself
 * with a subset of the given string to generate the child Reference objects.
 * The format is as follows: <br/>
 * qualifier = id + ("." + id)*v; <br/>
 * id = string | integer | "(" + qualifier + ")" | "{" + operation + "}"; <br/>
 * integer = ('0'..'9')*v1; </br>
 * string = ('a'..'z'|'A'..'Z'|'_') + ('a'..'z'|'A'..'Z'|'0'..'9'|'_')*v; <br/>
 * operation = "find" + validator + ("," + integer)*o; <br/>
 * validator = key + "=" + value; <br/>
 * <br/>
 * Brackets are used to use a reference as the id of another reference.
 * Strings are used to reference map items by their key, while integers are
 * used to reference list items by their index.
 * @param pos A reference to a variable to contain the character position at which
 *            a recursion ended. For example, if a recursion is starting with an
 *            opening bracket, by the end of this recursion pos should contain
 *            the position of the closing bracket.
 *            At the end of the call this will contain the position of
 *            the character that is NOT processed during that call.
 */
SharedPtr<Reference> ReferenceParser::_parseQualifier(Char const *qualifier, Int &pos)
{
  // Validation.
  if (qualifier == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Cannot be null."));
  }

  SharedPtr<Reference> ref = this->_parseQualifierSegment(qualifier, pos);

  // Check if a result validator is attached to the segment.
  if (compareStr(qualifier+pos, ReferenceParser::WHERE_KEYWORD, ReferenceParser::WHERE_KEYWORD_LEN) == 0) {
    pos += ReferenceParser::WHERE_KEYWORD_LEN;
    Char const *qualifier2 = qualifier + pos;
    auto resultValidator = this->createResultValidator(qualifier2);
    pos = qualifier2 - qualifier;
    ref->setResultValidator(resultValidator);
    // Now we need to skip the closing bracket, after making sure it exists.
    if (qualifier[pos] != CHR(')')) {
      throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Invalid qualifier."), qualifier);
    }
    ++pos;
  }

  if (qualifier[pos] == CHR('.') || qualifier[pos] == CHR(':')) {
    ++pos;
    ref->setNext(this->_parseQualifier(qualifier, pos));
  }

  // At this point we should've reached the end of the qualifier or subqualifier.
  if (qualifier[pos] != CHR('\0') && (qualifier[pos] != CHR('}') || pos == 0)) {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Invalid qualifier."), qualifier);
  }

  return ref;
}


SharedPtr<Reference> ReferenceParser::_parseQualifierSegment(Char const *qualifier, Int &pos)
{
  // Validation.
  if (qualifier == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Cannot be null."));
  }

  SharedPtr<Reference> seg;

  WChar wc;
  Int processedIn, processedOut;

  convertStr(qualifier+pos, 4, &wc, 1, processedIn, processedOut);
  if (processedOut == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Invalid qualifier."), qualifier);
  }
  // Check the first character to determine the route to take.
  if (qualifier[pos] == CHR('(')) {
    //
    // IndexReference
    //
    ++pos;
    Int j = 0;
    pos += this->parseInt(qualifier+pos, j);
    seg = this->createIndexReference(j);
    seg->setUsageCriteria(this->usageCriteria);
    // Now we need to skip the closing bracket, after making sure it exists.
    if (qualifier[pos] != CHR(')')) {
      throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Invalid qualifier."), qualifier);
    }
    ++pos;
  } else if (qualifier[pos] >= CHR('0') && qualifier[pos] <= CHR('9')) {
    //
    // IndexReference
    //
    Int j = 0;
    pos += this->parseInt(qualifier+pos, j);
    seg = this->createIndexReference(j);
    seg->setUsageCriteria(this->usageCriteria);
  } else if (matchCharGroup(wc, this->getLetterCharGroup())) {
    //
    // StrKeyReference or ScopeReference or SelfReference
    //
    Int i = 0;
    do {
      i += processedIn;
      convertStr(qualifier+pos+i, 4, &wc, 1, processedIn, processedOut);
      if (processedOut == 0) {
        throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Invalid qualifier."), qualifier);
      }
    } while (matchCharGroup(wc, this->getLetterCharGroup()) ||
             matchCharGroup(wc, this->getNumberCharGroup()));
    if (qualifier[pos+i] == CHR(':')) {
      seg = this->createScopeReference(qualifier+pos, i);
    } else if (i == ReferenceParser::SELF_KEYWORD_LEN &&
               compareStr(qualifier+pos, ReferenceParser::SELF_KEYWORD, i) == 0) {
      seg = this->createSelfReference();
    } else {
      seg = this->createStrKeyReference(qualifier+pos, i);
    }
    seg->setUsageCriteria(this->usageCriteria);
    pos += i;
  } else if (compareStr(qualifier+pos, ReferenceParser::FIND_KEYWORD, ReferenceParser::FIND_KEYWORD_LEN) == 0) {
    //
    // SearchReference
    //
    pos += ReferenceParser::FIND_KEYWORD_LEN;
    Char const *qualifier2 = qualifier + pos;
    seg = this->createSearchReference(qualifier2);
    seg->setUsageCriteria(this->usageCriteria);
    pos = qualifier2 - qualifier;
    // Now we need to skip the closing bracket, after making sure it exists.
    if (qualifier[pos] != CHR('}')) {
      throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Invalid qualifier."), qualifier);
    }
    ++pos;
  } else if (compareStr(qualifier+pos, ReferenceParser::EVAL_KEYWORD, ReferenceParser::EVAL_KEYWORD_LEN) == 0) {
    //
    // IndirectReference
    //
    pos += ReferenceParser::EVAL_KEYWORD_LEN;
    Char const *qualifier2 = qualifier + pos;
    seg = this->createIndirectReference(qualifier2);
    seg->setUsageCriteria(this->usageCriteria);
    pos = qualifier2 - qualifier;
    // Now we need to skip the closing bracket, after making sure it exists.
    if (qualifier[pos] != CHR('}')) {
      throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Invalid qualifier."), qualifier);
    }
    ++pos;
  } else {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Invalid qualifier."), qualifier);
  }

  return seg;
}


/**
 * Parses a single segment of the given qualifier string into an existing String
 * or Integer objects. The caller has to provide a String and an Integer objects
 * and the method will write the parsed results to one of them, depending on the
 * contents found in the qualifier string, then it will return a pointer to the
 * object to which the results was written. The given qualifier string will also
 * be incremented to point to the character immediately following the parsed
 * segment. If the segment ends with a dot, qualifier will be moved to point to
 * the dot, not the beginning of the new segment.
 */
Reference const& ReferenceParser::parseQualifierSegmentToTemp(Char const *&qualifier)
{
  Int i;
  // Validation.
  if (qualifier == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Cannot be null."));
  }

  WChar wc;
  Int processedIn, processedOut;

  convertStr(qualifier, 4, &wc, 1, processedIn, processedOut);
  if (processedOut == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Invalid qualifier."), qualifier);
  }

  if (matchCharGroup(wc, this->getLetterCharGroup())) {
    //
    // StrKeyReference or ScopeReference or SelfReference
    //
    i = 0;
    do {
      i += processedIn;
      convertStr(qualifier+i, 4, &wc, 1, processedIn, processedOut);
      if (processedOut == 0) {
        throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Invalid qualifier."), qualifier);
      }
    } while (matchCharGroup(wc, this->getLetterCharGroup()) ||
             matchCharGroup(wc, this->getNumberCharGroup()));
    if (qualifier[i] == CHR(':')) {
      Reference const &ref = this->prepareTempScopeReference(qualifier, i);
      qualifier += i;
      this->setTempQualifierSegmentResultValidator(qualifier, const_cast<Reference*>(&ref));
      return ref;
    } else if (i == ReferenceParser::SELF_KEYWORD_LEN &&
               compareStr(qualifier, ReferenceParser::SELF_KEYWORD, i) == 0) {
      Reference const &ref = this->prepareTempSelfReference();
      qualifier += i;
      this->setTempQualifierSegmentResultValidator(qualifier, const_cast<Reference*>(&ref));
      return ref;
    } else {
      Reference const &ref = this->prepareTempStrKeyReference(qualifier, i);
      qualifier += i;
      this->setTempQualifierSegmentResultValidator(qualifier, const_cast<Reference*>(&ref));
      return ref;
    }
  } else if (*qualifier == CHR('(')) {
    //
    // IndexReference
    //
    Int j = 0;
    ++qualifier;
    qualifier += this->parseInt(qualifier, j);
    Reference const &ref = this->prepareTempIndexReference(j);
    if (*qualifier != CHR(')')) {
      throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Invalid qualifier."), qualifier);
    }
    ++qualifier;
    this->setTempQualifierSegmentResultValidator(qualifier, const_cast<Reference*>(&ref));
    return ref;
  } else if (*qualifier >= CHR('0') && *qualifier <= CHR('9')) {
    //
    // IndexReference
    //
    Int j = 0;
    qualifier += this->parseInt(qualifier, j);
    Reference const &ref = this->prepareTempIndexReference(j);
    this->setTempQualifierSegmentResultValidator(qualifier, const_cast<Reference*>(&ref));
    return ref;
  } else if (compareStr(qualifier, ReferenceParser::FIND_KEYWORD, ReferenceParser::FIND_KEYWORD_LEN) == 0) {
    //
    // SearchReference
    //
    qualifier += ReferenceParser::FIND_KEYWORD_LEN;
    Reference const &ref = this->prepareTempSearchReference(qualifier);
    // Now we need to skip the closing bracket, after making sure it exists.
    if (*qualifier != CHR('}')) {
      throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Invalid qualifier."), qualifier);
    }
    ++qualifier;
    this->setTempQualifierSegmentResultValidator(qualifier, const_cast<Reference*>(&ref));
    return ref;
  } else if (compareStr(qualifier, ReferenceParser::EVAL_KEYWORD, ReferenceParser::EVAL_KEYWORD_LEN) == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"),
                    STR("Indirect references are not supported by this function."), qualifier);
  } else {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Invalid qualifier."), qualifier);
  }
}


void ReferenceParser::setTempQualifierSegmentResultValidator(Char const *&qualifier, Reference *seg)
{
  // Check if a result validator is attached to the segment.
  if (compareStr(qualifier, ReferenceParser::WHERE_KEYWORD, ReferenceParser::WHERE_KEYWORD_LEN) == 0) {
    qualifier += ReferenceParser::WHERE_KEYWORD_LEN;
    if (this->tempResultValidator == 0) this->tempResultValidator = StrAttributeValidator::create();
    qualifier += this->parseStrAttributeValidator(qualifier, this->tempResultValidator.get());
    if (*qualifier == CHR('&') || *qualifier == CHR('|')) {
      throw EXCEPTION(InvalidArgumentException, STR("qualifier"),
                      STR("Logical operations in validators are not supported yet."), qualifier);
    }
    seg->setResultValidator(this->tempResultValidator);
    if (*qualifier != CHR(')')) {
      throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Invalid qualifier."), qualifier);
    }
    ++qualifier;
  } else {
    seg->setResultValidator(SharedPtr<Validator>());
  }
}


//==============================================================================
// Qualifier Validation and Segment Seeking Functions

/**
 * Called by the public validateQualifier funciton. This recursive function
 * performas the actual validation of the qualifier string.
 * @param absolute Specifies whether to validate an absolute qualifier or not.
 *                 An absolute qualifier is one that doesn't use a subqualifier,
 *                 i.e. doesn't have any brackets.
 * @param subQualifier Specifies whether we are currently parsing a sub
 *                     qualifier. If this value is false the function will know
 *                     that it's not validating a subqualifier and hence will
 *                     refuse any closing brackets it may encounter.
 */
Bool ReferenceParser::_validateQualifier(Char const *&qualifier, Bool absolute, Bool subQualifier)
{
  // Validation.
  if (qualifier == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Cannot be null."));
  }

  if (!this->_validateQualifierSegment(qualifier, absolute)) return false;

  // Parse result validation.
  if (compareStr(qualifier, ReferenceParser::WHERE_KEYWORD, ReferenceParser::WHERE_KEYWORD_LEN) == 0) {
    qualifier += ReferenceParser::WHERE_KEYWORD_LEN;
    if (!this->_validateResultValidator(qualifier)) return false;
    if (*qualifier != CHR(')')) return false;
    ++qualifier;
  }

  // Parse child references, if any.
  if (*qualifier == CHR('.') || *qualifier == CHR(':')) {
    ++qualifier;
    return this->_validateQualifier(qualifier, absolute, subQualifier);
  }

  // At this point we should've reached the end of the qualifier or subqualifier.
  if (*qualifier != CHR('\0') && subQualifier == false) return false;

  return true;
}


Bool ReferenceParser::_validateQualifierSegment(Char const *&qualifier, Bool absolute)
{
  // Validation.
  if (qualifier == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Cannot be null."));
  }

  WChar wc;
  Int processedIn, processedOut;
  convertStr(qualifier, 4, &wc, 1, processedIn, processedOut);
  if (processedOut == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Invalid qualifier."), qualifier);
  }

  // Check the first character to determine the route to take.
  if (*qualifier == CHR('(') && !absolute) {
    // Validate the subqualifier.
    Int j = 0;
    ++qualifier;
    qualifier += this->parseInt(qualifier, j);
    // Now we need to skip the closing bracket, after making sure it exists.
    if (*qualifier != CHR(')')) return false;
    ++qualifier;
  } else if (*qualifier >= CHR('0') && *qualifier <= CHR('9')) {
    // IndexReference.
    while (*qualifier >= CHR('0') && *qualifier <= CHR('9')) ++qualifier;
  } else if (matchCharGroup(wc, this->getLetterCharGroup())) {
    // StrKeyReference or ScopeReference
    do {
      qualifier += processedIn;
      convertStr(qualifier, 4, &wc, 1, processedIn, processedOut);
      if (processedOut == 0) {
        throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Invalid qualifier."), qualifier);
      }
    } while (matchCharGroup(wc, this->getLetterCharGroup()) ||
             matchCharGroup(wc, this->getNumberCharGroup()));
  } else if (compareStr(qualifier, ReferenceParser::FIND_KEYWORD, ReferenceParser::FIND_KEYWORD_LEN) == 0 &&
             absolute == false) {
    // SearchReference.
    // Skip the validator part.
    qualifier += ReferenceParser::FIND_KEYWORD_LEN;
    qualifier += this->skipSpaces(qualifier);
    Int i = 0;
    while (qualifier[i]!=CHR('=') && qualifier[i]!=CHR('&') && qualifier[i]!=CHR('|') &&
           qualifier[i]!=CHR(')') && qualifier[i]!=CHR('}') &&
           qualifier[i]!=CHR(' ') && qualifier[i]!=CHR('\t') &&
           qualifier[i]!=CHR(',') && qualifier[i]!=CHR('\0')) {
      convertStr(qualifier+i, 4, &wc, 1, processedIn, processedOut);
      i += processedIn;
    }
    if (i == 0) return false;
    qualifier += i;
    qualifier += this->skipSpaces(qualifier);
    if (*qualifier != CHR('=')) return false;
    ++qualifier;
    qualifier += this->skipSpaces(qualifier);
    i = 0;
    while (qualifier[i]!=CHR('&') && qualifier[i]!=CHR('|') &&
           qualifier[i]!=CHR(')') && qualifier[i]!=CHR('}') &&
           qualifier[i]!=CHR(' ') && qualifier[i]!=CHR('\t') &&
           qualifier[i]!=CHR(',') && qualifier[i]!=CHR('\0')) {
      convertStr(qualifier+i, 4, &wc, 1, processedIn, processedOut);
      i += processedIn;
    }
    if (i == 0) return false;
    qualifier += i;
    qualifier += this->skipSpaces(qualifier);
    if (*qualifier == CHR('&') || *qualifier == CHR('|')) return false;
    // Skip match limitation parameter.
    if (*qualifier == CHR(',')) {
      ++qualifier;
      qualifier += this->skipSpaces(qualifier);
      while (*qualifier >= CHR('0') && *qualifier <= CHR('9')) ++qualifier;
      qualifier += this->skipSpaces(qualifier);
    }
    // Check if the search direction should be backwards.
    if (compareStr(qualifier, ReferenceParser::BACKWARDS_KEYWORD, ReferenceParser::BACKWARDS_KEYWORD_LEN) == 0) {
      qualifier += ReferenceParser::BACKWARDS_KEYWORD_LEN;
      qualifier += this->skipSpaces(qualifier);
    }
    if (*qualifier != CHR('}')) return false;
    ++qualifier;
  } else if (compareStr(qualifier, ReferenceParser::EVAL_KEYWORD, ReferenceParser::EVAL_KEYWORD_LEN) == 0 &&
             absolute == false) {
    // Validate the subqualifier.
    qualifier += ReferenceParser::EVAL_KEYWORD_LEN;
    if (!this->_validateQualifier(qualifier, absolute, true)) return false;
    // Now we need to skip the closing bracket, after making sure it exists.
    if (*qualifier != CHR('}')) return false;
    ++qualifier;
  } else {
    return false;
  }

  return true;
}


Bool ReferenceParser::_validateResultValidator(Char const *&qualifier)
{
  WChar wc;
  Int processedIn, processedOut;

  qualifier += this->skipSpaces(qualifier);
  Int i = 0;
  while (qualifier[i]!=CHR('=') && qualifier[i]!=CHR('&') && qualifier[i]!=CHR('|') &&
         qualifier[i]!=CHR(')') && qualifier[i]!=CHR('}') &&
         qualifier[i]!=CHR(' ') && qualifier[i]!=CHR('\t') &&
         qualifier[i]!=CHR(',') && qualifier[i]!=CHR('\0')) {
    convertStr(qualifier+i, 4, &wc, 1, processedIn, processedOut);
    if (processedOut == 0) {
      throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Invalid qualifier."), qualifier);
    }
    i += processedIn;
  }
  if (i == 0) return false;
  qualifier += i;
  qualifier += this->skipSpaces(qualifier);
  if (*qualifier != CHR('=')) return false;
  ++qualifier;
  qualifier += this->skipSpaces(qualifier);
  i = 0;
  while (qualifier[i]!=CHR('&') && qualifier[i]!=CHR('|') &&
         qualifier[i]!=CHR(')') && qualifier[i]!=CHR('}') &&
         qualifier[i]!=CHR(' ') && qualifier[i]!=CHR('\t') &&
         qualifier[i]!=CHR(',') && qualifier[i]!=CHR('\0')) {
    convertStr(qualifier+i, 4, &wc, 1, processedIn, processedOut);
    if (processedOut == 0) {
      throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Invalid qualifier."), qualifier);
    }
    i += processedIn;
  }
  if (i == 0) return false;
  qualifier += i;
  qualifier += this->skipSpaces(qualifier);

  return true;
}


/**
 * This recursive function will do the actual search for the last segment. It
 * will search the string for the end of this segment and will either return
 * the qualifier itself if it encounters a string termination, or it will
 * recurse if it finds a dot or an opening bracket. If it finds a closing
 * bracket it will simply return the position of that closing bracket so that
 * it can continue searching after the closing bracket.
 */
Char const* ReferenceParser::findLastQualifierSegment(Char const *qualifier)
{
  // Validation.
  if (qualifier == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Cannot be null."));
  }

  Char const *beginning = qualifier;

  if (!this->_validateQualifierSegment(qualifier, false)) {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Invalid qualifier."), qualifier);
  }

  // Parse result validation.
  if (compareStr(qualifier, ReferenceParser::WHERE_KEYWORD, ReferenceParser::WHERE_KEYWORD_LEN) == 0) {
    qualifier += ReferenceParser::WHERE_KEYWORD_LEN;
    if (!this->_validateResultValidator(qualifier)) {
      throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Invalid qualifier."), qualifier);
    }
    if (*qualifier != CHR(')')) {
      throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Invalid qualifier."), qualifier);
    }
    ++qualifier;
  }

  if (*qualifier == CHR('.') || *qualifier == CHR(':')) {
    // We found a segment separator, so start again at the new segment.
    return this->findLastQualifierSegment(qualifier+1);
  } else if (*qualifier == CHR('\0')) {
    // Ok, we found a string termination, so whatever we received in the function argument was actually the
    // beginning of the last segment.
    return beginning;
  } else {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Invalid qualifier."), qualifier);
  }
}


//==============================================================================
// Reference Generation Functions

SharedPtr<Reference> ReferenceParser::createScopeReference(Char const *scope, Word length)
{
  if (length == ReferenceParser::ROOT_KEYWORD_LEN &&
      compareStr(scope, ReferenceParser::ROOT_KEYWORD, length) == 0) {
    return std::make_shared<ScopeReference>(scope, length);
  } else {
    return std::make_shared<ScopeReference>(scope, length, false);
  }
}


SharedPtr<Reference> ReferenceParser::createSearchReference(Char const *&qualifier)
{
  auto searchValidator = StrAttributeValidator::create();
  qualifier += this->parseStrAttributeValidator(qualifier, searchValidator.get());
  SharedPtr<Reference> ref = std::make_shared<SearchReference>(searchValidator);
  if (*qualifier == CHR('&') || *qualifier == CHR('|')) {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"),
                    STR("Logical operations in validators are not supported yet."), qualifier);
  }
  // Check for a match limitation parameter.
  if (*qualifier == CHR(',')) {
    ++qualifier;
    qualifier += this->skipSpaces(qualifier);
    Int j = 0;
    while (*qualifier >= CHR('0') && *qualifier <= CHR('9')) {
      j *= 10;
      j += static_cast<Int>(*qualifier - CHR('0'));
      ++qualifier;
    }
    ref.s_cast_get<SearchReference>()->setMatchLimitation(j);
    qualifier += this->skipSpaces(qualifier);
  }
  // Check if the search direction should be backwards.
  if (compareStr(qualifier, ReferenceParser::BACKWARDS_KEYWORD, ReferenceParser::BACKWARDS_KEYWORD_LEN) == 0) {
    qualifier += ReferenceParser::BACKWARDS_KEYWORD_LEN;
    ref.s_cast_get<SearchReference>()->setSearchDirection(false);
    qualifier += this->skipSpaces(qualifier);
  }
  return ref;
}


SharedPtr<Reference> ReferenceParser::createIndirectReference(Char const *&qualifier)
{
  Int i;
  SharedPtr<Reference> ref;
  ref = std::make_shared<IndirectReference>(this->_parseQualifier(qualifier, i));
  qualifier += i;
  return ref;
}


SharedPtr<Validator> ReferenceParser::createResultValidator(Char const *&qualifier)
{
  auto resultValidator = StrAttributeValidator::create();
  qualifier += this->parseStrAttributeValidator(qualifier, resultValidator.get());
  if (*qualifier == CHR('&') || *qualifier == CHR('|')) {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"),
                    STR("Logical operations in validators are not supported yet."), qualifier);
  }
  return resultValidator;
}


Reference const& ReferenceParser::prepareTempScopeReference(Char const *scope, Word length)
{
  this->tempScopeReference.setScope(scope, length);
  if (length == ReferenceParser::ROOT_KEYWORD_LEN &&
      compareStr(scope, ReferenceParser::ROOT_KEYWORD, length) == 0) {
    this->tempScopeReference.setSearchDirection(true);
  } else {
    this->tempScopeReference.setSearchDirection(false);
  }
  return this->tempScopeReference;
}


Reference const& ReferenceParser::prepareTempSearchReference(Char const *&qualifier)
{
  if (this->tempSearchValidator == 0) this->tempSearchValidator = StrAttributeValidator::create();
  qualifier += this->parseStrAttributeValidator(qualifier, this->tempSearchValidator.get());
  this->tempSearchReference.setSearchValidator(this->tempSearchValidator);
  if (*qualifier == CHR('&') || *qualifier == CHR('|')) {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"),
                    STR("Logical operations in validators are not supported yet."), qualifier);
  }
  // Check for a match limitation parameter.
  if (*qualifier == CHR(',')) {
    ++qualifier;
    qualifier += this->skipSpaces(qualifier);
    Int j = 0;
    while (*qualifier >= CHR('0') && *qualifier <= CHR('9')) {
      j *= 10;
      j += static_cast<Int>(*qualifier - CHR('0'));
      ++qualifier;
    }
    this->tempSearchReference.setMatchLimitation(j);
    qualifier += this->skipSpaces(qualifier);
  }
  // Check if the search direction should be backwards.
  if (compareStr(qualifier, ReferenceParser::BACKWARDS_KEYWORD, ReferenceParser::BACKWARDS_KEYWORD_LEN) == 0) {
    qualifier += ReferenceParser::BACKWARDS_KEYWORD_LEN;
    this->tempSearchReference.setSearchDirection(false);
    qualifier += this->skipSpaces(qualifier);
  }
  return this->tempSearchReference;
}


//==============================================================================
// Helper Functions

Int ReferenceParser::parseInt(Char const *qualifier, Int &result)
{
  Int pos = 0;
  result = 0;
  Int sign = 1;
  if (qualifier[pos] == CHR('-')) {
    sign = -1;
    ++pos;
  } else if (qualifier[pos] == CHR('+')) {
    sign = 1;
    ++pos;
  }
  while (qualifier[pos] >= CHR('0') && qualifier[pos] <= CHR('9')) {
    result *= 10;
    result += static_cast<Int>(qualifier[pos] - CHR('0'));
    ++pos;
  }
  result *= sign;
  return pos;
}


Int ReferenceParser::parseStrAttributeValidator(Char const *qualifier, StrAttributeValidator *targetValidator)
{
  WChar wc;
  Int processedIn, processedOut;

  // Parse the name of the expression.
  Int pos = this->skipSpaces(qualifier);
  Int i = pos;
  while (qualifier[i]!=CHR('=') && qualifier[i]!=CHR('&') && qualifier[i]!=CHR('|') &&
         qualifier[i]!=CHR(')') && qualifier[i]!=CHR('}') &&
         qualifier[i]!=CHR(' ') && qualifier[i]!=CHR('\t') &&
         qualifier[i]!=CHR(',') && qualifier[i]!=CHR('\0')) {
    convertStr(qualifier+i, 4, &wc, 1, processedIn, processedOut);
    i += processedIn;
  }
  if (i == pos) {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"),
                    STR("Invalid validator expression."), qualifier);
  }
  targetValidator->setName(qualifier+pos, i-pos);
  pos = i + this->skipSpaces(qualifier+i);
  if (qualifier[pos] != CHR('=')) {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"),
                    STR("Invalid validator expression."), qualifier);
  }
  // skip the = sign.
  ++pos;
  // Parse the value of the expression.
  pos += this->skipSpaces(qualifier+pos);
  i = pos;
  while (qualifier[i]!=CHR('&') && qualifier[i]!=CHR('|') &&
         qualifier[i]!=CHR(')') && qualifier[i]!=CHR('}') &&
         qualifier[i]!=CHR(' ') && qualifier[i]!=CHR('\t') &&
         qualifier[i]!=CHR(',') && qualifier[i]!=CHR('\0')) {
    convertStr(qualifier+i, 4, &wc, 1, processedIn, processedOut);
    i += processedIn;
  }
  if (i == pos) {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"),
                    STR("Invalid validator expression."), qualifier);
  }
  targetValidator->setValue(qualifier+pos, i-pos);
  pos = i + this->skipSpaces(qualifier+i);
  return pos;
}

} } // namespace
