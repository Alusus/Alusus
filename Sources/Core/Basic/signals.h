/**
 * @file Core/Basic/signals.h
 * Contains definitions for signal and slot classes.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
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

  // Slots bound to member functions are stored here rather than in func because the object pointer together with the
  // member function pointer don't fit in std::function's internal buffer, which would cause a heap allocation for
  // every such slot. The member function pointer is stored as a pointer to a member of an incomplete class and is cast
  // back to its original type when called; the standard guarantees that such a round trip gives back the original
  // value.
  // NOTE: This relies on all member function pointers sharing the same representation, which is the case with the
  //       Itanium ABI used by GCC and Clang. MSVC uses different representations depending on the class's inheritance
  //       and may reject the cast (error C2440), in which case the pointer would need to be copied into a byte buffer
  //       instead.
  private: struct _Unknown;
  private: typedef void (_Unknown::*_GenericMemberFunc)();
  private: void *memberSelf = 0;
  private: _GenericMemberFunc memberFunc = 0;
  private: RT (*memberInvoker)(Slot *slot, ARGS... args) = 0;


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
    this->memberInvoker = 0;
  }

  public: void init(std::function<RT(ARGS...)> const &f)
  {
    this->disconnect();
    this->func = f;
    this->memberInvoker = 0;
  }

  public: template<class C> void init(C *self, RT(C::*m)(ARGS...))
  {
    this->disconnect();
    this->func = nullptr;
    this->memberSelf = self;
    this->memberFunc = reinterpret_cast<_GenericMemberFunc>(m);
    this->memberInvoker = [](Slot *slot, ARGS... args)->RT {
      auto m = reinterpret_cast<RT(C::*)(ARGS...)>(slot->memberFunc);
      return (static_cast<C*>(slot->memberSelf)->*m)(args...);
    };
  }

  public: Bool isInitialized() const
  {
    return this->memberInvoker != 0 || (Bool)this->func;
  }

  public: RT call(ARGS... args)
  {
    if (this->memberInvoker != 0) return this->memberInvoker(this, args...);
    return this->func(args...);
  }


  //============================================================================
  // Operators

  public: void operator=(std::function<RT(ARGS...)> const &f)
  {
    this->init(f);
  }

  public: RT operator()(ARGS... args)
  {
    return this->call(args...);
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

  public: virtual ~_Signal()
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
    if (this->slotsEntries.empty()) return;

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
    if (this->slotsEntries.empty()) return 0;

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
    if (this->slotsEntries.empty()) return false;

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

  public: virtual ~SignalRelay()
  {
    while (this->linkSlot.getSignals().size() > 0) {
      this->unrelay(*this->linkSlot.getSignals().back());
    }
    this->disconnectAll();
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
