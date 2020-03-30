/**
 * @file Spp/RootManagerExtension.cpp
 * Contains the implementation of class Spp::RootManagerExtension.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp
{

//==============================================================================
// Initialization Functions

RootManagerExtension::Overrides* RootManagerExtension::extend(
  Core::Main::RootManager *rootManager,
  SharedPtr<Ast::Helper> const &astHelper, SharedPtr<CodeGen::AstProcessor> const &astProcessor,
  SharedPtr<CodeGen::Generator> const &generator, SharedPtr<LlvmCodeGen::TargetGenerator> const &targetGenerator,
  SharedPtr<BuildManager> const &buildManager
) {
  auto extension = std::make_shared<RootManagerExtension>(rootManager);
  rootManager->addDynamicInterface(extension);

  auto overrides = new Overrides();
  extension->astHelper = astHelper;
  extension->astProcessor = astProcessor;
  extension->generator = generator;
  extension->targetGenerator = targetGenerator;
  extension->buildManager = buildManager;

  overrides->dumpLlvmIrForElementRef = extension->dumpLlvmIrForElement.set(
    &RootManagerExtension::_dumpLlvmIrForElement
  ).get();
  overrides->buildObjectFileForElementRef = extension->buildObjectFileForElement.set(
    &RootManagerExtension::_buildObjectFileForElement
  ).get();
  overrides->importFileRef = extension->importFile.set(&RootManagerExtension::_importFile).get();
  overrides->getModifierStringsRef = extension->getModifierStrings.set(&RootManagerExtension::_getModifierStrings).get();

  return overrides;
}


void RootManagerExtension::unextend(Core::Main::RootManager *rootManager, Overrides *overrides)
{
  auto extension = ti_cast<RootManagerExtension>(rootManager);
  extension->dumpLlvmIrForElement.reset(overrides->dumpLlvmIrForElementRef);
  extension->buildObjectFileForElement.reset(overrides->buildObjectFileForElementRef);
  extension->importFile.reset(overrides->importFileRef);
  extension->getModifierStrings.reset(overrides->getModifierStringsRef);
  extension->astHelper.remove();
  extension->astProcessor.remove();
  extension->generator.remove();
  extension->targetGenerator.remove();
  extension->buildManager.remove();
  rootManager->removeDynamicInterface<RootManagerExtension>();
  delete overrides;
}


//==============================================================================
// Main Functions

void RootManagerExtension::_dumpLlvmIrForElement(
  TiObject *self, TiObject *element, Core::Notices::Store *noticeStore, Core::Processing::Parser *parser
) {
  PREPARE_SELF(rootManagerExt, RootManagerExtension);
  rootManagerExt->buildManager->dumpLlvmIrForElement(element, noticeStore, parser);
}


Bool RootManagerExtension::_buildObjectFileForElement(
  TiObject *self, TiObject *element, Char const *objectFilename, Core::Notices::Store *noticeStore,
  Core::Processing::Parser *parser
) {
  PREPARE_SELF(rootManagerExt, RootManagerExtension);
  return rootManagerExt->buildManager->buildObjectFileForElement(element, objectFilename, noticeStore, parser);
}


void RootManagerExtension::_importFile(TiObject *self, Char const *filename)
{
  PREPARE_SELF(rootManager, Core::Main::RootManager);
  Str error;
  if (!rootManager->tryImportFile(filename, error)) {
    throw EXCEPTION(FileException, filename, C('r'), error.c_str());
  }
}


Bool RootManagerExtension::_getModifierStrings(
  TiObject *self, TiObject *element, Char const *modifierKwd, Char const **resultStrs[], Word *resultCount,
  Core::Notices::Store *noticeStore, Core::Processing::Parser *parser
) {
  auto node = ti_cast<Core::Data::Node>(element);
  auto def = Core::Data::findOwner<Core::Data::Ast::Definition>(node);
  if (def == 0 || def->getModifiers() == 0 || def->getModifiers()->getCount() == 0) {
    *resultCount = 0;
    *resultStrs = 0;
    return true;
  }
  // Look for the modifier.
  for (Int i = 0; i < def->getModifiers()->getElementCount(); ++i) {
    auto modifier = ti_cast<Core::Data::Ast::ParamPass>(def->getModifiers()->getElement(i));
    if (modifier == 0) continue;
    auto identifier = modifier->getOperand().ti_cast_get<Core::Data::Ast::Identifier>();
    if (identifier != 0 && identifier->getValue() == modifierKwd && modifier->getParam() != 0) {
      // Found the requested modifier.
      Core::Basic::PlainList<TiObject> strList;
      auto strs = modifier->getParam().ti_cast_get<Core::Basic::Containing<TiObject>>();
      if (strs == 0) {
        strList.add(modifier->getParam().get());
        strs = &strList;
      }
      *resultCount = strs->getElementCount();
      *resultStrs = reinterpret_cast<Char const**>(malloc(sizeof(Char*) * (*resultCount)));
      for (Int j = 0; j < *resultCount; ++j) {
        auto str = ti_cast<Core::Data::Ast::StringLiteral>(strs->getElement(j));
        if (str == 0) {
          noticeStore->add(std::make_shared<Spp::Notices::InvalidModifierDataNotice>(
            Core::Data::Ast::findSourceLocation(strs->getElement(j))
          ));
          parser->flushApprovedNotices();
          free(*resultStrs);
          *resultStrs = 0;
          *resultCount = 0;
          return false;
        }
        (*resultStrs)[j] = str->getValue().get();
      }
      return true;
    }
  }
  *resultCount = 0;
  *resultStrs = 0;
  return true;
}

} // namespace
