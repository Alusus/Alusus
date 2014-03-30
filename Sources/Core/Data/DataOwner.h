/**
 * @file Core/Data/DataOwner.h
 * Contains the header of interface Data::DataOwner.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_DATA_OWNER_H
#define DATA_DATA_OWNER_H

namespace Core { namespace Data
{

// TODO: DOC

/**
 * @brief An interface to provide tree management functions.
 * @ingroup data
 *
 * Classes that owns children need to implement this class to provide those
 * functionalitites.
 */
class DataOwner : public IdentifiableInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(DataOwner, IdentifiableInterface, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Abstract Functions

  public: virtual void unsetIndexes(Int from, Int to) = 0;

}; // class

} } // namespace

#endif
