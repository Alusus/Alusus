/**
 * @file Scg/CodeGeneration/typedefs.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __CodeGeneration_typedefs_h__
#define __CodeGeneration_typedefs_h__

#include <memory>

namespace Core
{
namespace Basic
{
class IdentifiableObject;
}
}

namespace Scg
{

typedef std::vector < Core::Basic::SharedPtr<Core::Basic::IdentifiableObject> >
ParsedItemArray;

/**
 * Defines a pair of a name and a parsed item.
 */
class NameParsedItemPair : public std::pair < std::string, std::shared_ptr<Core::Basic::IdentifiableObject> >
{
public:
  NameParsedItemPair()
    : std::pair<std::string, std::shared_ptr<Core::Basic::IdentifiableObject>>() {}

  NameParsedItemPair(const std::string &name, const std::shared_ptr<Core::Basic::IdentifiableObject> &item)
    : std::pair<std::string, std::shared_ptr<Core::Basic::IdentifiableObject>>(name, item) {}

  /**
   * Retrieves the name of the parsed item.
   * @return A string containing the name.
   */
  const std::string &getName() const
  {
    return first;
  }

  /**
   * Retrieves a pointer to the parsed item.
   * @return A pointer to the parsed item.
   */
  const std::shared_ptr<Core::Basic::IdentifiableObject> getParsedItem() const
  {
    return second;
  }

  /**
   * Retrieves a pointer to the parsed item.
   * @return A pointer to the parsed item.
   */
  std::shared_ptr<Core::Basic::IdentifiableObject> getParsedItem()
  {
    return second;
  }
};

} // namespace

#endif // __CodeGeneration_typedefs_h__
