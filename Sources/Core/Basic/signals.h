/**
 * @file Core/Basic/signals.h
 * Contains definitions for signals macros and classes.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef BASIC_SIGNALS_H
#define BASIC_SIGNALS_H

namespace Core { namespace Basic
{

// TODO: DOC

/**
 * @brief The base for classes that can emit or receive signals.
 * @ingroup basic_utils
 *
 * Classes that can emit or receive signals must inherit from this class. This
 * class keeps a list of all connections allowing it to emit signals as well as
 * to disconnect all connections on destruction.<br>
 * The user shouldn't use this class directly, instead, he should either use the
 * EVENT macro, the ENQUIRY macro, or the Signal_Handler definition.
 *
 * @sa EVENT
 * @sa ENQUIRY
 * @sa Signal_Handler
 */
class SignalBase
{
  //============================================================================
  // Data Types

  /// Contains info of a single connection.
  protected: struct Connection
  {
    Connection(IdentifiableObject *o, void (IdentifiableObject::*f)()) : obj(o), mfunc(f)
    {
    }

    Connection(void (*f)()) : obj(0), func(f)
    {
    }

    /// A pointer to the target object.
    IdentifiableObject *obj;

    union
    {
      /**
             * @brief A pointer to the target object's member function.
             *
             * If this connection refers to a source (obj is the source of the
             * signal) this value will be 0.
             * This should only be used if obj is not 0.
             */
      void (IdentifiableObject::*mfunc)();

      /**
             * @brief A pointer to a non member function.
             *
             * This is used to connect the signal to global functions instead of
             * member functions. This should only be used if obj is 0.
             */
      void (*func)();
    };
  };


  //============================================================================
  // Member Variables

  /// The list of connections made to or from this object.
  protected: std::vector<Connection> connections;


  //============================================================================
  // Constructor & Destructor

  protected: SignalBase()
  {
  }

  /// Remove all connections.
  protected: ~SignalBase();


  //============================================================================
  // Member Functions

  /// Connect a member function target to this source.
  protected: Bool _connect(IdentifiableObject *obj, void (IdentifiableObject::*mfunc)());

  /// Connect a non member function target to this source.
  protected: Bool _connect(void (*func)());

  /// Disconnect a member function target from this source.
  protected: Bool _unconnect(IdentifiableObject *obj, void (IdentifiableObject::*mfunc)());

  /// Disconnect a non member function target from this source.
  protected: Bool _unconnect(void (*func)());

  /// Disconnect all connections to a specific object.
  public: void onReceiverDestroyed(IdentifiableObject* obj);
};


/**
 * @brief A definition macro for signals.
 * @ingroup basic_utils
 *
 * This macro is used to define signals. The macro defines a class that inherits
 * from SignalBase and implements a 'emit' function and () operator with the
 * specified argument definitions. The defined functions have a return value of
 * void. The macro gives an internal name for the class and instantiates it with
 * the name provided to the macro.<br>
 * The getTargetCount member function returns the number of all targets
 * attached to this signal.
 *
 * Following is an exmaple of how this can be used. The Obj::Func function
 * which is the function that will recieve the event has two arguments: an int
 * and a float.<br>
 * SIGNAL(my_event, (int i, float f), (i, f));<br>
 * my_event.connect(&obj, &Obj::Func);<br>
 * my_event(5, 3.2f);
 *
 * @param name The name of the event. This will be the name of the instance of
 *             the defined class.
 * @param argsDef The definition of the arguments of the function exactly as it
 *                 appears in the function definition (the part after the
 *                 function name including the brackets). This will also be the
 *                 args definition of the emit function and the () operator.
 * @param argsCall The list of arguments when placed in the call to the defined
 *                  function. This is typically a comma separated list of all
 *                  arguments in argsDef included in brackets.
 */
#define SIGNAL(name, argsDef, argsCall) \
  class _##name : public Core::Basic::SignalBase \
  { \
    public: \
    _##name() {} \
    template<class T> Core::Basic::Bool connect(T *obj, void (T::*mfunc)argsDef) \
    { \
      return this->_connect(obj, reinterpret_cast<void (Core::Basic::IdentifiableObject::*)()>(mfunc)); \
    } \
    Core::Basic::Bool connect(void (*func)argsDef) \
    { \
      return this->_connect(reinterpret_cast<void (*)()>(func)); \
    } \
    template<class T> Core::Basic::Bool unconnect(T *obj, void (T::*mfunc)argsDef) \
    { \
      return this->_unconnect(obj, reinterpret_cast<void (Core::Basic::IdentifiableObject::*)()>(mfunc)); \
    } \
    Core::Basic::Bool unconnect(void (*func)argsDef) \
    { \
      return this->_unconnect(reinterpret_cast<void (*)()>(func)); \
    } \
    void operator() argsDef \
    { \
      this->emit argsCall; \
    } \
    void emit argsDef \
    { \
      for (Core::Basic::Int _i = 0; _i < (Core::Basic::Int)this->connections.size(); ++_i) { \
        if (this->connections[_i].func == 0) continue; \
        if (this->connections[_i].obj == 0) { \
          reinterpret_cast<void (*)argsDef>(this->connections[_i].func)argsCall; \
        } else { \
          ((this->connections[_i].obj)->* \
            (reinterpret_cast<void (Core::Basic::IdentifiableObject::*)argsDef> \
                             (this->connections[_i].mfunc)))argsCall; \
        } \
      } \
    } \
    Core::Basic::Int getTargetCount() \
    { \
      Core::Basic::Int count=0; \
      for (Core::Basic::Int _i = 0; _i < (Core::Basic::Int)this->connections.size(); ++_i) { \
        if (this->connections[_i].func != 0) ++count; \
      } \
      return count; \
    } \
  } name


