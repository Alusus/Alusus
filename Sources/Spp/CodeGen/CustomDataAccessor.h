/**
 * @file Spp/CodeGen/CustomDataAccessor.h
 * Contains the header of class Spp::CodeGen::CustomDataAccessor.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_CUSTOMDATAACCESSOR_H
#define SPP_CODEGEN_CUSTOMDATAACCESSOR_H

#define DEFINE_CUSTOM_DATA_ACCESSORS(name) \
  public: template <class DT, class OT> inline DT* tryGet##name(OT *object) { \
    return tryGetCustomData<DT>(object, this->id##name); \
  } \
  public: template <class DT, class OT> inline DT* get##name(OT *object) { \
    return getCustomData<DT>(object, this->id##name); \
  } \
  public: template <class DT, class OT> inline void set##name(OT *object, SharedPtr<DT> const &data) { \
    object->setCustomData(this->id##name, data); \
  } \
  public: template <class OT> inline void remove##name(OT *object) { \
    object->removeCustomData(this->id##name); \
  }

namespace Spp::CodeGen
{

class CustomDataAccessor : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(CustomDataAccessor, TiObject, "Spp.CodeGen", "Spp", "alusus.org");


  //============================================================================
  // Member Variables

  private: Str idCodeGenData;
  private: Str idAutoCtor;
  private: Str idAutoCtorType;
  private: Str idAutoDtor;
  private: Str idAutoDtorType;
  private: Str idCodeGenFailed;
  private: Str idInitStatementGenIndex;
  private: Str idBuildId;
  private: Str idGlobalVarState;


  //============================================================================
  // Constructor & Destructor

  public: CustomDataAccessor(Char const *prefix = "", Char const *sharedPrefix = 0)
  {
    this->setIdPrefix(prefix, sharedPrefix);
  }


  //============================================================================
  // Member Functions

  public: void setIdPrefix(Char const *prefix, Char const *sharedPrefix = 0)
  {
    Str idPrefix = prefix;
    Str sharedIdPrefix = sharedPrefix != 0 ? Str(sharedPrefix) : idPrefix;
    this->idCodeGenData = idPrefix + S("codeGenData");
    this->idAutoCtor = idPrefix + S("autoCtor");
    this->idAutoCtorType = idPrefix + S("autoCtorType");
    this->idAutoDtor = idPrefix + S("autoDtor");
    this->idAutoDtorType = idPrefix + S("autoDtorType");
    this->idCodeGenFailed = idPrefix + S("codeGenFailed");
    this->idInitStatementGenIndex = idPrefix + S("initStatementGenIndex");

    this->idBuildId = sharedIdPrefix + S("buildId");
    this->idGlobalVarState = sharedIdPrefix + S("globalVarState");
  }

  DEFINE_CUSTOM_DATA_ACCESSORS(CodeGenData);
  DEFINE_CUSTOM_DATA_ACCESSORS(AutoCtor);
  DEFINE_CUSTOM_DATA_ACCESSORS(AutoCtorType);
  DEFINE_CUSTOM_DATA_ACCESSORS(AutoDtor);
  DEFINE_CUSTOM_DATA_ACCESSORS(AutoDtorType);
  DEFINE_CUSTOM_DATA_ACCESSORS(BuildId);
  DEFINE_CUSTOM_DATA_ACCESSORS(GlobalVarState);

  // didCodeGenFail

  public:
  template <class OT>
  inline Bool didCodeGenFail(OT *object)
  {
    auto f = object->getCustomData(this->idCodeGenFailed).template ti_cast_get<TiBool>();
    return f && f->get();
  }

  // setCodeGenFailed

  public:
  template <class OT>
  inline void setCodeGenFailed(OT *object, Bool f)
  {
    object->setCustomData(this->idCodeGenFailed, TiBool::create(f));
  }

  // resetCodeGenFailed

  public:
  template <class OT>
  inline void resetCodeGenFailed(OT *object)
  {
    object->removeCustomData(this->idCodeGenFailed);
  }

  // getInitStatementsGenIndex

  public:
  template <class OT>
  inline Int getInitStatementsGenIndex(OT *object)
  {
    auto i = object->getCustomData(this->idInitStatementGenIndex).template ti_cast_get<TiInt>();
    return i == 0 ? 0 : i->get();
  }

  // setInitStatementsGenIndex

  template <class OT>
  inline void setInitStatementsGenIndex(OT *object, Int i)
  {
    auto index = object->getCustomData(this->idInitStatementGenIndex).template ti_cast_get<TiInt>();
    if (index == 0) {
      object->setCustomData(this->idInitStatementGenIndex, TiInt::create(i));
    } else {
      index->set(i);
    }
  }

  // resetInitStatementsGenIndex

  template <class OT>
  inline void resetInitStatementsGenIndex(OT *object)
  {
    object->removeCustomData(this->idInitStatementGenIndex);
  }

}; // class

} // namespace

#endif
