/**
 * @file Spp/CodeGen/ExtraDataAccessor.h
 * Contains the header of class Spp::CodeGen::ExtraDataAccessor.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_EXTRADATAACCESSOR_H
#define SPP_CODEGEN_EXTRADATAACCESSOR_H

#define DEFINE_EXTRA_ACCESSORS(name) \
  public: template <class DT, class OT> inline DT* tryGet##name(OT *object) { \
    return tryGetExtra<DT>(object, this->id##name); \
  } \
  public: template <class DT, class OT> inline DT* get##name(OT *object) { \
    return getExtra<DT>(object, this->id##name); \
  } \
  public: template <class DT, class OT> inline void set##name(OT *object, SharedPtr<DT> const &data) { \
    object->setExtra(this->id##name, data); \
  } \
  public: template <class OT> inline void remove##name(OT *object) { \
    object->removeExtra(this->id##name); \
  }

namespace Spp::CodeGen
{

class ExtraDataAccessor : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(ExtraDataAccessor, TiObject, "Spp.CodeGen", "Spp", "alusus.org");


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

  public: ExtraDataAccessor(Char const *prefix = "", Char const *sharedPrefix = 0)
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

  DEFINE_EXTRA_ACCESSORS(CodeGenData);
  DEFINE_EXTRA_ACCESSORS(AutoCtor);
  DEFINE_EXTRA_ACCESSORS(AutoCtorType);
  DEFINE_EXTRA_ACCESSORS(AutoDtor);
  DEFINE_EXTRA_ACCESSORS(AutoDtorType);
  DEFINE_EXTRA_ACCESSORS(BuildId);
  DEFINE_EXTRA_ACCESSORS(GlobalVarState);

  // didCodeGenFail

  public:
  template <class OT>
  inline Bool didCodeGenFail(OT *object)
  {
    auto f = object->getExtra(this->idCodeGenFailed).template ti_cast_get<TiBool>();
    return f && f->get();
  }

  // setCodeGenFailed

  public:
  template <class OT>
  inline void setCodeGenFailed(OT *object, Bool f)
  {
    object->setExtra(this->idCodeGenFailed, TiBool::create(f));
  }

  // resetCodeGenFailed

  public:
  template <class OT>
  inline void resetCodeGenFailed(OT *object)
  {
    object->removeExtra(this->idCodeGenFailed);
  }

  // getInitStatementsGenIndex

  public:
  template <class OT>
  inline Int getInitStatementsGenIndex(OT *object)
  {
    auto i = object->getExtra(this->idInitStatementGenIndex).template ti_cast_get<TiInt>();
    return i == 0 ? 0 : i->get();
  }

  // setInitStatementsGenIndex

  template <class OT>
  inline void setInitStatementsGenIndex(OT *object, Int i)
  {
    auto index = object->getExtra(this->idInitStatementGenIndex).template ti_cast_get<TiInt>();
    if (index == 0) {
      object->setExtra(this->idInitStatementGenIndex, TiInt::create(i));
    } else {
      index->set(i);
    }
  }

  // resetInitStatementsGenIndex

  template <class OT>
  inline void resetInitStatementsGenIndex(OT *object)
  {
    object->removeExtra(this->idInitStatementGenIndex);
  }

}; // class

} // namespace

#endif
