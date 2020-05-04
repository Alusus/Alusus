/**
 * @file Spp/BuildSession.h
 * Contains the header of class Spp::BuildSession.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_BUILDSESSION_H
#define SPP_BUILDSESSION_H

namespace Spp
{

class BuildSession : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(BuildSession, TiObject, "Spp", "Spp", "alusus.org");


  //============================================================================
  // Member Variables

  public: TioSharedPtr globalCtorTgFunc;
  public: TioSharedPtr globalCtorTgContext;
  public: TioSharedPtr globalEntryTgFunc;
  public: TioSharedPtr globalEntryTgContext;
  public: SharedPtr<SharedMap<TiObject>> extraInfo;
  public: Bool offlineExecution;
  public: Str globalCtorName;
  public: Str globalEntryName;

}; // class

} // namespace

#endif
