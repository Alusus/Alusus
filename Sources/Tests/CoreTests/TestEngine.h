/**
 * @file Tests/CoreTests/TestEngine.h
 * Contains the header of class CoreTests::TestEngine.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef TEST_TESTENGINE_H
#define TEST_TESTENGINE_H

namespace Tests { namespace CoreTests
{

// TODO: DOC

class TestEngine : public Processing::Engine
{
  //============================================================================
  // Type Info

  TYPE_INFO(TestEngine, Processing::Engine, "Core.Test", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: TestGrammarPlant plant;


  //============================================================================
  // Constructors / Destructor

  public: TestEngine()
  {
    this->initialize(this->plant.getRepository());
  }

  public: virtual ~TestEngine()
  {
  }


  //============================================================================
  // Member Functions

  public: TestGrammarPlant* getTestGrammarStore()
  {
    return &this->plant;
  }

}; // class

} } // namespace

#endif