/**
 * @brief A definition macro for relayed signals.
 * @ingroup basic_utils
 *
 * This macro calls SIGNAL to define the signal and then creates a function that
 * can be used to as a relay to connect the signal to another signal.
 *
 * @sa SIGNAL
 */
#define RESIGNAL(name, argsDef, argsCall) \
  SIGNAL(name, argsDef, argsCall); \
  void name##Relay argsDef { name.emit argsCall; }


/**
 * @brief A definition macro for signals with acknowledgements.
 * @ingroup basic_utils
 *
 * This is equivalent to SIGNAL, except that the defined function has a return
 * value of Bool. This doesn't define the () operator, instead it defines two
 * functions: emitAny and emitAll.<br>
 * emitAny calls the targets one after the other until it reaches one that
 * returns true, in which case it quits and returns true, otherwise it returns
 * false.<br>
 * emitAll calls all targets, regardless of their return value, and returns the
 * number of targets that returned false. In other words, it emits the signal
 * and tells you how many couldn't handle the signal.<br>
 * The getTargetCount member function returns the number of all targets
 * attached to this signal.
 *
 * @sa SIGNAL
 */
#define SIGNAL_WITH_ACK(name, argsDef, argsCall) \
  class _##name : public Core::Basic::SignalBase \
  { \
    public: \
    _##name() {} \
    template<class T> Core::Basic::Bool connect(T *obj, Core::Basic::Bool (T::*mfunc)argsDef) \
    { \
      return this->_connect(obj, reinterpret_cast<void (Core::Basic::IdentifiableObject::*)()>(mfunc)); \
    } \
    Core::Basic::Bool connect(Core::Basic::Bool (*func)argsDef) \
    { \
      return this->_connect(reinterpret_cast<void (*)()>(func)); \
    } \
    template<class T> Core::Basic::Bool unconnect(T *obj, Core::Basic::Bool (T::*mfunc)argsDef) \
    { \
      return this->_unconnect(obj, reinterpret_cast<void (Core::Basic::IdentifiableObject::*)()>(mfunc)); \
    } \
    Core::Basic::Bool unconnect(Core::Basic::Bool (*func)argsDef) \
    { \
      return this->_unconnect(reinterpret_cast<void (*)()>(func)); \
    } \
    Core::Basic::Bool emitAny argsDef \
    { \
      for (Core::Basic::Int _i = 0; _i < (Core::Basic::Int)this->connections.size(); ++_i) { \
        if (this->connections[_i].func == 0) continue; \
        if (this->connections[_i].obj == 0) { \
          if (reinterpret_cast<Core::Basic::Bool (*)argsDef>(this->connections[_i].func)argsCall) return true; \
        } else {\
          if (((this->connections[_i].obj)->* \
            (reinterpret_cast<Core::Basic::Bool (Core::Basic::IdentifiableObject::*)argsDef> \
                             (this->connections[_i].mfunc)))argsCall) return true; \
        } \
      } \
      return false; \
    } \
    Core::Basic::Int emitAll argsDef \
    { \
      Core::Basic::Int count = 0; \
      for (Core::Basic::Int _i = 0; _i < (Core::Basic::Int)this->connections.size(); ++_i) { \
        if (this->connections[_i].func == 0) continue; \
        if (this->connections[_i].obj == 0) { \
          if (!reinterpret_cast<Core::Basic::Bool (*)argsDef>(this->connections[_i].func)argsCall) ++count; \
        } else { \
          if (!((this->connections[_i].obj)->* \
            (reinterpret_cast<Core::Basic::Bool (Core::Basic::IdentifiableObject::*)argsDef> \
                             (this->connections[_i].mfunc)))argsCall) ++count; \
        } \
      } \
      return count; \
    } \
    Core::Basic::Int getTargetCount() \
    { \
      Core::Basic::Int count=0; \
      for (Core::Basic::Int _i = 0; _i < (Core::Basic::Int)this->connections.size(); ++_i) { \
        if (this->connections[_i].func != 0) ++count; \
      } \
      return count; \
    } \
  } name


/**
 * @brief A definition macro for relayed signals with acknowledgements.
 * @ingroup basic_utils
 *
 * This macro calls SIGNAL_WITH_ACK to define the signal and then creates a
 * function that can be used to as a relay to connect the signal to another
 * signal.
 *
 * @sa SIGNAL_WITH_ACK
 */
#define RESIGNAL_WITH_ACK(name, argsDef, argsCall) \
  SIGNAL_WITH_ACK(name, argsDef, argsCall); \
  Core::Basic::Bool name##_relay_any argsDef { return name.emitAny argsCall; } \
  Core::Basic::Int name##_relay_all argsDef { return name.emitAll argsCall; }


} } // namespace

#endif
