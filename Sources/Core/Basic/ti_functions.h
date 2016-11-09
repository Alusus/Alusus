/**
 * @file Core/Basic/TiFunction.h
 * Contains the header of class Core::Basic::TiFunction.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_TIFUNCTION_H
#define CORE_BASIC_TIFUNCTION_H

namespace Core { namespace Basic
{

class TiFunctionBase : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(TiFunctionBase, TiObject, "Core.Basic", "Core", "alusus.net");

}; // class


template<class FT> class TiFunction : public TiFunctionBase
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(TiFunction, TiFunctionBase, "Core.Basic", "Core", "alusus.net", FT);


  //============================================================================
  // Data Types

  public: typedef std::function<FT> Fn;


  //============================================================================
  // Member Variables

  public: Fn const fn;


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

  TEMPLATE_TYPE_INFO(TiFunctionOverride, TiFunction<FT>, "Core.Basic", "Core", "alusus.net", FT);


  //============================================================================
  // Data Types

  public: typedef std::function<FT> Fn;


  //============================================================================
  // Member Variables

  public: SharedPtr<TiFunctionBase> superFn;


  //============================================================================
  // Constructors & Destructor

  public: TiFunctionOverride() {}

  public: TiFunctionOverride(Fn const &f, SharedPtr<TiFunctionBase> const &super) : TiFunction<FT>(f), superFn(super)
  {
  }

  public: static SharedPtr<TiFunctionOverride<FT>> create(Fn const &f, SharedPtr<TiFunctionBase> const &super)
  {
    return std::make_shared<TiFunctionOverride<FT>>(f, super);
  }

}; // class

} } // namespace

#endif
