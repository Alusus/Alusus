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
// Member Functions

/**
 * Fills the provided StrStream object with the textual representation of this
 * reference.
 * @sa parseQualifier()
 */
void ReferenceParser::buildQualifier(Reference const *ref, StrStream &qualifier)
{
  if (ref == 0) {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::ReferenceParser::buildQualifier"),
                                   STR("Cannot be null."));
  }

  while (ref != 0) {
    if (ref->isA<ScopeReference>()) {
      qualifier << static_cast<ScopeReference const*>(ref)->getScope() << STR(":");
    } else {
      if (ref->isA<IndexReference>()) {
        qualifier << CHR('(') << static_cast<IndexReference const*>(ref)->getIndex() << CHR(')');
      } else if (ref->isA<StrKeyReference>()) {
        qualifier << static_cast<StrKeyReference const*>(ref)->getKey();
      } else if (ref->isA<IndirectReference>()) {
        qualifier << ReferenceParser::EVAL_KEYWORD;
        ReferenceParser::buildQualifier(static_cast<IndirectReference const*>(ref)->getKey().get(), qualifier);
        qualifier << CHR('}');
      } else if (ref->isA<RawIndirectReference>()) {
        RawIndirectReference const *iref = static_cast<RawIndirectReference const*>(ref);
        qualifier << ReferenceParser::EVAL_KEYWORD << Str(iref->getQualifier(), iref->getSize()) << CHR('}');
      } else if (ref->isA<SearchReference>()) {
        SearchReference const *iref = static_cast<SearchReference const*>(ref);
        qualifier << ReferenceParser::FIND_KEYWORD;
        StrAttributeValidator *validator = iref->getSearchValidator().io_cast_get<StrAttributeValidator>();
        if (validator == 0) {
          throw GeneralException(STR("Provided reference contains a search segment with an invalid validator."),
                                 STR("Core::Data::ReferenceParser::buildQualifier"));
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
        throw GeneralException(STR("Provided reference contains an invalid segment type."),
                               STR("Core::Data::ReferenceParser::buildQualifier"));
      }
      if (ref->getResultValidator() != 0) {
        StrAttributeValidator *validator = ref->getResultValidator().io_cast_get<StrAttributeValidator>();
        if (validator == 0) {
          throw GeneralException(STR("Provided reference contains a segment with an invalid result validator."),
                                 STR("Core::Data::ReferenceParser::buildQualifier"));
        }
        qualifier << ReferenceParser::WHERE_KEYWORD;
        qualifier << validator->getName() << CHR('=') << validator->getValue();
        qualifier << CHR(')');
      }
      if (ref->getNext() != 0) qualifier << STR(".");
    }
    ref = ref->getNext().get();
  }
}


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
 * @param criteria The usage criteria that will be assigned to each segment in
 *                 the generated reference chain.
 */
