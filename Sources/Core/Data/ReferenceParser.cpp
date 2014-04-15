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
void ReferenceParser::buildQualifier(Reference *ref, StrStream &qualifier)
{
  if (ref == 0) {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::ReferenceParser::buildQualifier"),
                                   STR("Cannot be null."));
  }
  switch (ref->getScope().val) {
    case ReferenceScope::ARGS: qualifier << "args."; break;
    case ReferenceScope::STACK: qualifier << "stack."; break;
    case ReferenceScope::MODULE: qualifier << "module."; break;
    case ReferenceScope::PMODULE: qualifier << "pmodule."; break;
    case ReferenceScope::ROOT: qualifier << "root."; break;
  }

  ReferenceSegment *seg = ref->getSegment().get();
  if (seg == 0) {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::ReferenceParser::buildQualifier"),
                                   STR("Reference contains no segments."));
  }

  while (seg != 0) {
    if (seg->isA<IntReferenceSegment>()) {
      qualifier << static_cast<IntReferenceSegment*>(seg)->getIndex();
    } else if (seg->isA<StrReferenceSegment>()) {
      qualifier << static_cast<StrReferenceSegment*>(seg)->getKey();
    } else if (seg->isA<IndirectReferenceSegment>()) {
      qualifier << CHR('(');
      ReferenceParser::buildQualifier(static_cast<IndirectReferenceSegment*>(seg)->getKey().get(), qualifier);
      qualifier << CHR(')');
    } else if (seg->isA<RawIndirectReferenceSegment>()) {
      RawIndirectReferenceSegment *iseg = static_cast<RawIndirectReferenceSegment*>(seg);
      qualifier << CHR('(');
      qualifier << Str(iseg->getQualifier(), iseg->getSize());
      qualifier << CHR(')');
    } else {
      throw GeneralException(STR("Provided reference contained an invalid segment type."),
                             STR("Core::Data::ReferenceParser::buildQualifier"));
    }
    seg = seg->getNext().get();
    if (seg != 0) qualifier << CHR('.');
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
 * id = string | integer | "(" + qualifier + ")"; <br/>
 * integer = ('0'..'9')*v1; </br>
 * string = ('a'..'z'|'A'..'Z'|'_') + ('a'..'z'|'A'..'Z'|'0'..'9'|'_')*v;
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
SharedPtr<Reference> ReferenceParser::_parseQualifier(const Char *qualifier, Int &pos)
{
  // Validation.
  if (qualifier == 0) {
    throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::ReferenceParser::parseQualifier"),
                                   STR("Cannot be null."));
  }

  SharedPtr<Reference> ref = std::make_shared<Reference>();
  pos=0;

  // Handle reference type ("root.", "this.", and "stack.").
  if (compareStr(qualifier, STR("root."), 5) == 0) {
    ref->setScope(ReferenceScope::ROOT);
    pos += 5;
  } else if (compareStr(qualifier, STR("module."), 7) == 0) {
    ref->setScope(ReferenceScope::MODULE);
    pos += 7;
  } else if (compareStr(qualifier, STR("pmodule."), 8) == 0) {
    ref->setScope(ReferenceScope::PMODULE);
    pos += 8;
  } else if (compareStr(qualifier, STR("stack."), 6) == 0) {
    ref->setScope(ReferenceScope::STACK);
    pos += 6;
  } else if (compareStr(qualifier, STR("args."), 5) == 0) {
    ref->setScope(ReferenceScope::ARGS);
    pos += 5;
  }

  SharedPtr<ReferenceSegment> seg;
  SharedPtr<ReferenceSegment> lastSeg;

  while (true) {
    // Check the first character to determine the route to take.
    if (qualifier[pos] == CHR('(')) {
      ++pos;
      Int i;
      // Parse a reference id.
      try {
        seg = std::make_shared<IndirectReferenceSegment>(ReferenceParser::_parseQualifier(qualifier+pos, i));
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
      if (qualifier[pos] != CHR(')')) {
        throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::Reference::parseQualifier"),
                                       STR("Invalid qualifier."), qualifier);
      }
      ++pos;
    } else if ((qualifier[pos] >= CHR('a') && qualifier[pos] <= CHR('z')) ||
               (qualifier[pos] >= CHR('A') && qualifier[pos] <= CHR('Z')) ||
               qualifier[pos] == CHR('_')) {
      // Parse a string id.
      Int i = 0;
      while ((qualifier[pos+i] >= CHR('a') && qualifier[pos+i] <= CHR('z')) ||
             (qualifier[pos+i] >= CHR('A') && qualifier[pos+i] <= CHR('Z')) ||
             (qualifier[pos+i] >= CHR('0') && qualifier[pos+i] <= CHR('9')) ||
             qualifier[pos+i] == CHR('_')) i++;
      seg = std::make_shared<StrReferenceSegment>(qualifier+pos, i);
      pos += i;
    } else if (qualifier[pos] >= CHR('0') && qualifier[pos] <= CHR('9')) {
      // Parse an integer id.
      Int j = 0;
      while (qualifier[pos] >= CHR('0') && qualifier[pos] <= CHR('9')) {
        j *= 10;
        j += static_cast<Int>(qualifier[pos] - CHR('0'));
        ++pos;
      }
      seg = std::make_shared<IntReferenceSegment>(j);
    } else {
      throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::ReferenceParser::parseQualifier"),
                                     STR("Invalid qualifier."), qualifier);
    }
    if (lastSeg == 0) {
      ref->setSegment(seg);
    } else {
      lastSeg->setNext(seg);
    }
    lastSeg = seg;

    if (qualifier[pos] == CHR('.')) pos++;
    else break;
  }

  // At this point we should've reached the end of the qualifier or subqualifier.
  if (qualifier[pos] != CHR('\0') && (qualifier[pos] != CHR(')') || pos == 0)) {
    throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::ReferenceParser::parseQualifier"),
                                   STR("Invalid qualifier."), qualifier);
  }

  return ref;
}


