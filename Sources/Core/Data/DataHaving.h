/**
 * @file Core/Data/DataHaving.h
 * Contains the header of interface Data::DataHaving.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_DATAHAVING_H
#define CORE_DATA_DATAHAVING_H

namespace Core::Data
{

// TODO: DOC

/**
 * @brief An interface to provide tree management functions.
 * @ingroup core_data
 *
 * Classes that owns children need to implement this class to provide those
 * functionalitites.
 */
class DataHaving : public TiInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(DataHaving, TiInterface, "Core.Data", "Core", "alusus.org");


  //============================================================================
  // Abstract Functions

  public: virtual void unsetIndexes(Int from, Int to) = 0;

}; // class

} // namespace

#endif
