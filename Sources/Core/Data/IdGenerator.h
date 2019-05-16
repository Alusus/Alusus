/**
 * @file Core/Data/IdGenerator.h
 * Contains the header of class Core::Data::IdGenerator.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_IDGENERATOR_H
#define CORE_DATA_IDGENERATOR_H

namespace Core { namespace Data
{

// TODO: DOC

// TODO: How to release ids that are no longer used?

/**
 * @brief A generator of unique IDs.
 * @ingroup core_data
 *
 * This singleton class generates unique Word ids for any use. Using this class
 * to generate the required ids guarantees that no two objects will have the
 * same id.<br>
 * This is used by grammar definitions.
 */
class IdGenerator
{
  //============================================================================
  // Data Types

  private: struct Desc
  {
    Str str;
  };

  private: typedef DirectSortedIndex<Desc, Str, &Desc::str> Index;


  //============================================================================
  // Member Variables

  private: std::vector<Desc> ids;

  private: Index index;


  //============================================================================
  // Constructor

  /// Prevent the singleton class from being inistantiated.
  private: IdGenerator() : index(&ids)
  {
    this->getId(S("UNKNOWN"));
  }


  //============================================================================
  // Member Functions

  public: Bool isDefined(Word id) const
  {
    return id < this->ids.size();
  }

  public: Word getId(Char const *desc);

  public: Str const& getDesc(Word id) const;

  /// Get the singleton object.
  public: static IdGenerator* getSingleton();

}; // class

} } // namespace

/**
 * @brief A shortcut to access the ID generator singleton.
 * @ingroup core_data
 */
#define ID_GENERATOR Core::Data::IdGenerator::getSingleton()

#endif
