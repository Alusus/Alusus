/**
 * @file Core/Basic/signals.cpp
 * Contains the implementation for signals classes.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Basic
{

//==============================================================================
// Member Functions

/**
 * Loops through all connected objects and removes 'this' from their targets.
 */
SignalBase::~SignalBase()
{
  for (Int i = 0; i < (Int)this->connections.size(); ++i) {
    TiObject *obj = this->connections[i].obj;
    if (obj == 0) continue;
    SignalReverseConnector *connector = obj->getInterface<SignalReverseConnector>();
    if (connector != 0) connector->onSignalUnlinked(this);
  }
}


/**
 * Add the given object and function pointer to this object's connection list,
 * and add 'this' to the given object's connection list.
 *
 * @note This function should not be called directly, instead the user should
 *       call the connect function defined in EVENT, REQUEST_ANY, and
 *       REQUEST_ALL.
 *
 * @return true if the connection was successful, false if already connected.
 */
Bool SignalBase::_connect(TiObject *obj, void (TiObject::*mfunc)())
{
  // First, make sure the given object is not already connected.
  for (Int i = 0; i < (Int)this->connections.size(); ++i) {
    if (this->connections[i].obj == obj && this->connections[i].mfunc == mfunc) return false;
  }
  // Now add the new object.
  this->connections.push_back(Connection(obj, mfunc));
  // Add 'this' to obj's connecitons.
  SignalReverseConnector *connector = obj->getInterface<SignalReverseConnector>();
  if (connector != 0) connector->onSignalLinked(this);
  return true;
}


/**
 * Add the given function pointer to this object's connection list.
 *
 * @note This function should not be called directly, instead the user should
 *       call the connect function defined in EVENT, REQUEST_ANY, and
 *       REQUEST_ALL.
 *
 * @return true if the connection was successful, false if already connected.
 */
Bool SignalBase::_connect(void (*func)())
{
  // First, make sure the given object is not already connected.
  for (Int i = 0; i < (Int)this->connections.size(); ++i) {
    if (this->connections[i].obj == 0 && this->connections[i].func == func) return false;
  }
  // Now add the new object.
  this->connections.push_back(Connection(func));
  return true;
}


/**
 * Remove all connections from 'this' to the given object, and remove all
 * references to 'this' from the given object's list.
 *
 * @note This function should not be called directly, instead the user should
 *       call the connect function defined in EVENT, REQUEST_ANY, and
 *       REQUEST_ALL.
 *
 * @return true if the operation was successful, false if no connection was
 *         found.
 */
Bool SignalBase::_unconnect(TiObject *obj, void (TiObject::*mfunc)())
{
  // Look for the connection in this list and remove it.
  for (Int i = 0; i < (Int)this->connections.size(); ++i) {
    if (this->connections[i].obj == obj && this->connections[i].mfunc == mfunc) {
      this->connections.erase(this->connections.begin() + i);
      // If we no longer have connections to the same object, we can inform it of that.
      Int j;
      for (j = i+1; j < (Int)this->connections.size(); ++j) {
        if (this->connections[j].obj == obj) return true;
      }
      SignalReverseConnector *connector = obj->getInterface<SignalReverseConnector>();
      if (connector != 0) connector->onSignalUnlinked(this);
      return true;
    }
  }
  return false;
}


/**
 * Remove the connection from 'this' to the given function.
 *
 * @note This function should not be called directly, instead the user should
 *       call the connect function defined in EVENT, REQUEST_ANY, and
 *       REQUEST_ALL.
 *
 * @return true if the operation was successful, false if no connection was
 *         found.
 */
Bool SignalBase::_unconnect(void (*func)())
{
  // Look for the connection in this list and remove it.
  for (Int i = 0; i < (Int)this->connections.size(); ++i) {
    if (this->connections[i].obj == 0 && this->connections[i].func == func) {
      this->connections.erase(this->connections.begin() + i);
      return true;
    }
  }
  return false;
}


void SignalBase::onReceiverDestroyed(TiObject *obj)
{
  // Look for the connection in this list and remove it.
  for (Int i = 0; i < (Int)this->connections.size(); ++i) {
    if (this->connections[i].obj == obj) {
      this->connections.erase(this->connections.begin() + i);
      --i;
    }
  }
}

} } // namespace
