/**
 * @file Core/Data/Grammar/Map.h
 * Contains the header of class Core::Data::Grammar::Map.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_GRAMMAR_MAP_H
#define CORE_DATA_GRAMMAR_MAP_H

namespace Core::Data::Grammar
{

class Map : public NbMap, public Binding, public Inheriting
{
  //============================================================================
  // Type Info

  TYPE_INFO(Map, NbMap, "Core.Data.Grammar", "Core", "alusus.org", (
    INHERITANCE_INTERFACES(Binding, Inheriting)
  ));


  //============================================================================
  // Member Variables

  private: SharedPtr<Reference> baseRef;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Binding,
    (baseRef, Reference, SHARED_REF, setBaseRef(value), baseRef.get())
  );


  //============================================================================
  // Constructor / Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Map);

  IMPLEMENT_ATTR_CONSTRUCTOR(Map);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(Map);

  public: Map(
    std::initializer_list<Argument> const &attrs, std::initializer_list<Argument> const &elements, Bool useIndex
  ) : NbMap(useIndex)
  {
    ATTR_INITIALIZATION_LOOP(attrs)
    MAP_ELEMENT_INITIALIZATION_LOOP(elements)
  }

  public: static SharedPtr<Map> create(
    std::initializer_list<Argument> const &attrs, std::initializer_list<Argument> const &elements, Bool useIndex
  ) {
    return std::make_shared<Map>(attrs, elements, useIndex);
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

  public: using NbMap::setBase;

  /// @}

  /// @name Inheriting Implementation
  /// @{

  public: virtual Reference* getBaseReference() const
  {
    return this->baseRef.get();
  }

  public: virtual void setBase(TiObject *base)
  {
    Map *baseMap = ti_cast<Map>(base);
    if (baseMap == 0) {
      throw EXCEPTION(GenericException, S("Base reference points to an object of an invalid type."));
    }
    this->setBase(baseMap);
  }

  public: virtual TiObject* getBase() const
  {
    return NbMap::getBase();
  }

  /// @}

}; // class

} // namespace

#endif