SharedPtr<Reference> ReferenceParser::_parseQualifier(Char const *qualifier, Int &pos,
                                                      ReferenceUsageCriteria criteria)
{
  // Validation.
  if (qualifier == 0) {
    throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::ReferenceParser::parseQualifier"),
                                   STR("Cannot be null."));
  }

  SharedPtr<Reference> ref;
  SharedPtr<Reference> seg;
  SharedPtr<Reference> lastSeg;

  pos=0;

  while (true) {
    // Check the first character to determine the route to take.
    if (qualifier[pos] == CHR('(')) {
      //
      // IndexReference
      //
      ++pos;
      Int j = 0;
      pos += ReferenceParser::parseInt(qualifier+pos, j);
      seg = std::make_shared<IndexReference>(j);
      seg->setUsageCriteria(criteria);
      // Now we need to skip the closing bracket, after making sure it exists.
      if (qualifier[pos] != CHR(')')) {
        throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::Reference::parseQualifier"),
                                       STR("Invalid qualifier."), qualifier);
      }
      ++pos;
    } else if (qualifier[pos] >= CHR('0') && qualifier[pos] <= CHR('9')) {
      //
      // IndexReference
      //
      Int j = 0;
      pos += ReferenceParser::parseInt(qualifier+pos, j);
      seg = std::make_shared<IndexReference>(j);
      seg->setUsageCriteria(criteria);
    } else if ((qualifier[pos] >= CHR('a') && qualifier[pos] <= CHR('z')) ||
               (qualifier[pos] >= CHR('A') && qualifier[pos] <= CHR('Z')) ||
               qualifier[pos] == CHR('_')) {
      //
      // StrKeyReference or ScopeReference or SelfReference
      //
      Int i = 0;
      while ((qualifier[pos+i] >= CHR('a') && qualifier[pos+i] <= CHR('z')) ||
             (qualifier[pos+i] >= CHR('A') && qualifier[pos+i] <= CHR('Z')) ||
             (qualifier[pos+i] >= CHR('0') && qualifier[pos+i] <= CHR('9')) ||
             qualifier[pos+i] == CHR('_')) i++;
      if (qualifier[pos+i] == CHR(':')) {
        if (i == ReferenceParser::ROOT_KEYWORD_LEN &&
            compareStr(qualifier+pos, ReferenceParser::ROOT_KEYWORD, i) == 0) {
          seg = std::make_shared<ScopeReference>(qualifier+pos, i);
        } else {
          seg = std::make_shared<ScopeReference>(qualifier+pos, i, false);
        }
      } else if (i == ReferenceParser::SELF_KEYWORD_LEN &&
                 compareStr(qualifier+pos, ReferenceParser::SELF_KEYWORD, i) == 0) {
        seg = std::make_shared<SelfReference>();
      } else {
        seg = std::make_shared<StrKeyReference>(qualifier+pos, i);
      }
      seg->setUsageCriteria(criteria);
      pos += i;
    } else if (compareStr(qualifier+pos, ReferenceParser::FIND_KEYWORD, ReferenceParser::FIND_KEYWORD_LEN) == 0) {
      //
      // SearchReference
      //
      pos += ReferenceParser::FIND_KEYWORD_LEN;
      auto searchValidator = StrAttributeValidator::create();
      pos += ReferenceParser::parseStrAttributeValidator(qualifier+pos, searchValidator.get());
      seg = std::make_shared<SearchReference>(searchValidator);
      if (qualifier[pos] == CHR('&') || qualifier[pos] == CHR('|')) {
        throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::Reference::parseQualifier"),
                                       STR("Logical operations in validators are not supported yet."), qualifier);
      }
      // Check for a match limitation parameter.
      if (qualifier[pos] == CHR(',')) {
        ++pos;
        pos += ReferenceParser::skipSpaces(qualifier+pos);
        Int j = 0;
        while (qualifier[pos] >= CHR('0') && qualifier[pos] <= CHR('9')) {
          j *= 10;
          j += static_cast<Int>(qualifier[pos] - CHR('0'));
          ++pos;
        }
        seg.s_cast_get<SearchReference>()->setMatchLimitation(j);
        pos += ReferenceParser::skipSpaces(qualifier+pos);
      }
      // Check if the search direction should be backwards.
      if (compareStr(qualifier+pos, ReferenceParser::BACKWARDS_KEYWORD, ReferenceParser::BACKWARDS_KEYWORD_LEN) == 0) {
        pos += ReferenceParser::BACKWARDS_KEYWORD_LEN;
        seg.s_cast_get<SearchReference>()->setSearchDirection(false);
        pos += ReferenceParser::skipSpaces(qualifier+pos);
      }
      if (qualifier[pos] != CHR('}')) {
        throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::Reference::parseQualifier"),
                                       STR("Invalid qualifier."), qualifier);
      }
      ++pos;
    } else if (compareStr(qualifier+pos, ReferenceParser::EVAL_KEYWORD, ReferenceParser::EVAL_KEYWORD_LEN) == 0) {
      //
      // IndirectReference
      //
      pos += ReferenceParser::EVAL_KEYWORD_LEN;
      Int i;
      // Parse a reference id.
      try {
        seg = std::make_shared<IndirectReference>(ReferenceParser::_parseQualifier(qualifier+pos, i, criteria));
        seg->setUsageCriteria(criteria);
      } catch (InvalidArgumentException &e) {
        if (e.getArgumentName() == STR("qualifier")) {
          throw InvalidArgumentException(e.getArgumentName().c_str(), e.getLocation().c_str(),
                                         e.getComment().c_str(), e.getArgumentValue());
        } else {
          throw e;
        }
      }
      pos += i;
      // Now we need to skip the closing bracket, after making sure it exists.
      if (qualifier[pos] != CHR('}')) {
        throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::Reference::parseQualifier"),
                                       STR("Invalid qualifier."), qualifier);
      }
      ++pos;
    } else {
      throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::ReferenceParser::parseQualifier"),
                                     STR("Invalid qualifier."), qualifier);
    }

    // Check if a result validator is attached to the segment.
    if (compareStr(qualifier+pos, ReferenceParser::WHERE_KEYWORD, ReferenceParser::WHERE_KEYWORD_LEN) == 0) {
      pos += ReferenceParser::WHERE_KEYWORD_LEN;
      auto resultValidator = StrAttributeValidator::create();
      pos += ReferenceParser::parseStrAttributeValidator(qualifier+pos, resultValidator.get());
      if (qualifier[pos] == CHR('&') || qualifier[pos] == CHR('|')) {
        throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::Reference::parseQualifier"),
                                       STR("Logical operations in validators are not supported yet."), qualifier);
      }
      seg->setResultValidator(resultValidator);
      if (qualifier[pos] != CHR(')')) {
        throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::Reference::parseQualifier"),
                                       STR("Invalid qualifier."), qualifier);
      }
      ++pos;
    }

    if (lastSeg == 0) {
      ref = seg;
    } else {
      lastSeg->setNext(seg);
    }
    lastSeg = seg;

    if (qualifier[pos] == CHR('.') || qualifier[pos] == CHR(':')) pos++;
    else break;
  }

  ASSERT(ref != 0);

  // At this point we should've reached the end of the qualifier or subqualifier.
  if (qualifier[pos] != CHR('\0') && (qualifier[pos] != CHR('}') || pos == 0)) {
    throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::ReferenceParser::parseQualifier"),
                                   STR("Invalid qualifier."), qualifier);
  }

  return ref;
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
Reference const& ReferenceParser::parseQualifierSegment(Char const *&qualifier)
{
  Int i;
  // Validation.
  if (qualifier == 0) {
    throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::ReferenceParser::parseQualifierSegment"),
                                   STR("Cannot be null."));
  }

  if ((*qualifier >= CHR('a') && *qualifier <= CHR('z')) ||
      (*qualifier >= CHR('A') && *qualifier <= CHR('Z')) ||
      *qualifier == CHR('_')) {
    //
    // StrKeyReference or ScopeReference or SelfReference
    //
    i = 0;
    while ((qualifier[i] >= CHR('a') && qualifier[i] <= CHR('z')) ||
           (qualifier[i] >= CHR('A') && qualifier[i] <= CHR('Z')) ||
           (qualifier[i] >= CHR('0') && qualifier[i] <= CHR('9')) ||
           qualifier[i] == CHR('_')) i++;
    if (qualifier[i] == CHR(':')) {
      this->tempScopeReference.setScope(qualifier, i);
      if (i == ReferenceParser::ROOT_KEYWORD_LEN &&
          compareStr(qualifier, ReferenceParser::ROOT_KEYWORD, i) == 0) {
        this->tempScopeReference.setSearchDirection(true);
      } else {
        this->tempScopeReference.setSearchDirection(false);
      }
      qualifier += i;
      this->setQualifierSegmentResultValidator(qualifier, &this->tempScopeReference);
      return this->tempScopeReference;
    } else if (i == ReferenceParser::SELF_KEYWORD_LEN &&
               compareStr(qualifier, ReferenceParser::SELF_KEYWORD, i) == 0) {
      qualifier += i;
      this->setQualifierSegmentResultValidator(qualifier, &this->tempSelfReference);
      return this->tempSelfReference;
    } else {
      this->tempStrKeyReference.setKey(qualifier, i);
      qualifier += i;
      this->setQualifierSegmentResultValidator(qualifier, &this->tempStrKeyReference);
      return this->tempStrKeyReference;
    }
  } else if (*qualifier == CHR('(')) {
    //
    // IndexReference
    //
    Int j = 0;
    ++qualifier;
    qualifier += ReferenceParser::parseInt(qualifier, j);
    this->tempIndexReference.setIndex(j);
    if (*qualifier != CHR(')')) {
      throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::Reference::parseQualifierSegment"),
                                     STR("Invalid qualifier."), qualifier);
    }
    ++qualifier;
    this->setQualifierSegmentResultValidator(qualifier, &this->tempSearchReference);
    return this->tempIndexReference;
  } else if (*qualifier >= CHR('0') && *qualifier <= CHR('9')) {
    //
    // IndexReference
    //
    Int j = 0;
    qualifier += ReferenceParser::parseInt(qualifier, j);
    this->tempIndexReference.setIndex(j);
    this->setQualifierSegmentResultValidator(qualifier, &this->tempIndexReference);
    return this->tempIndexReference;
  } else if (compareStr(qualifier, ReferenceParser::FIND_KEYWORD, ReferenceParser::FIND_KEYWORD_LEN) == 0) {
    //
    // SearchReference
    //
    qualifier += ReferenceParser::FIND_KEYWORD_LEN;
    if (this->tempSearchValidator == 0) this->tempSearchValidator = StrAttributeValidator::create();
    qualifier += ReferenceParser::parseStrAttributeValidator(qualifier, this->tempSearchValidator.get());
    this->tempSearchReference.setSearchValidator(this->tempSearchValidator);
    if (*qualifier == CHR('&') || *qualifier == CHR('|')) {
      throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::Reference::parseQualifierSegment"),
                                     STR("Logical operations in validators are not supported yet."), qualifier);
    }
    // Check for a match limitation parameter.
    if (*qualifier == CHR(',')) {
      ++qualifier;
      qualifier += ReferenceParser::skipSpaces(qualifier);
      Int j = 0;
      while (*qualifier >= CHR('0') && *qualifier <= CHR('9')) {
        j *= 10;
        j += static_cast<Int>(*qualifier - CHR('0'));
        ++qualifier;
      }
      this->tempSearchReference.setMatchLimitation(j);
      qualifier += ReferenceParser::skipSpaces(qualifier);
    }
    // Check if the search direction should be backwards.
    if (compareStr(qualifier, ReferenceParser::BACKWARDS_KEYWORD, ReferenceParser::BACKWARDS_KEYWORD_LEN) == 0) {
      qualifier += ReferenceParser::BACKWARDS_KEYWORD_LEN;
      this->tempSearchReference.setSearchDirection(false);
      qualifier += ReferenceParser::skipSpaces(qualifier);
    }
    if (*qualifier != CHR('}')) {
      throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::Reference::parseQualifierSegment"),
                                     STR("Invalid qualifier."), qualifier);
    }
    ++qualifier;
    this->setQualifierSegmentResultValidator(qualifier, &this->tempSearchReference);
    return this->tempSearchReference;
  } else if (compareStr(qualifier, ReferenceParser::EVAL_KEYWORD, ReferenceParser::EVAL_KEYWORD_LEN) == 0) {
    throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::ReferenceParser::parseQualifierSegment"),
                                   STR("Indirect references are not supported by this function."), qualifier);
  } else {
    throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::ReferenceParser::parseQualifierSegment"),
                                   STR("Invalid qualifier."), qualifier);
  }
}


