/**
 * @file Core/Basic/ti_declarers.h
 * Contains declaration macros for TI classes and interfaces.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_TIDECLARERS_H
#define CORE_BASIC_TIDECLARERS_H

//==============================================================================
// Interface Implementation Macros

#define INTERFACES(parent, interfaceChecks) \
  protected: \
    virtual TiInterface* _getInterface(TypeInfo const *info) { \
      TiInterface *interfacePtr; \
      SPACE_EXPAND_ARGS interfaceChecks \
      return parent::_getInterface(info); \
    } \
  public: \
    virtual TiObject* getTiObject() { return this; } \
    virtual TiObject const* getTiObject() const { return this; }

#define _INHERITANCE_INTERFACES_CONDITION(interface) \
  interfacePtr = static_cast<interface*>(this)->getInterfacePtr(info); \
  if (interfacePtr != 0) return interfacePtr;
#define _INHERITANCE_INTERFACES_CONDITIONS1(interface1) \
  _INHERITANCE_INTERFACES_CONDITION(interface1)
#define _INHERITANCE_INTERFACES_CONDITIONS2(interface1, interface2) \
  _INHERITANCE_INTERFACES_CONDITION(interface1) \
  _INHERITANCE_INTERFACES_CONDITION(interface2)
#define _INHERITANCE_INTERFACES_CONDITIONS3(interface1, interface2, interface3) \
  _INHERITANCE_INTERFACES_CONDITIONS2(interface1, interface2) \
  _INHERITANCE_INTERFACES_CONDITION(interface3)
#define _INHERITANCE_INTERFACES_CONDITIONS4(interface1, interface2, interface3, interface4) \
  _INHERITANCE_INTERFACES_CONDITIONS3(interface1, interface2, interface3) \
  _INHERITANCE_INTERFACES_CONDITION(interface4)
#define _INHERITANCE_INTERFACES_CONDITIONS5(interface1, interface2, interface3, interface4, interface5) \
  _INHERITANCE_INTERFACES_CONDITIONS4(interface1, interface2, interface3, interface4) \
  _INHERITANCE_INTERFACES_CONDITION(interface5)
#define _INHERITANCE_INTERFACES_CONDITIONS6(interface1, interface2, interface3, interface4, interface5, interface6) \
  _INHERITANCE_INTERFACES_CONDITIONS5(interface1, interface2, interface3, interface4, interface5) \
  _INHERITANCE_INTERFACES_CONDITION(interface6)
#define INHERITANCE_INTERFACES(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, \
               _INHERITANCE_INTERFACES_CONDITIONS6, \
               _INHERITANCE_INTERFACES_CONDITIONS5, \
               _INHERITANCE_INTERFACES_CONDITIONS4, \
               _INHERITANCE_INTERFACES_CONDITIONS3, \
               _INHERITANCE_INTERFACES_CONDITIONS2, \
               _INHERITANCE_INTERFACES_CONDITIONS1)(__VA_ARGS__)

#define _OBJECT_INTERFACES_CONDITION(interface) \
  if (this->interface->isInterfaceDerivedFrom(info)) return this->interface;
#define _OBJECT_INTERFACES_CONDITIONS1(interface1) \
  _OBJECT_INTERFACES_CONDITION(interface1)
#define _OBJECT_INTERFACES_CONDITIONS2(interface1, interface2) \
  _OBJECT_INTERFACES_CONDITION(interface1) \
  _OBJECT_INTERFACES_CONDITION(interface2)
#define _OBJECT_INTERFACES_CONDITIONS3(interface1, interface2, interface3) \
  _OBJECT_INTERFACES_CONDITIONS2(interface1, interface2) \
  _OBJECT_INTERFACES_CONDITION(interface3)
#define _OBJECT_INTERFACES_CONDITIONS4(interface1, interface2, interface3, interface4) \
  _OBJECT_INTERFACES_CONDITIONS3(interface1, interface2, interface3) \
  _OBJECT_INTERFACES_CONDITION(interface4)
#define _OBJECT_INTERFACES_CONDITIONS5(interface1, interface2, interface3, interface4, interface5) \
  _OBJECT_INTERFACES_CONDITIONS4(interface1, interface2, interface3, interface4) \
  _OBJECT_INTERFACES_CONDITION(interface5)
#define _OBJECT_INTERFACES_CONDITIONS6(interface1, interface2, interface3, interface4, interface5, interface6) \
  _OBJECT_INTERFACES_CONDITIONS5(interface1, interface2, interface3, interface4, interface5) \
  _OBJECT_INTERFACES_CONDITION(interface6)
#define OBJECT_INTERFACES(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, \
               _OBJECT_INTERFACES_CONDITIONS6, \
               _OBJECT_INTERFACES_CONDITIONS5, \
               _OBJECT_INTERFACES_CONDITIONS4, \
               _OBJECT_INTERFACES_CONDITIONS3, \
               _OBJECT_INTERFACES_CONDITIONS2, \
               _OBJECT_INTERFACES_CONDITIONS1)(__VA_ARGS__)

#define OBJECT_INTERFACE_LIST(interfaceList) \
  interfacePtr = this->interfaceList.get(info); \
  if (interfacePtr != 0) return interfacePtr;


//==============================================================================
// Object Type Info Macros

#define _GET_TYPE_INFO_BODY(myType, baseType, typeNamespace, moduleName, url) \
  static Core::Basic::ObjectTypeInfo const *typeInfo = 0; \
  if (typeInfo == 0) { \
    Char const* uniqueName = S(url) S("/") S(moduleName) S("/") S(typeNamespace) S(".") S(#myType); \
    typeInfo = reinterpret_cast<Core::Basic::ObjectTypeInfo const*>(GLOBAL_STORAGE->getObject(uniqueName)); \
    if (typeInfo == 0) { \
      typeInfo = new Core::Basic::ObjectTypeInfo(S(#myType), S(typeNamespace), S(moduleName), S(url), \
      baseType::getTypeInfo(), myType::_getFactory(0)); \
      GLOBAL_STORAGE->setObject(uniqueName, const_cast<Core::Basic::ObjectTypeInfo*>(typeInfo)); \
    } \
  } \
  return typeInfo;

#define _TYPE_INFO(myType, baseType, typeNamespace, moduleName, url) \
  private: \
    static TiObjectFactory* _getFactory(Float dummy) { return 0; } \
  public: \
    virtual Core::Basic::ObjectTypeInfo const* getMyTypeInfo() const { return myType::getTypeInfo(); } \
    static Core::Basic::ObjectTypeInfo const* getTypeInfo() { \
      _GET_TYPE_INFO_BODY(myType, baseType, typeNamespace, moduleName, url) \
    }
#define _TYPE_INFO_WITH_INTERFACES(myType, baseType, typeNamespace, moduleName, url, interfaces) \
  private: \
    static TiObjectFactory* _getFactory(Float dummy) { return 0; } \
  public: \
    virtual Core::Basic::ObjectTypeInfo const* getMyTypeInfo() const { return myType::getTypeInfo(); } \
    static Core::Basic::ObjectTypeInfo const* getTypeInfo() { \
      _GET_TYPE_INFO_BODY(myType, baseType, typeNamespace, moduleName, url) \
    } \
    INTERFACES(baseType, interfaces)
#define TYPE_INFO(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _TYPE_INFO_WITH_INTERFACES, _TYPE_INFO, _, _, _, _)(__VA_ARGS__)

#define _CUSTOM_NAME_TYPE_INFO(myType, myTypeName, baseType, typeNamespace, moduleName, url) \
  private: \
    static TiObjectFactory* _getFactory(Float dummy) { return 0; } \
  public: \
    virtual Core::Basic::ObjectTypeInfo const* getMyTypeInfo() const { return myType::getTypeInfo(); } \
    static Core::Basic::ObjectTypeInfo const* getTypeInfo() { \
      _GET_TYPE_INFO_BODY(myTypeName, baseType, typeNamespace, moduleName, url) \
    }
#define _CUSTOM_NAME_TYPE_INFO_WITH_INTERFACES(myType, myTypeName, baseType, typeNamespace, moduleName, url, interfaces) \
  private: \
    static TiObjectFactory* _getFactory(Float dummy) { return 0; } \
  public: \
    virtual Core::Basic::ObjectTypeInfo const* getMyTypeInfo() const { return myType::getTypeInfo(); } \
    static Core::Basic::ObjectTypeInfo const* getTypeInfo() { \
      _GET_TYPE_INFO_BODY(myTypeName, baseType, typeNamespace, moduleName, url) \
    } \
    INTERFACES(baseType, interfaces)
#define CUSTOM_NAME_TYPE_INFO(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, \
               _CUSTOM_NAME_TYPE_INFO_WITH_INTERFACES, \
               _CUSTOM_NAME_TYPE_INFO, _, _, _, _, _)(__VA_ARGS__)

#define OBJECT_FACTORY(myType) \
  private: static TiObjectFactory* _getFactory(Int dummy) { return getTiObjectFactory<myType>(); }


//==============================================================================
// Template Type Info Macros

#define _GET_TYPE_INFO_TEMPLATE_BODY(myType, baseType, typeNamespace, moduleName, url, templateArgs) \
  static Core::Basic::ObjectTypeInfo const *typeInfo = 0; \
  if (typeInfo == 0) { \
    Str typeName = Str(S(#myType)) + S("<") + TypeName<COMMA_EXPAND_ARGS templateArgs>::get() + S(">"); \
    Str uniqueName = Str(S(url) S("/") S(moduleName) S("/") S(typeNamespace) S(".")) + typeName; \
    typeInfo = reinterpret_cast<Core::Basic::ObjectTypeInfo const*>(GLOBAL_STORAGE->getObject(uniqueName)); \
    if (typeInfo == 0) { \
      typeInfo = new Core::Basic::ObjectTypeInfo(typeName, S(typeNamespace), S(moduleName), S(url), \
                                                 baseType::getTypeInfo(), myType::_getFactory(0)); \
      GLOBAL_STORAGE->setObject(uniqueName, const_cast<Core::Basic::ObjectTypeInfo*>(typeInfo)); \
    } \
  } \
  return typeInfo;

#define _TEMPLATE_TYPE_INFO(myType, baseType, typeNamespace, moduleName, url, templateArgs) \
  private: \
    static TiObjectFactory* _getFactory(Float dummy) { return 0; } \
  public: \
    virtual Core::Basic::ObjectTypeInfo const* getMyTypeInfo() const { \
      return myType<COMMA_EXPAND_ARGS templateArgs>::getTypeInfo(); \
    } \
    static Core::Basic::ObjectTypeInfo const* getTypeInfo() { \
      _GET_TYPE_INFO_TEMPLATE_BODY(myType, baseType, typeNamespace, moduleName, url, templateArgs) \
    }
#define _TEMPLATE_TYPE_INFO_WITH_INTERFACES(myType, baseType, typeNamespace, moduleName, url, templateArgs, interfaces) \
  private: \
    static TiObjectFactory* _getFactory(Float dummy) { return 0; } \
  public: \
    virtual Core::Basic::ObjectTypeInfo const* getMyTypeInfo() const { \
      return myType<COMMA_EXPAND_ARGS templateArgs>::getTypeInfo(); \
    } \
    static Core::Basic::ObjectTypeInfo const* getTypeInfo() { \
      _GET_TYPE_INFO_TEMPLATE_BODY(myType, baseType, typeNamespace, moduleName, url, templateArgs) \
    } \
    INTERFACES(baseType, interfaces)
#define TEMPLATE_TYPE_INFO(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, \
               _TEMPLATE_TYPE_INFO_WITH_INTERFACES, \
               _TEMPLATE_TYPE_INFO, _, _, _, _, _)(__VA_ARGS__)


//==============================================================================
// Interface Type Info Macros

#define INTERFACE_INFO(myType, baseType, typeNamespace, moduleName, url) \
  public: \
    virtual Core::Basic::InterfaceTypeInfo const* getMyInterfaceInfo() const { return myType::getTypeInfo(); } \
    static Core::Basic::InterfaceTypeInfo const* getTypeInfo() {   \
      static Core::Basic::InterfaceTypeInfo const *typeInfo = 0; \
      if (typeInfo == 0) { \
        Char const* uniqueName = S(url) S("/") S(moduleName) S("/") S(typeNamespace) S(".") S(#myType); \
        typeInfo = reinterpret_cast<Core::Basic::InterfaceTypeInfo const*>(GLOBAL_STORAGE->getObject(uniqueName)); \
        if (typeInfo == 0) { \
          typeInfo = new Core::Basic::InterfaceTypeInfo(S(#myType), S(typeNamespace), S(moduleName), S(url), \
            baseType::getTypeInfo()); \
          GLOBAL_STORAGE->setObject(uniqueName, const_cast<Core::Basic::InterfaceTypeInfo*>(typeInfo)); \
        } \
      } \
      return typeInfo; \
    } \
    TiInterface* getInterfacePtr(Core::Basic::TypeInfo const *info) {\
      if (info == myType::getTypeInfo()) \
        return this; \
      else \
        return baseType::getInterfacePtr(info); \
    }


//==============================================================================
// Template Interface Type Info Macros

#define TEMPLATE_INTERFACE_INFO(myType, baseType, typeNamespace, moduleName, url, templateArgs) \
  public: \
    virtual Core::Basic::InterfaceTypeInfo const* getMyInterfaceInfo() const { \
      return myType<COMMA_EXPAND_ARGS templateArgs>::getTypeInfo(); \
    } \
    static Core::Basic::InterfaceTypeInfo const* getTypeInfo() {   \
      static Core::Basic::InterfaceTypeInfo const *typeInfo = 0; \
      if (typeInfo == 0) { \
        Str typeName = Str(S(#myType)) + S("<") + TypeName<COMMA_EXPAND_ARGS templateArgs>::get() + S(">"); \
        Str uniqueName = Str(S(url) S("/") S(moduleName) S("/") S(typeNamespace) S(".")) + typeName; \
        typeInfo = reinterpret_cast<Core::Basic::InterfaceTypeInfo const*>(GLOBAL_STORAGE->getObject(uniqueName)); \
        if (typeInfo == 0) { \
          typeInfo = new Core::Basic::InterfaceTypeInfo(typeName, S(typeNamespace), S(moduleName), S(url), \
            baseType::getTypeInfo()); \
          GLOBAL_STORAGE->setObject(uniqueName, const_cast<Core::Basic::InterfaceTypeInfo*>(typeInfo)); \
        } \
      } \
      return typeInfo; \
    } \
    TiInterface* getInterfacePtr(Core::Basic::TypeInfo const *info) {\
      if (info == myType<COMMA_EXPAND_ARGS templateArgs>::getTypeInfo()) \
        return this; \
      else \
        return baseType::getInterfacePtr(info); \
    }


//==============================================================================
// Object Interface Type Info Macros

#define OBJ_INTERFACE_INFO(myType, baseType, typeNamespace, moduleName, url) \
  private: \
    static ObjTiInterfaceFactory* _getFactory(Float dummy) { return getObjTiInterfaceFactory<myType>(); } \
  public: \
    virtual Core::Basic::InterfaceTypeInfo const* getMyInterfaceInfo() const { return myType::getTypeInfo(); } \
    virtual Core::Basic::ObjInterfaceTypeInfo const* getMyObjInterfaceInfo() const { return myType::getTypeInfo(); } \
    static Core::Basic::ObjInterfaceTypeInfo const* getTypeInfo() {   \
      static Core::Basic::ObjInterfaceTypeInfo const *typeInfo = 0; \
      if (typeInfo == 0) { \
        Char const* uniqueName = S(url) S("/") S(moduleName) S("/") S(typeNamespace) S(".") S(#myType); \
        typeInfo = reinterpret_cast<Core::Basic::ObjInterfaceTypeInfo const*>(GLOBAL_STORAGE->getObject(uniqueName)); \
        if (typeInfo == 0) { \
          typeInfo = new Core::Basic::ObjInterfaceTypeInfo(S(#myType), S(typeNamespace), S(moduleName), S(url), \
            baseType::getTypeInfo(), myType::_getFactory(0)); \
          GLOBAL_STORAGE->setObject(uniqueName, const_cast<Core::Basic::ObjInterfaceTypeInfo*>(typeInfo)); \
        } \
      } \
      return typeInfo; \
    } \
    TiInterface* getInterfacePtr(Core::Basic::TypeInfo const *info) {\
      if (info == myType::getTypeInfo()) \
        return this; \
      else \
        return baseType::getInterfacePtr(info); \
    }


//==============================================================================
// Obsolete Macros

#define IMPLEMENT_INTERFACES(parent, ...) \
  INTERFACES(parent, (INHERITANCE_INTERFACES(__VA_ARGS__)))

#define IMPLEMENT_INTERFACES_1(parent, interface1) \
  IMPLEMENT_INTERFACES(parent, interface1)

#define IMPLEMENT_INTERFACES_2(parent, interface1, interface2) \
  IMPLEMENT_INTERFACES(parent, interface1, interface2)

#define IMPLEMENT_INTERFACES_3(parent, interface1, interface2, interface3) \
  IMPLEMENT_INTERFACES(parent, interface1, interface2, interface3)

#define IMPLEMENT_INTERFACES_4(parent, interface1, interface2, interface3, interface4) \
  IMPLEMENT_INTERFACES(parent, interface1, interface2, interface3, interface4)

#define IMPLEMENT_INTERFACES_5(parent, interface1, interface2, interface3, interface4, interface5) \
  IMPLEMENT_INTERFACES(parent, interface1, interface2, interface3, interface4, interface5)

#endif
