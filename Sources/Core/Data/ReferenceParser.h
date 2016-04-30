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

#ifndef CORE_DATA_REFERENCEPARSER_H
#define CORE_DATA_REFERENCEPARSER_H

namespace Core { namespace Data
{

// TODO: DOC

class ReferenceParser
{
  //============================================================================
  // Constants

  protected: static constexpr Char const *ROOT_KEYWORD = STR("root");
  protected: static constexpr Int ROOT_KEYWORD_LEN = 4;

  protected: static constexpr Char const *SELF_KEYWORD = STR("self");
  protected: static constexpr Int SELF_KEYWORD_LEN = 4;

  protected: static constexpr Char const *FIND_KEYWORD = STR("{find ");
  protected: static constexpr Int FIND_KEYWORD_LEN = 6;

  protected: static constexpr Char const *EVAL_KEYWORD = STR("{eval ");
  protected: static constexpr Int EVAL_KEYWORD_LEN = 6;

  protected: static constexpr Char const *BACKWARDS_KEYWORD = STR("backwards");
  protected: static constexpr Int BACKWARDS_KEYWORD_LEN = 9;

  protected: static constexpr Char const *WHERE_KEYWORD = STR("~where(");
  protected: static constexpr Int WHERE_KEYWORD_LEN = 7;


  //============================================================================
  // Data Types

  private: struct CharGroupStorage
  {
    SharedPtr<CharGroupUnit> letterCharGroup;
    SharedPtr<CharGroupUnit> numberCharGroup;
  };


  //============================================================================
  // Member Variables

  protected: IndexReference tempIndexReference;

  protected: StrKeyReference tempStrKeyReference;

  protected: ScopeReference tempScopeReference;

  protected: SearchReference tempSearchReference;

  protected: SelfReference tempSelfReference;

  protected: SharedPtr<StrAttributeValidator> tempSearchValidator;

  protected: SharedPtr<StrAttributeValidator> tempResultValidator;

  protected: ReferenceUsageCriteria usageCriteria;


  //============================================================================
  // Construction & Destruction

  public: ReferenceParser()
  {
  }

  public: virtual ~ReferenceParser()
  {
  }

  /**
   * @brief Get a shared instance that can be used to parser non temp refs.
   * This is just to avoid reinstantiating the object multiple times. It's not
   * a singleton because this class is not designed to work as a singleton. This
   * method is only needed for performance purposes since the same instance can
   * be reused to parse non-temp references. Those who are parsing temp segments
   * should instantiate their own instances.
   */
  public: static ReferenceParser* getSharedInstance()
  {
    static ReferenceParser parser;
    return &parser;
  }

  private: static CharGroupStorage* getSharedCharGroupStorage();


  //============================================================================
  // Member Functions

  /// @name Char Group Functions
  /// @{

  public: static void setSharedLetterCharGroup(SharedPtr<CharGroupUnit> const group);

  public: static void setSharedNumberCharGroup(SharedPtr<CharGroupUnit> const group);

  protected: virtual CharGroupUnit* getLetterCharGroup() const;

  protected: virtual CharGroupUnit* getNumberCharGroup() const;

  /// @}

  /// @name Qualifier Building Functions
  /// @{

  /// Build the string representation of this reference.
  public: virtual void buildQualifier(Reference const *ref, StrStream &qualifier);

  /// Build the string representation of this reference and returns it.
  public: const Str getQualifier(Reference const *ref)
  {
    StrStream stream;
    this->buildQualifier(ref, stream);
    return stream.str();
  }

  /// @}

  /// @name Qualifier Parsing Functions
  /// @{

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
   * @param criteria The usage criteria that will be assigned to each segment in
   *                 the generated reference chain.
   */
  public: SharedPtr<Reference> parseQualifier(Char const *qualifier,
    ReferenceUsageCriteria criteria = ReferenceUsageCriteria::SINGLE_DATA_SINGLE_MATCH)
  {
    Int pos = 0;
    this->usageCriteria = criteria;
    return this->_parseQualifier(qualifier, pos);
  }

