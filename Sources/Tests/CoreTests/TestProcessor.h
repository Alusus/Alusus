/**
 * @file Tests/CoreTests/TestProcessor.h
 * Contains the header of class CoreTests::TestProcessor.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef TEST_TEST_PROCESSOR_H
#define TEST_TEST_PROCESSOR_H

namespace Tests { namespace CoreTests
{

// TODO: DOC

class TestProcessor : public Main::Processor
{
  //============================================================================
  // Type Info

  TYPE_INFO(TestProcessor, Main::Processor, "Core.Test", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: TestGrammarPlant plant;


  //============================================================================
  // Constructors / Destructor

  public: TestProcessor()
  {
    this->initialize(this->plant.getCharGroupDefinitions(), this->plant.getTokenDefinitions(),
                     this->plant.getManager());
  }

  public: virtual ~TestProcessor()
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
