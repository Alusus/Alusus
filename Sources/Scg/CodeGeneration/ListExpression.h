/**
 * @file Scg/CodeGeneration/LinkExpression.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __ListExpression_h__
#define __ListExpression_h__

// STL header files
#include <vector>

// Alusus Core header files
#include <core.h>

// Alusus SCG header files
#include <CodeGeneration/typedefs.h>
#include <exceptions.h>
#include <typedefs.h>

namespace Scg
{
  class ListExpression
  {
  private:
    ParsedItemArray items;

  public:
    ListExpression(CodeGenerator *gen,
        const Core::Basic::SharedPtr<Core::Basic::IdentifiableObject> &item);

    /**
     * Retrieves the number of items in this list expression.
     * @return The number of items in this list expression.
     */
    ParsedItemArray::size_type GetItemCount() const { return items.size(); }

    /**
     * Retrieves the item at the given index.
     * @param[in] index The index of the item.
     * @return A pointer to the item.
     */
    ParsedItemArray::value_type GetItem(ParsedItemArray::size_type index) const
    { return items[index]; }

    /**
     * If this list is a list of tokens, this function extracts the tokens
     * and return them as a string list.
     * @return
     */
    StringArray ParseTokenList() const;
  };
}

#endif // __ListExpression_h__
