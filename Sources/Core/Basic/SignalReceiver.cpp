/**
 * @file Core/Basic/SignalReceiver.cpp
 * Contains the implementation for Base::SignalReceiver.
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
// Destructor

SignalReceiver::~SignalReceiver()
{
  for (Word i = 0; i < this->signalConnections.size(); ++i) {
    this->signalConnections[i]->onReceiverDestroyed(this);
  }
}


//==============================================================================
// SignalReverseConnector Functions

void SignalReceiver::onSignalLinked(SignalBase *signal)
{
  for (Word i = 0; i < this->signalConnections.size(); ++i) {
    if (this->signalConnections[i] == signal) return;
  }
  this->signalConnections.push_back(signal);
}


void SignalReceiver::onSignalUnlinked(SignalBase *signal)
{
  for (Word i = 0; i < this->signalConnections.size(); ++i) {
    if (this->signalConnections[i] == signal) {
      this->signalConnections.erase(this->signalConnections.begin() + i);
      return;
    }
  }
}

} } // namespace
