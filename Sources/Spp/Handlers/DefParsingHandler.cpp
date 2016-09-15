/**
 * @file Spp/Handlers/DefParsingHandler.cpp
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <spp.h>

namespace Spp { namespace Handlers
{

using namespace Core;
using namespace Core::Data;


//==============================================================================
// Overloaded Abstract Functions

void DefParsingHandler::onProdEnd(Processing::Parser *parser, Processing::ParserState *state)
{
  auto expr = state->getData().ti_cast_get<Core::Data::Ast::List>();
  ASSERT(expr != 0);
  auto exprMetadata = ti_cast<Core::Data::Ast::MetadataHolder>(expr);
  ASSERT(exprMetadata != 0);

  Core::Data::Ast::LinkOperator *linkOp = 0;
  if (expr->getCount() == 2) {
    linkOp = ti_cast<Core::Data::Ast::LinkOperator>(expr->get(1));
  }
  if (linkOp == 0) {
    state->addBuildMsg(std::make_shared<MissingDefLink>(exprMetadata->getSourceLocation()));
    state->setData(SharedPtr<TiObject>(0));
    return;
  }

  // Get the name of the definition.
  auto nameToken = linkOp->getFirst().ti_cast_get<Core::Data::Ast::Identifier>();
  if (nameToken == 0) {
    state->addBuildMsg(std::make_shared<MissingDefName>(exprMetadata->getSourceLocation()));
    state->setData(SharedPtr<TiObject>(0));
    return;
  }
  auto name = nameToken->getValue();

  // Get the definee (after the colon).
  auto def = linkOp->getSecond().get();
  if (def == 0) {
    // TODO: We need to choose terms for the parts of a define command, e.g.
    // definition name, definition, etc.
    state->addBuildMsg(std::make_shared<InvalidDefCommand>(exprMetadata->getSourceLocation()));
    state->setData(SharedPtr<TiObject>(0));
    return;
  }

  // If the definee is a function we'll set it's name.
  if (def->isDerivedFrom<Spp::Ast::Function>()) {
    static_cast<Spp::Ast::Function*>(def)->setName(name.c_str());
  }

  // Store the definition in the definitions manager.
  this->mergeDefinition(name.c_str(), def, state);

  // Reset parsed data because we are done with the command.
  state->setData(SharedPtr<TiObject>(0));
}


void DefParsingHandler::mergeDefinition(Char const *qualifier, TiObject *obj, Core::Processing::ParserState *state)
{
  auto repository = state->getDataStack();

  // Find a scope.
  Int i = repository->getLevelCount() - 1;
  Core::Data::Ast::Scope *scope = 0;
  while (scope == 0 && i > 0) {
    scope = repository->getLevelData(i).ti_cast_get<Core::Data::Ast::Scope>();
    --i;
  }
  if (scope == 0) {
    throw EXCEPTION(Core::Basic::GenericException, STR("No scope was found in the stack."));
  }

  QualifierSeeker seeker;
  TiObject *dest = seeker.tryGet(qualifier, scope);
  if (dest == 0) {
    seeker.set(qualifier, scope, obj);
    Data::setTreeIds(obj);
  } else {
    if (!Core::Standard::mergeContainers(dest, obj, state)) {
      // Generate a build message.
      Core::Data::Ast::MetadataHolder *itemMeta = ti_cast<Core::Data::Ast::MetadataHolder>(obj);
      Data::SourceLocation sl;
      if (itemMeta != 0) sl = itemMeta->getSourceLocation();
      state->addBuildMsg(std::make_shared<Core::Standard::RedefinitionMsg>(qualifier, sl));
      // Overwrite old data.
      repository->set(qualifier, obj);
    }
  }
}

} } // namespace
