/**
 * @file Core/Data/OperationHandler.h
 * Contains the header of class Data::OperationHandler.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_OPERATION_HANDLER_H
#define DATA_OPERATION_HANDLER_H

namespace Core { namespace Data
{

// TODO: DOC

class OperationHandler : public IdentifiableObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(OperationHandler, IdentifiableObject, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Protected Constructor

  protected: OperationHandler()
  {
  }

}; // class

} } // namespace

#endif