  /// Internal recursive function that does the actual parsing of qualifiers.
  protected: virtual SharedPtr<Reference> _parseQualifier(Char const *qualifier, Int &pos);

  protected: virtual SharedPtr<Reference> _parseQualifierSegment(Char const *qualifier, Int &pos);

  /// Parses a single segment of the qualifier string.
  public: virtual Reference const& parseQualifierSegmentToTemp(Char const *&qualifier);

  protected: virtual void setTempQualifierSegmentResultValidator(Char const *&qualifier, Reference *seg);

  /// @}

  /// @name Qualifier Validation and Segment Seeking Functions
  /// @{

  /**
   * @brief Validate a qualifier string.
   * @param absolute Specifies whether to validate an absolute qualifier or
   *                 not. An absolute qualifier is one that doesn't use a
   *                 subqualifier, i.e. doesn't have any brackets.
   */
  public: Bool validateQualifier(Char const *qualifier, Bool absolute=false)
  {
    return this->_validateQualifier(qualifier, absolute);
  }

  /// Internal recursive function that does the actual validation.
  protected: virtual Bool _validateQualifier(Char const *&qualifier, Bool absolute, Bool subQualifier=false);

  protected: virtual Bool _validateQualifierSegment(Char const *&qualifier, Bool absolute);

  protected: virtual Bool _validateResultValidator(Char const *&qualifier);

  /**
   * @brief Find the beginning of the last segment within a qualifier string.
   * The last segument could be a subqualifier (a bracketed full qualifier) in
   * which case the return will be the opening bracket. If the last segment is
   * not a subqualifier the return will be the beginning of the segment, not
   * the dot preceding it.
   */
  public: Char const* findLastQualifierSegment(Char const *qualifier);

  /// @}

  /// @name Reference Generation Functions
  /// @{

  protected: virtual SharedPtr<Reference> createIndexReference(Int index)
  {
    return std::make_shared<IndexReference>(index);
  }

  protected: virtual SharedPtr<Reference> createStrKeyReference(Char const *key, Word length)
  {
    return std::make_shared<StrKeyReference>(key, length);
  }

  protected: virtual SharedPtr<Reference> createScopeReference(Char const *scope, Word length);

  protected: virtual SharedPtr<Reference> createSelfReference()
  {
    return std::make_shared<SelfReference>();
  }

  protected: virtual SharedPtr<Reference> createSearchReference(Char const *&qualifier);

  protected: virtual SharedPtr<Reference> createIndirectReference(Char const *&qualifier);

  protected: virtual SharedPtr<Validator> createResultValidator(Char const *&qualifier);

  protected: virtual Reference const& prepareTempIndexReference(Int index)
  {
    this->tempIndexReference.setIndex(index);
    return this->tempIndexReference;
  }

  protected: virtual Reference const& prepareTempStrKeyReference(Char const *key, Word length)
  {
    this->tempStrKeyReference.setKey(key, length);
    return this->tempStrKeyReference;
  }

  protected: virtual Reference const& prepareTempScopeReference(Char const *scope, Word length);

  protected: virtual Reference const& prepareTempSelfReference()
  {
    return this->tempSelfReference;
  }

  protected: virtual Reference const& prepareTempSearchReference(Char const *&qualifier);

  /// @}

  /// @name Helper Functions
  /// @{

  protected: virtual Int parseInt(Char const *qualifier, Int &result);

  protected: virtual Int parseStrAttributeValidator(Char const *qualifier, StrAttributeValidator *targetValidator);

  protected: static Int skipSpaces(Char const *qualifier)
  {
    Int i = 0;
    while (qualifier[i] == CHR(' ') || qualifier[i] == CHR('\t')) ++i;
    return i;
  }

  /// @}

}; // class

} } // namespace

/**
 * @brief A shortcut to access the reference parser singleton.
 * @ingroup data
 */
#define REF_PARSER Core::Data::ReferenceParser::getSharedInstance()

#endif
