/**
 * @file Core/Data/Grammar/Map.h
 * Contains the header of class Core::Data::Grammar::Map.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_GRAMMAR_MAP_H
#define CORE_DATA_GRAMMAR_MAP_H

namespace Core::Data::Grammar
{

class Map : public NbMap, public virtual Bindings, public virtual Initializable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Map, NbMap, "Core.Data.Grammar", "Core", "alusus.net", (
    INHERITANCE_INTERFACES(Bindings, Initializable)
  ));


  //============================================================================
  // Member Variables

  private: SharedPtr<Grammar::Reference> baseRef;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDINGS(Bindings,
    (baseRef, Reference, SHARED_REF, setBaseRef(value), baseRef.get())
  );


  //============================================================================
  // Constructor / Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Map);

  IMPLEMENT_ATTR_CONSTRUCTOR(Map);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(Map);

  public: Map(
    Bool useIndex, std::initializer_list<Argument> const &attrs, std::initializer_list<Argument> const &elements
  ) : NbMap(useIndex)
  {
    ATTR_INITIALIZATION_LOOP(attrs)
    MAP_ELEMENT_INITIALIZATION_LOOP(elements)
  }

  public: static SharedPtr<Map> create(
    Bool useIndex, std::initializer_list<Argument> const &attrs, std::initializer_list<Argument> const &elements
  ) {
    return std::make_shared<Map>(useIndex, attrs, elements);
  }


  //============================================================================
  // Member Functions

  /// @name Property Accessors
  /// @{

  public: void setBaseRef(SharedPtr<Reference> const &p)
  {
    this->baseRef = p;
  }
  private: void setBaseRef(Reference *ref)
  {
    this->setBaseRef(getSharedPtr(ref));
  }

  public: SharedPtr<Reference> const& getBaseRef() const
  {
    return this->baseRef;
  }

  /// @}

  /// @name Initializable Implementation
  /// @{

  public: virtual void initialize(TiObject *context);

  /// @}

}; // class

} // namespace

#endif
