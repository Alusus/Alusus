/**
 * @file Scg/Containers/Module.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_MODULE_H
#define SCG_MODULE_H

// Dependencies
#include <list>
#include <unordered_map>
// Alusus Headers
#include <Functions/FunctionStore.h>
#include <Functions/UserDefinedFunction.h>
#include <llvm_fwd.h>
#include <typedefs.h>

namespace Scg
{

// Scg forward declarations
// TODO: Maybe this should be moved to an fwd file?
class DeclareExtFunction;
class DefineFunction;
class CodeGenUnit;

class Module : public AstNode, public virtual Core::Data::Container
{
  friend class CodeGenUnit;

  //============================================================================
  // Type Info

  TYPE_INFO(Module, AstNode, "Scg", "Scg", "alusus.net");
  IMPLEMENT_INTERFACES_1(AstNode, Core::Data::Container);


  //============================================================================
  // Member Variables

  private: int tempIndex = 0;
  //! A store of all the functions defined or declared in the module.
  private: FunctionStore functionStore;
  //! A map containing the variables defined in this block.
  private: VariableMap variableMap;
  //! The name of the module.
  private: std::string name;
  //! A map of the user defined types.
  private: mutable ValueTypeMap typeMap;
  private: Core::Data::SharedList children;


  //============================================================================
  // Constructors & Destructor

  public: Module(Char const *name = STR("")) : name(name)
  {
  }

  public: virtual ~Module();


  //============================================================================
  // Member Functions

  public: void setName(Char const *n)
  {
    this->name = n;
  }

  /**
   * Retrieves the name of the module.
   * @return The name of the module.
   */
  public: std::string const& getName() const
  {
    return this->name;
  }

  // TODO: Do we need a constant version of this member function?
  /**
   * Retrieves the value type having the given name.
   * @param[in] typeName  The name of the value type to retrieve.
   */
  public: ValueType* getValueTypeByName(const std::string &typeName) const;

  /**
   * Retrieves the value type having the given name.
   * @param[in] typeName  The name of the value type to retrieve.
   */
  public: ValueType* tryGetValueTypeByName(const std::string &typeName) const;

  /**
   * Adds the given function to the function store of the module.
   * @param[in] function  A pointer to the function to be added.
   */
  public: void addFunction(SharedPtr<Function> const &function)
  {
    this->functionStore.add(function);
  }

  /**
   * Determine whether this module has a function with the given signature,
   * whether it is a function defined in the module or just a declaration.
   *
   * @note While this function might return true indicating that there is a
   * DefineFunction or DeclareExtFunction instructions, the function
   * getFunction() might still return @c nullptr until the function has
   * actually been generated during code generation.
   *
   * @param[in] name      The name of the function.
   * @param[in] arguments The types of the arguments of the required function.
   *
   * @return True or false.
   */
  public: bool hasFunction(const std::string &name,
                           const ValueTypeSpecArray &arguments) const;

  /**
   * get the function having the given name and argument types.
   *
   * @note This function might return null even if hasFunction() returns false,
   * as this function only retrieves the UserDefinedFunction object if it has been
   * generated.
   *
   * @param[in] name      The name of the function.
   * @param[in] arguments The types of the arguments of the required function.
   * @return A pointer to the function, or nullptr if there is no matching
   * function.
   */
  public: SharedPtr<Function> const& getFunction(const std::string &name,
                                                 const ValueTypeSpecArray &arguments) const
  {
    return this->functionStore.get(name, arguments);
  }

  public: Function* matchFunction(const std::string &name,
                                                   const ValueTypeSpecArray &arguments) const
  {
    return this->functionStore.match(*this, name, arguments);
  }

  /**
   * Finds the DefineFunction instruction that generates a function with the
   * given name.
   * @param[in] name  The name of the function.
   * @return A pointer to the DefineFunction instruction, or @c null.
   */
  public: DefineFunction* findDefineFunction(const std::string &name,
                                             const ValueTypeSpecArray &arguments);

  /**
   * Finds the DeclareExtFunction instruction that declares a function with the
   * given name.
   * @param[in] name  The name of the function.
   * @return A pointer to the DeclareExtFunction instruction, or @c null.
   */
  public: DeclareExtFunction* findDeclareFunction(const std::string &name,
                                                  const ValueTypeSpecArray &arguments);

  /**
   * Returns a reference to the variable map.
   * @return A reference to the variable map.
   */
  public: VariableMap const& getVariableMap() const
  {
    return this->variableMap;
  }
  public: VariableMap& getVariableMap()
  {
    return this->variableMap;
  }

  /**
   * get a reference to the type map of this module.
   *
   * @return A reference to the type map.
   */
  public: ValueTypeMap const& getTypeMap() const
  {
    return typeMap;
  }
  public: ValueTypeMap& getTypeMap()
  {
    return typeMap;
  }

  public: std::string getTempVarName();

  /**
   * Prepends the given expression at the beginning of the module.
   * @param[in] expr  A pointer to the expression.
   */
  public: void prependNode(SharedPtr<AstNode> const &expr);

  /**
   * Appends the given expression to the end of the module.
   * @param[in] expr  A pointer to the expression.
   */
  public: void appendNode(SharedPtr<AstNode> const &expr);


  //============================================================================
  // Container Implementation

  public: virtual void set(Int index, IdentifiableObject *val)
  {
  }

  public: virtual void remove(Int index)
  {
  }

  public: virtual Word getCount() const
  {
    return this->children.getCount();
  }

  public: virtual IdentifiableObject* get(Int index) const
  {
    return this->children.get(index);
  }

}; // class

} // namespace

#endif
