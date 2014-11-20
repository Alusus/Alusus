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

#ifndef DATA_REFERENCEPARSER_H
#define DATA_REFERENCEPARSER_H

namespace Core { namespace Data
{

// TODO: DOC

class ReferenceParser
{
  //============================================================================
  // Constants

  private: static constexpr Char const *ROOT_KEYWORD = STR("root");
  private: static constexpr Int ROOT_KEYWORD_LEN = 4;

  private: static constexpr Char const *SELF_KEYWORD = STR("self");
  private: static constexpr Int SELF_KEYWORD_LEN = 4;

  private: static constexpr Char const *FIND_KEYWORD = STR("{find ");
  private: static constexpr Int FIND_KEYWORD_LEN = 6;

  private: static constexpr Char const *EVAL_KEYWORD = STR("{eval ");
  private: static constexpr Int EVAL_KEYWORD_LEN = 6;

  private: static constexpr Char const *BACKWARDS_KEYWORD = STR("backwards");
  private: static constexpr Int BACKWARDS_KEYWORD_LEN = 9;

  private: static constexpr Char const *WHERE_KEYWORD = STR("~where(");
  private: static constexpr Int WHERE_KEYWORD_LEN = 7;


  //============================================================================
  // Member Variables

  private: IndexReference tempIndexReference;

  private: StrKeyReference tempStrKeyReference;

  private: ScopeReference tempScopeReference;

  private: SearchReference tempSearchReference;

  private: SelfReference tempSelfReference;

  private: SharedPtr<StrAttributeValidator> tempSearchValidator;

  private: SharedPtr<StrAttributeValidator> tempResultValidator;


  //============================================================================
  // Member Functions

  /// Build the string representation of this reference.
  public: static void buildQualifier(Reference const *ref, StrStream &qualifier);

  /// Build the string representation of this reference and returns it.
  public: static const Str getQualifier(Reference const *ref)
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
  public: static SharedPtr<Reference> parseQualifier(Char const *qualifier,
    ReferenceUsageCriteria criteria = ReferenceUsageCriteria::SINGLE_DATA_SINGLE_MATCH)
  {
    Int pos;
    return ReferenceParser::_parseQualifier(qualifier, pos, criteria);
  }

  /// Internal recursive function that does the actual parsing of qualifiers.
  private: static SharedPtr<Reference> _parseQualifier(Char const *qualifier, Int &pos,
                                                       ReferenceUsageCriteria criteria);

  /// Parses a single segment of the qualifier string.
  public: Reference const& parseQualifierSegment(Char const *&qualifier);

  private: void setQualifierSegmentResultValidator(Char const *&qualifier, Reference *seg);

  /**
     * @brief Validate a qualifier string.
     * @param absolute Specifies whether to validate an absolute qualifier or
     *                 not. An absolute qualifier is one that doesn't use a
     *                 subqualifier, i.e. doesn't have any brackets.
     */
  public: static Bool validateQualifier(Char const *qualifier, Bool absolute)
  {
    return ReferenceParser::_validateQualifier(qualifier, absolute, 0);
  }

  /// Internal recursive function that does the actual validation.
  private: static Bool _validateQualifier(Char const *qualifier, Bool absolute, Char const **end);

  /**
     * @brief Find the beginning of the last segment within a qualifier string.
     * The last segument could be a subqualifier (a bracketed full qualifier) in
     * which case the return will be the opening bracket. If the last segment is
     * not a subqualifier the return will be the beginning of the segment, not
     * the dot preceding it.
     */
  public: static Char const* findLastQualifierSegment(Char const *qualifier)
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
  private: static Char const* _findLastQualifierSegment(Char const *qualifier);

  private: static Int parseInt(Char const *qualifier, Int &result);

  private: static Int parseStrAttributeValidator(Char const *qualifier, StrAttributeValidator *targetValidator);

  private: static Int skipSpaces(Char const *qualifier)
  {
    Int i = 0;
    while (qualifier[i] == CHR(' ') || qualifier[i] == CHR('\t')) ++i;
    return i;
  }

}; // class

} } // namespace

#endif