void ReferenceParser::setQualifierSegmentResultValidator(Char const *&qualifier, Reference *seg)
{
  // Check if a result validator is attached to the segment.
  if (compareStr(qualifier, ReferenceParser::WHERE_KEYWORD, ReferenceParser::WHERE_KEYWORD_LEN) == 0) {
    qualifier += ReferenceParser::WHERE_KEYWORD_LEN;
    if (this->tempResultValidator == 0) this->tempResultValidator = StrAttributeValidator::create();
    qualifier += ReferenceParser::parseStrAttributeValidator(qualifier, this->tempResultValidator.get());
    if (*qualifier == CHR('&') || *qualifier == CHR('|')) {
      throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::Reference::setQualifierSegmentResultValidator"),
                                     STR("Logical operations in validators are not supported yet."), qualifier);
    }
    seg->setResultValidator(this->tempResultValidator);
    if (*qualifier != CHR(')')) {
      throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::Reference::setQualifierSegmentResultValidator"),
                                     STR("Invalid qualifier."), qualifier);
    }
    ++qualifier;
  } else {
    seg->setResultValidator(SharedPtr<Validator>());
  }
}


/**
 * Called by the public validateQualifier funciton. This recursive function
 * performas the actual validation of the qualifier string.
 * @param absolute Specifies whether to validate an absolute qualifier or not.
 *                 An absolute qualifier is one that doesn't use a subqualifier,
 *                 i.e. doesn't have any brackets.
 * @param end A pointer to a pointer. If this pointer is not null the function
 *            will update its value with the position within the string where
 *            validation reached. This is needed during the validation of
 *            subqualifiers. If this value is null the function will know tht
 *            it's not validation a subqualifier and hence will refuse any
 *            closing brackets it may encounters.
 */