/**
 * Parse the scope part of a qualifier string and returns a ReferenceScope
 * value representing that scope. The qualifier pointer will also be
 * incremented to point to the part immediately after the scope specifier.
 */
ReferenceScope ReferenceParser::parseQualifierScope(const Char *&qualifier)
{
  if (compareStr(qualifier, STR("root."), 5) == 0) {
    qualifier += 5;
    return ReferenceScope::ROOT;
  } else if (compareStr(qualifier, STR("module."), 7) == 0) {
    qualifier += 7;
    return ReferenceScope::MODULE;
  } else if (compareStr(qualifier, STR("pmodule."), 8) == 0) {
    qualifier += 8;
    return ReferenceScope::PMODULE;
  } else if (compareStr(qualifier, STR("stack."), 6) == 0) {
    qualifier += 6;
    return ReferenceScope::STACK;
  } else if (compareStr(qualifier, STR("args."), 5) == 0) {
    qualifier += 5;
    return ReferenceScope::ARGS;
  } else {
    return ReferenceScope::UNKNOWN;
  }
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
ReferenceSegment* ReferenceParser::parseQualifierSegment(const Char *&qualifier)
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
    // Parse a string id.
    i = 0;
    while ((qualifier[i] >= CHR('a') && qualifier[i] <= CHR('z')) ||
           (qualifier[i] >= CHR('A') && qualifier[i] <= CHR('Z')) ||
           (qualifier[i] >= CHR('0') && qualifier[i] <= CHR('9')) ||
           qualifier[i] == CHR('_')) i++;
    this->tempStrSegment.setKey(qualifier, i);
    qualifier += i;
    return &this->tempStrSegment;
  } else if (*qualifier >= CHR('0') && *qualifier <= CHR('9')) {
    // Parse an integer id.
    i = 0;
    Int j = 0;
    while (qualifier[i] >= CHR('0') && qualifier[i] <= CHR('9')) {
      j *= 10;
      j += static_cast<Int>(qualifier[i] - CHR('0'));
      i++;
    }
    this->tempIntSegment.setIndex(j);
    qualifier += i;
    return &this->tempIntSegment;
  } else {
    throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::ReferenceParser::parseQualifierSegment"),
                                   STR("Invalid qualifier."), qualifier);
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
Bool ReferenceParser::_validateQualifier(const Char *qualifier, Bool absolute, const Char **end)
{
  // Validation.
  if (qualifier == 0) {
    throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::ReferenceParser::validateQualifier"),
                                   STR("Cannot be null."));
  }

  // Check the first character to determine the route to take.
  if (*qualifier == CHR('(') && !absolute) {
    // Validate the subqualifier.
    if (!ReferenceParser::_validateQualifier(qualifier+1, absolute, &qualifier)) return false;
    // Now we need to skip the closing bracket, after making sure it exists.
    if (*qualifier != CHR(')')) return false;
    ++qualifier;
  } else if ((*qualifier >= CHR('a') && *qualifier <= CHR('z')) ||
             (*qualifier >= CHR('A') && *qualifier <= CHR('Z')) ||
             *qualifier == CHR('_')) {
    // Skip the string part.
    while ((*qualifier >= CHR('a') && *qualifier <= CHR('z')) ||
           (*qualifier >= CHR('A') && *qualifier <= CHR('Z')) ||
           (*qualifier >= CHR('0') && *qualifier <= CHR('9')) ||
           *qualifier == CHR('_')) ++qualifier;
  } else if (*qualifier >= CHR('0') && *qualifier <= CHR('9')) {
    // Skip the integer part.
    while (*qualifier >= CHR('0') && *qualifier <= CHR('9')) ++qualifier;
  } else {
    return false;
  }

  // Parse child references, if any.
  if (*qualifier == CHR('.')) {
    if (!ReferenceParser::_validateQualifier(qualifier+1, absolute, &qualifier)) return false;
  }

  // At this point we should've reached the end of the qualifier or subqualifier.
  if (*qualifier != CHR('\0') && (absolute || *qualifier != CHR(')') || end == 0)) return false;

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
const Char* ReferenceParser::_findLastQualifierSegment(const Char *qualifier)
{
  // Validation.
  if (qualifier == 0) {
    throw InvalidArgumentException(STR("qualifier"),
                                   STR("Core::Data::ReferenceParser::find_last_qualifier_segment"),
                                   STR("Cannot be null."));
  }

  const Char *beginning = qualifier;

  // Check the first character to determine the route to take.
  if (*qualifier == CHR('(')) {
    // Validate the subqualifier.
    qualifier = ReferenceParser::_findLastQualifierSegment(qualifier+1);
    // Now we need to skip the closing bracket, after making sure it exists.
    if (*qualifier != CHR(')')) {
      throw InvalidArgumentException(STR("qualifier"),
                                     STR("Core::Data::ReferenceParser::find_last_qualifier_segment"),
                                     STR("Invalid qualifier."));
    }
    ++qualifier;
  } else if ((*qualifier >= CHR('a') && *qualifier <= CHR('z')) ||
             (*qualifier >= CHR('A') && *qualifier <= CHR('Z')) ||
             *qualifier == CHR('_')) {
    // Skip the string part.
    while ((*qualifier >= CHR('a') && *qualifier <= CHR('z')) ||
           (*qualifier >= CHR('A') && *qualifier <= CHR('Z')) ||
           (*qualifier >= CHR('0') && *qualifier <= CHR('9')) ||
           *qualifier == CHR('_')) ++qualifier;
  } else if (*qualifier >= CHR('0') && *qualifier <= CHR('9')) {
    // Skip the integer part.
    while (*qualifier >= CHR('0') && *qualifier <= CHR('9')) ++qualifier;
  } else {
    throw InvalidArgumentException(STR("qualifier"),
                                   STR("Core::Data::ReferenceParser::find_last_qualifier_segment"),
                                   STR("Invalid qualifier."));
  }

  if (*qualifier == CHR('.')) {
    // We found a segment separator, so start again at the new segment.
    return ReferenceParser::_findLastQualifierSegment(qualifier+1);
  } else if (*qualifier == CHR(')')) {
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

} } // namespace
