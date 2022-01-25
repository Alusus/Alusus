/**
 * @file Core/Data/Grammar/factory_types.h
 * Types needed by the grammar factory.
 *
 * @copyright Copyright (C) 2022 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_DATA_GRAMMAR_FACTORYTYPES_H
#define CORE_DATA_GRAMMAR_FACTORYTYPES_H

namespace Core::Data::Grammar
{

/// @ingroup core_data_grammar
struct CommandArg
{
  public: SharedPtr<Reference> prod;
  public: SharedPtr<TiInt> min;
  public: SharedPtr<TiInt> max;
  public: SharedPtr<TiInt> flags;
  public: CommandArg() {}
  public: CommandArg(
    SharedPtr<Reference> prod, SharedPtr<TiInt> min, SharedPtr<TiInt> max, SharedPtr<TiInt> flags
  ) : prod(prod), min(min), max(max), flags(flags)
  {
  }
};

/// @ingroup core_data_grammar
struct CommandSection
{
  public: SharedPtr<Map> keywords;
  public: std::vector<CommandArg> args;
  public: SharedPtr<TiInt> min;
  public: SharedPtr<TiInt> max;
  public: SharedPtr<TiInt> flags;
  public: CommandSection() {}
  public: CommandSection(
    SharedPtr<Map> keywords, std::vector<CommandArg> args
  ) : keywords(keywords), args(args)
  {
  }
  public: CommandSection(
    SharedPtr<Map> keywords, std::vector<CommandArg> args,
    SharedPtr<TiInt> min, SharedPtr<TiInt> max, SharedPtr<TiInt> flags
  ) : keywords(keywords), args(args), min(min), max(max), flags(flags)
  {
  }
};

/// @ingroup core_data_grammar
struct StatementSegment
{
  public: SharedPtr<Reference> prod;
  public: SharedPtr<TiInt> min;
  public: SharedPtr<TiInt> max;
  public: StatementSegment(
    SharedPtr<Reference> prod, SharedPtr<TiInt> min, SharedPtr<TiInt> max
  ) : prod(prod), min(min), max(max)
  {
  }
};

} // namespace

#endif
