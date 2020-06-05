/**
 * @file Core/Basic/ti_functions.h
 * Contains definitions for TI function classes.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_TIFUNCTIONS_H
#define CORE_BASIC_TIFUNCTIONS_H

namespace Core { namespace Basic
{

class TiFunctionBase : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(TiFunctionBase, TiObject, "Core.Basic", "Core", "alusus.org");


  //============================================================================
  // Member Functions

  public: virtual TiFunctionBase* getSuper() const
  {
    return 0;
  }

  public: virtual void setSuper(TiFunctionBase *super)
  {
  }

}; // class


template<class FT> class TiFunction : public TiFunctionBase
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(TiFunction, TiFunctionBase, "Core.Basic", "Core", "alusus.org", (FT));


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
    return std::make_shared<TiFunction<FT>>(f);
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

  public: SharedPtr<TiFunctionBase> superFn;


  //============================================================================
  // Constructors & Destructor

  public: TiFunctionOverride() {}

  public: TiFunctionOverride(Fn const &f) : TiFunction<FT>(f)
  {
  }

  public: TiFunctionOverride(Fn const &f, SharedPtr<TiFunctionBase> const &super) : TiFunction<FT>(f), superFn(super)
  {
  }

  public: static SharedPtr<TiFunctionOverride<FT>> create(Fn const &f)
  {
    return std::make_shared<TiFunctionOverride<FT>>(f);
  }

  public: static SharedPtr<TiFunctionOverride<FT>> create(Fn const &f, SharedPtr<TiFunctionBase> const &super)
  {
    return std::make_shared<TiFunctionOverride<FT>>(f, super);
  }


  //============================================================================
  // Member Functions

  public: virtual TiFunctionBase* getSuper() const
  {
    return this->superFn.get();
  }

  public: virtual void setSuper(TiFunctionBase *super)
  {
    this->superFn = getSharedPtr(super, true);
  }

}; // class

} } // namespace

#endif
