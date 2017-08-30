/**
 * @file Tests/CoreTests/helpers.h
 * Contains the declarations of different helper functions and classes that are
 * used by unit tests.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

namespace Tests { namespace CoreTests
{

using namespace std;
using namespace Core;

/// Search the given tree recursively for the data of the given production id.
void findProdData(Word prodId, SharedPtr<TiObject> ptr,
                  vector<SharedPtr<TiObject> > &result, Int index = -1);

/// Search the given tree recursively for a token.
void findToken(Word tokenId, Char const *text, SharedPtr<TiObject> ptr,
               vector<SharedPtr<TiObject> > &result, Int index = -1);

/// Check whether the provided pointer has empty data.
Bool isEmpty(SharedPtr<TiObject> ptr);

/**
 * @brief A class used to validate the tokens generated from the lexer.
 *
 * It works by adding a list of tokens to it, then start the lexer. The class
 * will then make sure it receives the same sequence of tokens.
 */
class LexerTester : public TiObject
{
  TYPE_INFO(LexerTester, TiObject, "Core.Test", "Core", "alusus.net");

  private: vector<SharedPtr<Data::Token> > tokens;
  private: Int currentIndex;
  private: Str errorMsg;
  private: Processing::Lexer *lexer;

  private: Slot<void, Data::Token const*> handleNewTokenSlot = {this, &LexerTester::handleNewToken};

  public: LexerTester(Processing::Lexer *l) : currentIndex(0), lexer(l)
  {
    this->lexer->tokenGenerated.connect(this->handleNewTokenSlot);
  }

  public: void handleNewToken(Data::Token const *token)
  {
    if (this->errorMsg.size() != 0) return;
    if (this->currentIndex >= static_cast<Int>(this->tokens.size())) {
      this->errorMsg = STR("Extra token received: id=");
      this->errorMsg += Data::IdGenerator::getSingleton()->getDesc(token->getId());
      this->errorMsg += STR(", text=");
      this->errorMsg += token->getText();
      return;
    }
    SharedPtr<Data::Token> expected = this->tokens[this->currentIndex];
    if ((expected->getId() != UNKNOWN_ID && expected->getId() != token->getId()) ||
        expected->getText().compare(token->getText()) != 0) {
      this->errorMsg = STR("Unexpected token received at ");
      stringstream ss; ss << this->currentIndex;
      this->errorMsg += ss.str();
      this->errorMsg += STR(": id=");
      this->errorMsg += Data::IdGenerator::getSingleton()->getDesc(token->getId());
      this->errorMsg += STR(", text=");
      this->errorMsg += token->getText();
      return;
    }
    this->currentIndex++;
  }

  public: void reset()
  {
    this->tokens.clear();
    this->currentIndex = 0;
    this->errorMsg.clear();
  }

  public: void addToken(Word id, Char const *text)
  {
    SharedPtr<Data::Token> token(new Data::Token);
    token->setId(id);
    token->setText(text);
    this->tokens.push_back(token);
  }

  public: void test(Char const *text)
  {
    Data::SourceLocation sl(std::make_shared<Str>(STR("unittest")), 1, 1);
    this->lexer->handleNewString(text, sl);
    this->lexer->handleNewChar(FILE_TERMINATOR, sl);
    if (this->errorMsg.size() == 0) {
      if (this->currentIndex == static_cast<Int>(this->tokens.size())) {
        REQUIRE(true);
      } else {
        stringstream ss; ss << this->currentIndex;
        this->errorMsg = STR("Didn't receive all the expected tokens. Current index=");
        this->errorMsg += ss.str();
        FAIL(this->errorMsg.c_str());
      }
    } else {
      FAIL(this->errorMsg.c_str());
    }
  }
};


/// A class that receives and stores build messages.
class NoticeReceiver : public TiObject
{
  TYPE_INFO(NoticeReceiver, TiObject, "Core.Test", "Core", "alusus.net");

  private: vector<SharedPtr<Data::Notice> > notices;
  private: Slot<void, SharedPtr<Data::Notice> const&> noticeSlot = {this, &NoticeReceiver::receiveNotice};

  public: NoticeReceiver(Processing::Engine *engine)
  {
    engine->noticeSignal.connect(this->noticeSlot);
  }

  public: void receiveNotice(SharedPtr<Data::Notice> const &msg)
  {
    this->notices.push_back(msg);
  }

  public: Int getNoticeCount()
  {
    return this->notices.size();
  }

  public: const SharedPtr<Data::Notice>& getMsg(Int i)
  {
    return this->notices[i];
  }

  public: void clear()
  {
    this->notices.clear();
  }
};

} } // namespace

#endif
