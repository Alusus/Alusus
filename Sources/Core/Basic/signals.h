/**
 * @file Core/Basic/signals.h
 * Contains definitions for signal and slot classes.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_SIGNALS_H
#define CORE_BASIC_SIGNALS_H

namespace Core { namespace Basic
{

// TODO: Add multi-threading safety.

template<class RT, class ...ARGS> class _Signal;
template<class RT, class ...ARGS> class Signal;
template<class RT, class ...ARGS> class SignalRelay;

//==============================================================================
// Slot

template<class RT, class ...ARGS> class Slot
{
  friend class _Signal<RT, ARGS...>;
  friend class SignalRelay<RT, ARGS...>;

  //============================================================================
  // Member Vars

  private: std::vector<_Signal<RT, ARGS...>*> signals;
  private: std::function<RT(ARGS...)> func;


  //============================================================================
  // Constructors & Destructor

  protected: Slot() {}

  public: Slot(std::function<RT(ARGS...)> const &f) : func(f) {}

  public: template<class C> Slot(C *self, RT(C::*m)(ARGS...))
  {
    this->init(self, m);
  }

  public: ~Slot()
  {
    for (auto signal : this->signals) signal->disconnect(*this);
  }


  //============================================================================
  // Member Functions

  protected: void addSignal(_Signal<RT, ARGS...> *s)
  {
    this->signals.push_back(s);
  }
  protected: void removeSignal(_Signal<RT, ARGS...> *s)
  {
    for (Int i = 0; i < this->signals.size(); ++i) {
      if (this->signals[i] == s) {
        this->signals.erase(this->signals.begin() + i);
        return;
      }
    }
  }
  protected: std::vector<_Signal<RT, ARGS...>*> const& getSignals() const
  {
    return this->signals;
  }

  public: void disconnect()
  {
    for (auto signal : this->signals) signal->disconnect(*this);
    this->signals.clear();
  }

  public: void init(RT(*f)(ARGS...))
  {
    this->disconnect();
    this->func = std::function<RT(ARGS...)>(f);
  }

  public: void init(std::function<RT(ARGS...)> const &f)
  {
    this->disconnect();
    this->func = f;
  }

  public: template<class C> void init(C *self, RT(C::*m)(ARGS...))
  {
    this->disconnect();
    this->func = [=](ARGS... args)->RT {
      return (self->*m)(args...);
    };
  }

  public: Bool isInitialized() const
  {
    return (Bool)this->func;
  }

  public: RT call(ARGS... args)
  {
    return this->func(args...);
  }


  //============================================================================
  // Operators

  public: void operator=(std::function<RT(ARGS...)> const &f)
  {
    this->disconnect();
    this->func = f;
  }

  public: RT operator()(ARGS... args)
  {
    return this->func(args...);
  }

};


//==============================================================================
// Signal

template<class RT, class ...ARGS> class _Signal
{
  //============================================================================
  // Types

  protected: struct SlotEntry
  {
    Slot<RT, ARGS...> *slot;
    Bool enabled;
    SlotEntry() : slot(0), enabled(true) {}
    SlotEntry(Slot<RT, ARGS...> *s) : slot(s), enabled(true) {}
  };


  //============================================================================
  // Member Vars

  protected: std::vector<SlotEntry> slotsEntries;
  protected: std::atomic<Int> firing;
  protected: Bool scheduledDelete;


  //============================================================================
  // Constructor & Destructor

  public: _Signal() : firing(0), scheduledDelete(false) {}

  public: ~_Signal()
  {
    this->disconnectAll();
  }


  //============================================================================
  // Member Functions

  public: virtual void connect(Slot<RT, ARGS...> &slot)
  {
    for (auto s : this->slotsEntries) if (s.slot == &slot) return;
    this->slotsEntries.push_back(&slot);
    slot.addSignal(this);
  }

  public: virtual void disconnect(Slot<RT, ARGS...> &slot)
  {
    for (Int i = 0; i < this->slotsEntries.size(); ++i) {
      if (this->slotsEntries[i].slot == &slot) {
        if (this->firing > 0) {
          this->slotsEntries[i].enabled = false;
          this->scheduledDelete = true;
        } else {
          this->slotsEntries.erase(this->slotsEntries.begin() + i);
          slot.removeSignal(this);
        }
        return;
      }
    }
  }

  public: virtual void disconnectAll()
  {
    while (this->slotsEntries.size() > 0) {
      this->disconnect(*this->slotsEntries[0].slot);
    }
  }

  protected: virtual void disconnectScheduled()
  {
    // TOOD: Apply thread safetly.
    if (!this->scheduledDelete || this->firing > 0) return;
    for (Int i = 0; i < this->slotsEntries.size(); ++i) {
      if (!this->slotsEntries[i].enabled) {
        auto slot = this->slotsEntries[i].slot;
        this->slotsEntries.erase(this->slotsEntries.begin() + i);
        slot->removeSignal(this);
        --i;
      }
    }
    this->scheduledDelete = false;
  }

};

template<class RT, class ...ARGS> class Signal;

template<class ...ARGS> class Signal<void, ARGS...> : public _Signal<void, ARGS...>
{
  public: void emit(ARGS... args)
  {
    finally([=] {
      --this->firing;
      if (this->scheduledDelete) this->disconnectScheduled();
    });

    ++this->firing;
    for (auto slotEntry : this->slotsEntries) {
      if (slotEntry.enabled) {
        if (slotEntry.slot->isInitialized()) slotEntry.slot->call(args...);
      }
    }
  }
};

template<class ...ARGS> class Signal<Bool, ARGS...> : public _Signal<Bool, ARGS...>
{
  public: Word emit(ARGS... args)
  {
    finally([=] {
      --this->firing;
      if (this->scheduledDelete) this->disconnectScheduled();
    });

    ++this->firing;
    Word count = 0;
    for (auto slotEntry : this->slotsEntries) {
      if (slotEntry.enabled) {
        if (slotEntry.slot->isInitialized()) {
          if (slotEntry.slot->call(args...)) ++count;
        }
      }
    }
    return count;
  }

  public: Bool emitAny(ARGS... args)
  {
    finally([=] {
      --this->firing;
      if (this->scheduledDelete) this->disconnectScheduled();
    });

    Bool result = false;
    ++this->firing;
    for (auto slotEntry : this->slotsEntries) {
      if (slotEntry.enabled) {
        if (slotEntry.slot->isInitialized()) {
          if (slotEntry.slot->call(args...)) {
            result = true;
            break;
          }
        }
      }
    }
    return result;
  }
};


//==============================================================================
// SignalRelay

template<class RT, class ...ARGS> class SignalRelay : public _Signal<RT, ARGS...>
{
  //============================================================================
  // Member Variables

  private: Slot<RT, ARGS...> linkSlot;


  //============================================================================
  // Constructors

  public: SignalRelay()
  {
  }

  public: SignalRelay(_Signal<RT, ARGS...> *signal)
  {
    this->relay(signal);
  }

  public: ~SignalRelay()
  {
    while (this->linkSlot.getSignals().size() > 0) {
      this->unrelay(*this->linkSlot.getSignals().back());
    }
  }


  //============================================================================
  // Member Functions

  public: void relay(_Signal<RT, ARGS...> &signal)
  {
    signal.connect(this->linkSlot);
    for (auto slotEntry : this->slotsEntries) {
      signal.connect(*slotEntry.slot);
    }
  }

  public: void unrelay(_Signal<RT, ARGS...> &signal)
  {
    for (auto slotEntry : this->slotsEntries) {
      signal.disconnect(*slotEntry.slot);
    }
    signal.disconnect(this->linkSlot);
  }

  public: virtual void connect(Slot<RT, ARGS...> &slot)
  {
    for (auto signal : this->linkSlot.getSignals()) {
      signal->connect(slot);
    }
    _Signal<RT, ARGS...>::connect(slot);
  }

  public: virtual void disconnect(Slot<RT, ARGS...> &slot)
  {
    for (auto signal : this->linkSlot.getSignals()) {
      signal->disconnect(slot);
    }
    _Signal<RT, ARGS...>::disconnect(slot);
  }

};

} } // namespace

#endif
