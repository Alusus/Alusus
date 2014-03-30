/**
 * @file Scg/CodeGeneration/LowestLinkExpression.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __LowestLinkExpression_h__
#define __LowestLinkExpression_h__

// STL header files
#include <vector>

// Alusus Core header files
#include <core.h>

// Alusus SCG header files
#include <CodeGeneration/typedefs.h>
#include <exceptions.h>

namespace Scg
{
  class CodeGenerator;
  class DeclareExtFunction;

  class LowestLinkExpression
  {
  private:
    Core::Basic::SharedPtr<Core::Standard::ParsedItem> lhs;
    Core::Basic::SharedPtr<Core::Standard::ParsedItem> rhs;
    std::string separator;

  public:
    LowestLinkExpression(CodeGenerator *gen,
        const Core::Basic::SharedPtr<Core::Standard::ParsedItem> &item);

    /**
     * Retrieves the parsed item to the left of the token.
     *
     * @return The parsed item.
     */
    Core::Basic::SharedPtr<Core::Standard::ParsedItem> GetLHS()
    {
      return lhs;
    }

    /**
     * Retrieves the parsed item to the right of the token.
     *
     * @return The parsed item.
     */
    Core::Basic::SharedPtr<Core::Standard::ParsedItem> GetRHS()
    {
      return rhs;
    }

    /**
     * Retrieves the separator of the expression.
     *
     * @return A string containing the separator.
     */
    const std::string &GetSeparator() const
    {
      return separator;
    }
  };
}

#endif // __LowestLinkExpression_h__
