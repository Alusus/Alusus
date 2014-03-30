/**
 * @file Core/Basic/SignalReverseConnector.h
 * Contains the header of interface Basic::SignalReverseConnector.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef BASIC_SIGNAL_REVERSE_CONNECTOR_H
#define BASIC_SIGNAL_REVERSE_CONNECTOR_H

namespace Core { namespace Basic
{

// TODO: DOC

class SignalBase;

/**
 * @brief An interface for classes that wants reverse connections to signals.
 * @ingroup basic_utils
 *
 * Classes that can connect to signals may implement this interface to keep a
 * list of those connections for proper disconnection when the object is
 * destroyed.
 */
class SignalReverseConnector : public IdentifiableInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(SignalReverseConnector, IdentifiableInterface, "Core.Basic", "Core", "alusus.net");


  //============================================================================
  // Abstract Functions

  public: virtual void onSignalLinked(SignalBase *signal) = 0;

  public: virtual void onSignalUnlinked(SignalBase *signal) = 0;

}; // class

} } // namespace

#endif
