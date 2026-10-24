/**
 * @file Core/Basic/ti_callees.h
 * Contains definitions for TI function classes.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_TICALLEES_H
#define CORE_BASIC_TICALLEES_H

namespace Core::Basic
{

class TiCallee : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(TiCallee, TiObject, "Core.Basic", "Core", "alusus.org");


  //============================================================================
  // Member Functions

  public: virtual TiCallee* getSuper() const
  {
    return 0;
  }

  public: virtual void setSuper(TiCallee *super)
  {
  }

}; // class


template<class FT> class TiFunction : public TiCallee
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(TiFunction, TiCallee, "Core.Basic", "Core", "alusus.org", (FT));


  //============================================================================
  // Data Types

  public: typedef std::function<FT> Fn;


  //============================================================================
  // Member Variables

  public: Fn fn;


  //============================================================================
  // Constructors & Destructor

  public: TiFunction() {}

  public: TiFunction(Fn const &f) : fn(f)
  {
  }

  public: static SharedPtr<TiFunction<FT>> create(Fn const &f)
  {
    return newSrdObj<TiFunction<FT>>(f);
  }

}; // class


template<class FT> class TiFunctionOverride : public TiFunction<FT>
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(TiFunctionOverride, TiFunction<FT>, "Core.Basic", "Core", "alusus.org", (FT));


  //============================================================================
  // Data Types

  public: typedef std::function<FT> Fn;


  //============================================================================
  // Member Variables

  public: SharedPtr<TiCallee> superFn;


  //============================================================================
  // Constructors & Destructor

  public: TiFunctionOverride() {}

  public: TiFunctionOverride(Fn const &f) : TiFunction<FT>(f)
  {
  }

  public: TiFunctionOverride(Fn const &f, SharedPtr<TiCallee> const &super) : TiFunction<FT>(f), superFn(super)
  {
  }

  public: static SharedPtr<TiFunctionOverride<FT>> create(Fn const &f)
  {
    return newSrdObj<TiFunctionOverride<FT>>(f);
  }

  public: static SharedPtr<TiFunctionOverride<FT>> create(Fn const &f, SharedPtr<TiCallee> const &super)
  {
    return newSrdObj<TiFunctionOverride<FT>>(f, super);
  }


  //============================================================================
  // Member Functions

  public: virtual TiCallee* getSuper() const
  {
    return this->superFn.get();
  }

  public: virtual void setSuper(TiCallee *super)
  {
    this->superFn = getSharedPtr(super, true);
  }

}; // class


/**
 * @brief A callee that holds a plain function pointer receiving the self object.
 * Unlike TiFunction, this callee doesn't use std::function, which makes calls
 * cheaper and keeps call stacks clean while debugging. FT is the function's
 * signature excluding the self argument.
 */
template<class FT> class TiMethod;

template<class RT, class ...ARGS> class TiMethod<RT(ARGS...)> : public TiCallee
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(TiMethod, TiCallee, "Core.Basic", "Core", "alusus.org", (RT(ARGS...)));


  //============================================================================
  // Data Types

  public: typedef RT (*Fn)(TiObject *self, ARGS...);


  //============================================================================
  // Member Variables

  public: Fn fn;


  //============================================================================
  // Constructors & Destructor

  public: TiMethod() : fn(0) {}

  public: TiMethod(Fn f) : fn(f)
  {
  }

  public: static SharedPtr<TiMethod<RT(ARGS...)>> create(Fn f)
  {
    return newSrdObj<TiMethod<RT(ARGS...)>>(f);
  }

}; // class


template<class FT> class TiMethodOverride;

template<class RT, class ...ARGS> class TiMethodOverride<RT(ARGS...)> : public TiMethod<RT(ARGS...)>
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(TiMethodOverride, TiMethod<RT(ARGS...)>, "Core.Basic", "Core", "alusus.org", (RT(ARGS...)));


  //============================================================================
  // Data Types

  public: typedef RT (*Fn)(TiObject *self, ARGS...);


  //============================================================================
  // Member Variables

  public: SharedPtr<TiCallee> superFn;


  //============================================================================
  // Constructors & Destructor

  public: TiMethodOverride() {}

  public: TiMethodOverride(Fn f) : TiMethod<RT(ARGS...)>(f)
  {
  }

  public: TiMethodOverride(Fn f, SharedPtr<TiCallee> const &super) : TiMethod<RT(ARGS...)>(f), superFn(super)
  {
  }

  public: static SharedPtr<TiMethodOverride<RT(ARGS...)>> create(Fn f)
  {
    return newSrdObj<TiMethodOverride<RT(ARGS...)>>(f);
  }

  public: static SharedPtr<TiMethodOverride<RT(ARGS...)>> create(Fn f, SharedPtr<TiCallee> const &super)
  {
    return newSrdObj<TiMethodOverride<RT(ARGS...)>>(f, super);
  }


  //============================================================================
  // Member Functions

  public: virtual TiCallee* getSuper() const
  {
    return this->superFn.get();
  }

  public: virtual void setSuper(TiCallee *super)
  {
    this->superFn = getSharedPtr(super, true);
  }

}; // class

} // namespace

#endif
