/**
 * @file Core/Data/ReferenceParser.h
 * Contains the header of class Core::Data::ReferenceParser.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_REFERENCE_PARSER_H
#define DATA_REFERENCE_PARSER_H

namespace Core { namespace Data
{

// TODO: DOC

class ReferenceParser
{
  //============================================================================
  // Member Variables

  IntReferenceSegment tempIntSegment;

  StrReferenceSegment tempStrSegment;

  RawIndirectReferenceSegment tempIndirectSegment;


  //============================================================================
  // Member Functions

  /// Build the string representation of this reference.
  public: static void buildQualifier(Reference *ref, StrStream &qualifier);

  /// Build the string representation of this reference and returns it.
  public: static const Str getQualifier(Reference *ref)
  {
    StrStream stream;
    ReferenceParser::buildQualifier(ref, stream);
    return stream.str();
  }

  /**
     * @brief Builds a Reference chain that represents the given gualifier.
     * Parses the given string based qualifier into a valid chain of Reference
     * objects. The format is as follows: <br/>
     * qualifier = id + ("." + id)*v; <br/>
     * id = string | integer | "(" + qualifier + ")"; <br/>
     * integer = ('0'..'9')*v1; </br>
     * string = ('a'..'z'|'A'..'Z'|'_') + ('a'..'z'|'A'..'Z'|'0'..'9'|'_')*v;
     * <br/>
     * Brackets are used to use a reference as the id of another reference.
     * Strings are used to reference map items by their key, while integers are
     * used to reference list items by their index.
     */
  public: static SharedPtr<Reference> parseQualifier(const Char *qualifier)
  {
    Int pos;
    return ReferenceParser::_parseQualifier(qualifier, pos);
  }

  /// Internal recursive function that does the actual parsing of qualifiers.
  private: static SharedPtr<Reference> _parseQualifier(const Char *qualifier, Int &pos);

  /// Parses the scope part of a qualifier string.
  public: static ReferenceScope parseQualifierScope(const Char *&scope);

  /// Parses a single segment of the qualifier string.
  public: ReferenceSegment* parseQualifierSegment(const Char *&qualifier);

  /**
     * @brief Validate a qualifier string.
     * @param absolute Specifies whether to validate an absolute qualifier or
     *                 not. An absolute qualifier is one that doesn't use a
     *                 subqualifier, i.e. doesn't have any brackets.
     */
  public: static Bool validateQualifier(const Char *qualifier, Bool absolute)
  {
    return ReferenceParser::_validateQualifier(qualifier, absolute, 0);
  }

  /// Internal recursive function that does the actual validation.
  private: static Bool _validateQualifier(const Char *qualifier, Bool absolute, const Char **end);

  /**
     * @brief Find the beginning of the last segment within a qualifier string.
     * The last segument could be a subqualifier (a bracketed full qualifier) in
     * which case the return will be the opening bracket. If the last segment is
     * not a subqualifier the return will be the beginning of the segment, not
     * the dot preceding it.
     */
  public: static const Char* findLastQualifierSeguemt(const Char *qualifier)
  {
    qualifier = ReferenceParser::_findLastQualifierSegment(qualifier);
    if (*qualifier != CHR('\0')) {
      throw InvalidArgumentException(STR("qualifier"),
                                     STR("Core::Data::ReferenceParser::find_last_qualifier_segment"),
                                     STR("Invalid qualifier."));
    }
    return qualifier;
  }

  /// Internal recursive function to find the last qualifier segument.
  private: static const Char* _findLastQualifierSegment(const Char *qualifier);

}; // class

} } // namespace

#endif
