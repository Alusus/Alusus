/**
 * @file Core/Basic/SignalReceiver.h
 * Contains the header of class Core::Basic::SignalReceiver.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef BASIC_SIGNAL_RECIEVER_H
#define BASIC_SIGNAL_RECEIVER_H

namespace Core { namespace Basic
{

// TODO: DOC

class SignalBase;

/**
 * @brief The base for classes that can receive signals.
 * @ingroup basic_utils
 *
 * This class implements the SignalReverseConnector interface to keep a
 * record of all connections in order to automatically disconnect them on
 * object's destruction.
 */
class SignalReceiver : public IdentifiableObject, public SignalReverseConnector
{
  //============================================================================
  // Type Info

  TYPE_INFO(SignalReceiver, IdentifiableObject, "Core.Basic", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_1(IdentifiableObject, SignalReverseConnector);


  //============================================================================
  // Member Variables

  private: std::vector<SignalBase*> signalConnections;


  //============================================================================
  // Destructor

  public: virtual ~SignalReceiver();


  //============================================================================
  // SignalReverseConnector Functions

  public: virtual void onSignalLinked(SignalBase *signal);

  public: virtual void onSignalUnlinked(SignalBase *signal);

}; // class

} } // namespace

#endif
