/**
 * @file Core/Processing/IdentifierTokenizingHandler.h
 * Contains the header of class Core::Processing::IdentifierTokenizingHandler.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_HANDLERS_IDENTIFIERTOKENIZINGHANDLER_H
#define CORE_PROCESSING_HANDLERS_IDENTIFIERTOKENIZINGHANDLER_H

namespace Core::Processing::Handlers
{

// TODO: DOC

/**
 * @brief Used to override the ID of const tokens.
 * @ingroup core_processing_handlers
 *
 * This tokenizing handler is used to override the id of automatically generated
 * constant tokens to a specific one. This is useful if we need all const tokens
 * to have the same id so it can be easily matched in token terms.
 */
class IdentifierTokenizingHandler : public TokenizingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(IdentifierTokenizingHandler, TokenizingHandler, "Core.Processing.Handlers", "Core", "alusus.org");


  //============================================================================
  // Types

  public: typedef std::unordered_map<Str, Word, std::hash<std::string>> Keywords;


  //============================================================================
  // Member Variables

  private: Keywords keywords;


  //============================================================================
  // Constructor

  public: IdentifierTokenizingHandler()
  {
  }


  //============================================================================
  // Member Functions

  public: void addKeyword(Char const *keyword)
  {
    if (this->keywords.count(keyword) == 0) this->keywords[keyword] = 1;
    else ++this->keywords[keyword];
  }

  public: void addKeywords(const std::initializer_list<Char const*> &keywords)
  {
    for (auto keyword : keywords) this->addKeyword(keyword);
  }

  public: void removeKeyword(Char const *keyword)
  {
    if (this->keywords.count(keyword) == 0) return;
    --this->keywords[keyword];
    if (this->keywords[keyword] == 0) this->keywords.erase(keyword);
  }

  public: void removeKeywords(const std::initializer_list<Char const*> &keywords)
  {
    for (auto keyword : keywords) this->removeKeyword(keyword);
  }

  public: virtual void prepareToken(
    Data::Token *token, Word id, WChar const *tokenText, Word tokenTextLength,
    Data::SourceLocationRecord const &sourceLocation
  );

}; // class

} // namespace

#endif