Bool ReferenceParser::_validateQualifier(Char const *qualifier, Bool absolute, Char const **end)
{
  // Validation.
  if (qualifier == 0) {
    throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::ReferenceParser::validateQualifier"),
                                   STR("Cannot be null."));
  }

  // Check the first character to determine the route to take.
  if (*qualifier == CHR('(') && !absolute) {
    // Validate the subqualifier.
    Int j = 0;
    ++qualifier;
    qualifier += ReferenceParser::parseInt(qualifier, j);
    // Now we need to skip the closing bracket, after making sure it exists.
    if (*qualifier != CHR(')')) return false;
    ++qualifier;
  } else if (*qualifier >= CHR('0') && *qualifier <= CHR('9')) {
    // IndexReference.
    while (*qualifier >= CHR('0') && *qualifier <= CHR('9')) ++qualifier;
  } else if ((*qualifier >= CHR('a') && *qualifier <= CHR('z')) ||
             (*qualifier >= CHR('A') && *qualifier <= CHR('Z')) ||
             *qualifier == CHR('_')) {
    // StrKeyReference or ScopeReference
    while ((*qualifier >= CHR('a') && *qualifier <= CHR('z')) ||
           (*qualifier >= CHR('A') && *qualifier <= CHR('Z')) ||
           (*qualifier >= CHR('0') && *qualifier <= CHR('9')) ||
           *qualifier == CHR('_')) ++qualifier;
  } else if (compareStr(qualifier, ReferenceParser::FIND_KEYWORD, ReferenceParser::FIND_KEYWORD_LEN) == 0) {
    // SearchReference.
    // Skip the validator part.
    qualifier += ReferenceParser::FIND_KEYWORD_LEN;
    qualifier += ReferenceParser::skipSpaces(qualifier);
    Int i = 0;
    while (qualifier[i]!=CHR('=') && qualifier[i]!=CHR('&') && qualifier[i]!=CHR('|') &&
           qualifier[i]!=CHR(')') && qualifier[i]!=CHR('}') &&
           qualifier[i]!=CHR(' ') && qualifier[i]!=CHR('\t') &&
           qualifier[i]!=CHR(',') && qualifier[i]!=CHR('\0')) ++i;
    if (i == 0) return false;
    qualifier += i;
    qualifier += ReferenceParser::skipSpaces(qualifier);
    if (*qualifier != CHR('=')) return false;
    ++qualifier;
    qualifier += ReferenceParser::skipSpaces(qualifier);
    i = 0;
    while (qualifier[i]!=CHR('&') && qualifier[i]!=CHR('|') &&
           qualifier[i]!=CHR(')') && qualifier[i]!=CHR('}') &&
           qualifier[i]!=CHR(' ') && qualifier[i]!=CHR('\t') &&
           qualifier[i]!=CHR(',') && qualifier[i]!=CHR('\0')) ++i;
    if (i == 0) return false;
    qualifier += i;
    qualifier += ReferenceParser::skipSpaces(qualifier);
    if (*qualifier == CHR('&') || *qualifier == CHR('|')) return false;
    // Skip match limitation parameter.
    if (*qualifier == CHR(',')) {
      ++qualifier;
      qualifier += ReferenceParser::skipSpaces(qualifier);
      while (*qualifier >= CHR('0') && *qualifier <= CHR('9')) ++qualifier;
      qualifier += ReferenceParser::skipSpaces(qualifier);
    }
    // Check if the search direction should be backwards.
    if (compareStr(qualifier, ReferenceParser::BACKWARDS_KEYWORD, ReferenceParser::BACKWARDS_KEYWORD_LEN) == 0) {
      qualifier += ReferenceParser::BACKWARDS_KEYWORD_LEN;
      qualifier += ReferenceParser::skipSpaces(qualifier);
    }
    if (*qualifier != CHR('}')) return false;
    ++qualifier;
  } else if (compareStr(qualifier, ReferenceParser::EVAL_KEYWORD, ReferenceParser::EVAL_KEYWORD_LEN) == 0) {
    // Validate the subqualifier.
    qualifier += ReferenceParser::EVAL_KEYWORD_LEN;
    if (!ReferenceParser::_validateQualifier(qualifier, absolute, &qualifier)) return false;
    // Now we need to skip the closing bracket, after making sure it exists.
    if (*qualifier != CHR('}')) return false;
    ++qualifier;
  } else {
    return false;
  }

  // Parse result validation.
  if (compareStr(qualifier, ReferenceParser::WHERE_KEYWORD, ReferenceParser::WHERE_KEYWORD_LEN) == 0) {
    qualifier += ReferenceParser::WHERE_KEYWORD_LEN;
    qualifier += ReferenceParser::skipSpaces(qualifier);
    Int i = 0;
    while (qualifier[i]!=CHR('=') && qualifier[i]!=CHR('&') && qualifier[i]!=CHR('|') &&
           qualifier[i]!=CHR(')') && qualifier[i]!=CHR('}') &&
           qualifier[i]!=CHR(' ') && qualifier[i]!=CHR('\t') &&
           qualifier[i]!=CHR(',') && qualifier[i]!=CHR('\0')) ++i;
    if (i == 0) return false;
    qualifier += i;
    qualifier += ReferenceParser::skipSpaces(qualifier);
    if (*qualifier != CHR('=')) return false;
    ++qualifier;
    qualifier += ReferenceParser::skipSpaces(qualifier);
    i = 0;
    while (qualifier[i]!=CHR('&') && qualifier[i]!=CHR('|') &&
           qualifier[i]!=CHR(')') && qualifier[i]!=CHR('}') &&
           qualifier[i]!=CHR(' ') && qualifier[i]!=CHR('\t') &&
           qualifier[i]!=CHR(',') && qualifier[i]!=CHR('\0')) ++i;
    if (i == 0) return false;
    qualifier += i;
    qualifier += ReferenceParser::skipSpaces(qualifier);
    if (*qualifier != CHR(')')) return false;
    ++qualifier;
  }

  // Parse child references, if any.
  if (*qualifier == CHR('.') || *qualifier == CHR(':')) {
    if (!ReferenceParser::_validateQualifier(qualifier+1, absolute, &qualifier)) return false;
  }

  // At this point we should've reached the end of the qualifier or subqualifier.
  if (*qualifier != CHR('\0') && (absolute || *qualifier != CHR('}') || end == 0)) return false;

  if (end != 0) *end = qualifier;
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
Char const* ReferenceParser::_findLastQualifierSegment(Char const *qualifier)
{
  // Validation.
  if (qualifier == 0) {
    throw InvalidArgumentException(STR("qualifier"),
                                   STR("Core::Data::ReferenceParser::_findLastQualifierSegment"),
                                   STR("Cannot be null."));
  }

  Char const *beginning = qualifier;

  // Check the first character to determine the route to take.
  if (*qualifier == CHR('(')) {
    // Skip the subqualifier.
    Int j = 0;
    ++qualifier;
    qualifier += ReferenceParser::parseInt(qualifier, j);
    // Now we need to skip the closing bracket, after making sure it exists.
    if (*qualifier != CHR(')')) {
      throw InvalidArgumentException(STR("qualifier"),
                                     STR("Core::Data::ReferenceParser::_findLastQualifierSegment"),
                                     STR("Invalid qualifier."), qualifier);
    }
    ++qualifier;
  } else if (*qualifier >= CHR('0') && *qualifier <= CHR('9')) {
    // Skip IndexReference.
    while (*qualifier >= CHR('0') && *qualifier <= CHR('9')) ++qualifier;
  } else if ((*qualifier >= CHR('a') && *qualifier <= CHR('z')) ||
             (*qualifier >= CHR('A') && *qualifier <= CHR('Z')) ||
             *qualifier == CHR('_')) {
    // Skip StrKeyReference or ScopeReference.
    while ((*qualifier >= CHR('a') && *qualifier <= CHR('z')) ||
           (*qualifier >= CHR('A') && *qualifier <= CHR('Z')) ||
           (*qualifier >= CHR('0') && *qualifier <= CHR('9')) ||
           *qualifier == CHR('_')) ++qualifier;
  } else if (compareStr(qualifier, ReferenceParser::FIND_KEYWORD, ReferenceParser::FIND_KEYWORD_LEN) == 0) {
    // Skip SearchReference.
    // Skip the validator part.
    qualifier += ReferenceParser::FIND_KEYWORD_LEN;
    qualifier += ReferenceParser::skipSpaces(qualifier);
    Int i = 0;
    while (qualifier[i]!=CHR('=') && qualifier[i]!=CHR('&') && qualifier[i]!=CHR('|') &&
           qualifier[i]!=CHR(')') && qualifier[i]!=CHR('}') &&
           qualifier[i]!=CHR(' ') && qualifier[i]!=CHR('\t') &&
           qualifier[i]!=CHR(',') && qualifier[i]!=CHR('\0')) ++i;
    if (i == 0) {
      throw InvalidArgumentException(STR("qualifier"),
                                     STR("Core::Data::ReferenceParser::_findLastQualifierSegment"),
                                     STR("Invalid qualifier."), qualifier);
    }
    qualifier += i;
    qualifier += ReferenceParser::skipSpaces(qualifier);
    if (*qualifier != CHR('=')) {
      throw InvalidArgumentException(STR("qualifier"),
                                     STR("Core::Data::ReferenceParser::_findLastQualifierSegment"),
                                     STR("Invalid qualifier."), qualifier);
    }
    ++qualifier;
    qualifier += ReferenceParser::skipSpaces(qualifier);
    i = 0;
    while (qualifier[i]!=CHR('&') && qualifier[i]!=CHR('|') &&
           qualifier[i]!=CHR(')') && qualifier[i]!=CHR('}') &&
           qualifier[i]!=CHR(' ') && qualifier[i]!=CHR('\t') &&
           qualifier[i]!=CHR(',') && qualifier[i]!=CHR('\0')) ++i;
    if (i == 0) {
      throw InvalidArgumentException(STR("qualifier"),
                                     STR("Core::Data::ReferenceParser::_findLastQualifierSegment"),
                                     STR("Invalid qualifier."), qualifier);
    }
    qualifier += i;
    qualifier += ReferenceParser::skipSpaces(qualifier);
    if (*qualifier == CHR('&') || *qualifier == CHR('|')) {
      throw InvalidArgumentException(STR("qualifier"),
                                     STR("Core::Data::ReferenceParser::_findLastQualifierSegment"),
                                     STR("Logical operations in validators are not supported yet."), qualifier);
    }
    // Skip match limitation parameter.
    if (*qualifier == CHR(',')) {
      ++qualifier;
      qualifier += ReferenceParser::skipSpaces(qualifier);
      while (*qualifier >= CHR('0') && *qualifier <= CHR('9')) ++qualifier;
      qualifier += ReferenceParser::skipSpaces(qualifier);
    }
    // Skip search direction keyword.
    if (compareStr(qualifier, ReferenceParser::BACKWARDS_KEYWORD, ReferenceParser::BACKWARDS_KEYWORD_LEN) == 0) {
      qualifier += ReferenceParser::BACKWARDS_KEYWORD_LEN;
      qualifier += ReferenceParser::skipSpaces(qualifier);
    }
    if (*qualifier != CHR('}')) {
      throw InvalidArgumentException(STR("qualifier"),
                                     STR("Core::Data::ReferenceParser::_findLastQualifierSegment"),
                                     STR("Invalid qualifier."), qualifier);
    }
    ++qualifier;
  } else if (compareStr(qualifier, ReferenceParser::EVAL_KEYWORD, ReferenceParser::EVAL_KEYWORD_LEN) == 0) {
    // Skip the subqualifier.
    qualifier += ReferenceParser::EVAL_KEYWORD_LEN;
    qualifier = ReferenceParser::_findLastQualifierSegment(qualifier);
    // Now we need to skip the closing bracket, after making sure it exists.
    if (*qualifier != CHR('}')) {
      throw InvalidArgumentException(STR("qualifier"),
                                     STR("Core::Data::ReferenceParser::_findLastQualifierSegment"),
                                     STR("Invalid qualifier."), qualifier);
    }
    ++qualifier;
  } else {
    throw InvalidArgumentException(STR("qualifier"),
                                   STR("Core::Data::ReferenceParser::_findLastQualifierSegment"),
                                   STR("Invalid qualifier."));
  }

  // Skip result validation.
  if (compareStr(qualifier, ReferenceParser::WHERE_KEYWORD, ReferenceParser::WHERE_KEYWORD_LEN) == 0) {
    qualifier += ReferenceParser::WHERE_KEYWORD_LEN;
    qualifier += ReferenceParser::skipSpaces(qualifier);
    Int i = 0;
    while (qualifier[i]!=CHR('=') && qualifier[i]!=CHR('&') && qualifier[i]!=CHR('|') &&
           qualifier[i]!=CHR(')') && qualifier[i]!=CHR('}') &&
           qualifier[i]!=CHR(' ') && qualifier[i]!=CHR('\t') &&
           qualifier[i]!=CHR(',') && qualifier[i]!=CHR('\0')) ++i;
    if (i == 0) {
      throw InvalidArgumentException(STR("qualifier"),
                                     STR("Core::Data::ReferenceParser::_findLastQualifierSegment"),
                                     STR("Invalid qualifier."), qualifier);
    }
    qualifier += i;
    qualifier += ReferenceParser::skipSpaces(qualifier);
    if (*qualifier != CHR('=')) {
      throw InvalidArgumentException(STR("qualifier"),
                                     STR("Core::Data::ReferenceParser::_findLastQualifierSegment"),
                                     STR("Invalid qualifier."), qualifier);
    }
    ++qualifier;
    qualifier += ReferenceParser::skipSpaces(qualifier);
    i = 0;
    while (qualifier[i]!=CHR('&') && qualifier[i]!=CHR('|') &&
           qualifier[i]!=CHR(')') && qualifier[i]!=CHR('}') &&
           qualifier[i]!=CHR(' ') && qualifier[i]!=CHR('\t') &&
           qualifier[i]!=CHR(',') && qualifier[i]!=CHR('\0')) ++i;
    if (i == 0) {
      throw InvalidArgumentException(STR("qualifier"),
                                     STR("Core::Data::ReferenceParser::_findLastQualifierSegment"),
                                     STR("Invalid qualifier."), qualifier);
    }
    qualifier += i;
    qualifier += ReferenceParser::skipSpaces(qualifier);
    if (*qualifier != CHR(')')) {
      throw InvalidArgumentException(STR("qualifier"),
                                     STR("Core::Data::ReferenceParser::_findLastQualifierSegment"),
                                     STR("Invalid qualifier."), qualifier);
    }
    ++qualifier;
  }

  if (*qualifier == CHR('.') || *qualifier == CHR(':')) {
    // We found a segment separator, so start again at the new segment.
    return ReferenceParser::_findLastQualifierSegment(qualifier+1);
  } else if (*qualifier == CHR('}')) {
    // We found a closing bracket, so we'll assume we are currently within a recursion and will simply return the
    // position of the closing bracket so that the caller can continue searching after the closing bracket.
    return qualifier;
  } else if (*qualifier == CHR('\0')) {
    // Ok, we found a string termination, so whatever we received in the function argument was actually the
    // beginning of the last segment.
    return beginning;
  } else {
    throw InvalidArgumentException(STR("qualifier"),
                                   STR("Core::Data::ReferenceParser::find_last_qualifier_segment"),
                                   STR("Invalid qualifier."));
  }
}


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
  // Parse the name of the expression.
  Int pos = ReferenceParser::skipSpaces(qualifier);
  Int i = pos;
  while (qualifier[i]!=CHR('=') && qualifier[i]!=CHR('&') && qualifier[i]!=CHR('|') &&
         qualifier[i]!=CHR(')') && qualifier[i]!=CHR('}') &&
         qualifier[i]!=CHR(' ') && qualifier[i]!=CHR('\t') &&
         qualifier[i]!=CHR(',') && qualifier[i]!=CHR('\0')) ++i;
  if (i == pos) {
    throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::ReferenceParser::parseStrAttributeValidator"),
                                   STR("Invalid validator expression."), qualifier);
  }
  targetValidator->setName(qualifier+pos, i-pos);
  pos = i + ReferenceParser::skipSpaces(qualifier+i);
  if (qualifier[pos] != CHR('=')) {
    throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::ReferenceParser::parseStrAttributeValidator"),
                                   STR("Invalid validator expression."), qualifier);
  }
  // skip the = sign.
  ++pos;
  // Parse the value of the expression.
  pos += ReferenceParser::skipSpaces(qualifier+pos);
  i = pos;
  while (qualifier[i]!=CHR('&') && qualifier[i]!=CHR('|') &&
         qualifier[i]!=CHR(')') && qualifier[i]!=CHR('}') &&
         qualifier[i]!=CHR(' ') && qualifier[i]!=CHR('\t') &&
         qualifier[i]!=CHR(',') && qualifier[i]!=CHR('\0')) ++i;
  if (i == pos) {
    throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::ReferenceParser::parseStrAttributeValidator"),
                                   STR("Invalid validator expression."), qualifier);
  }
  targetValidator->setValue(qualifier+pos, i-pos);
  pos = i + ReferenceParser::skipSpaces(qualifier+i);
  return pos;
}

} } // namespace
