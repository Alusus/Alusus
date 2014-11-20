/**
 * @file Scg/CodeGeneration/LowerLinkExpression.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __LowerLinkExpression_h__
#define __LowerLinkExpression_h__

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

  class LowerLinkExpression
  {
  private:
    Core::Basic::SharedPtr<Core::Basic::IdentifiableObject> lhs;
    Core::Basic::SharedPtr<Core::Basic::IdentifiableObject> rhs;
    std::string separator;

  public:
    LowerLinkExpression(CodeGenerator *gen,
        const Core::Basic::SharedPtr<Core::Basic::IdentifiableObject> &item);

    /**
     * Retrieves the parsed item to the left of the token.
     *
     * @return The parsed item.
     */
    Core::Basic::SharedPtr<Core::Basic::IdentifiableObject> GetLHS()
    {
      return lhs;
    }

    /**
     * Retrieves the parsed item to the right of the token.
     *
     * @return The parsed item.
     */
    Core::Basic::SharedPtr<Core::Basic::IdentifiableObject> GetRHS()
    {
      return rhs;
    }

    /**
     * Retrieves the separator of the expression.
     *
     * @return A string containing the separator.
     */
    std::string GetSeparator()
    {
      return separator;
    }
  };
}

#endif // __LowerLinkExpression_h__
