/**
 * @file Core/Basic/ti_object_factories.h
 * Contains definitions for TI object factories.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_TIOBJECTFACTORIES_H
#define CORE_BASIC_TIOBJECTFACTORIES_H

namespace Core { namespace Basic
{

class ObjTiInterface;

//==============================================================================
// Factory Classes

class TiObjectFactory
{
  //============================================================================
  // Types

  public: typedef std::function<TiObject*()> PlainCreateFunc;
  public: typedef std::function<SharedPtr<TiObject>()> SharedCreateFunc;


  //============================================================================
  // Member Variables

  private: PlainCreateFunc plainCreateFunc;
  private: SharedCreateFunc sharedCreateFunc;


  //============================================================================
  // Constructor

  public: TiObjectFactory(PlainCreateFunc plainFunc, SharedCreateFunc sharedFunc)
    : plainCreateFunc(plainFunc), sharedCreateFunc(sharedFunc)
  {
  }


  //============================================================================
  // Member Functions

  public: TiObject* createPlain()
  {
    if (this->plainCreateFunc != 0) return this->plainCreateFunc();
    else return 0;
  }

  public: SharedPtr<TiObject> createShared()
  {
    if (this->sharedCreateFunc != 0) return this->sharedCreateFunc();
    else return SharedPtr<TiObject>::null;
  }

}; // class


class ObjTiInterfaceFactory
{
  //============================================================================
  // Types

  public: typedef std::function<ObjTiInterface*(TiObject*)> PlainCreateFunc;
  public: typedef std::function<SharedPtr<ObjTiInterface>(TiObject*)> SharedCreateFunc;


  //============================================================================
  // Member Variables

  private: PlainCreateFunc plainCreateFunc;
  private: SharedCreateFunc sharedCreateFunc;


  //============================================================================
  // Constructor

  public: ObjTiInterfaceFactory(PlainCreateFunc plainFunc, SharedCreateFunc sharedFunc)
    : plainCreateFunc(plainFunc), sharedCreateFunc(sharedFunc)
  {
  }


  //============================================================================
  // Member Functions

  public: ObjTiInterface* createPlain(TiObject *owner)
  {
    if (this->plainCreateFunc != 0) return this->plainCreateFunc(owner);
    else return 0;
  }

  public: SharedPtr<ObjTiInterface> createShared(TiObject *owner)
  {
    if (this->sharedCreateFunc != 0) return this->sharedCreateFunc(owner);
    else return SharedPtr<ObjTiInterface>::null;
  }

}; // class


//==============================================================================
// Helper Functions

template<class T> TiObjectFactory* getTiObjectFactory()
{
  return new TiObjectFactory(
    []()->TiObject*
    {
      return new T();
    },
    []()->SharedPtr<TiObject>
    {
      return std::make_shared<T>();
    });
}


template<class T> ObjTiInterfaceFactory* getObjTiInterfaceFactory()
{
  return new ObjTiInterfaceFactory(
    [](TiObject *owner)->ObjTiInterface*
    {
      return new T(owner);
    },
    [](TiObject *owner)->SharedPtr<ObjTiInterface>
    {
      return std::make_shared<T>(owner);
    }
  );
}

} } // namespace

#endif
