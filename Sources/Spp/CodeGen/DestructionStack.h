/**
 * @file Spp/CodeGen/DestructionStack.h
 * Contains the header of class Spp::CodeGen::DestructionStack.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_DESTRUCTIONSTACK_h
#define SPP_CODEGEN_DESTRUCTIONSTACK_h

namespace Spp::CodeGen
{

class DestructionStack : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(DestructionStack, TiObject, "Spp.CodeGen", "Spp", "alusus.org");


  //============================================================================
  // Member Variables

  private: PlainList<Core::Data::Node> items;
  private: std::vector<Int> scopeIndexes;


  //============================================================================
  // Constructor & Destructor

  public: DestructionStack()
  {
  }


  //============================================================================
  // Member Functions

  public: void pushScope()
  {
    this->scopeIndexes.push_back(this->items.getCount());
  }

  public: void popScope();

  public: Int getScopeStartIndex(Int scope) const;

  public: void pushItem(Core::Data::Node *item)
  {
    this->items.add(item);
  }

  public: Core::Data::Node* getItem(Int index) const
  {
    return this->items.get(index);
  }

  public: Word getItemCount() const
  {
    return this->items.getCount();
  }

}; // class

} // namespace

#endif
