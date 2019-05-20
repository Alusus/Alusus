/**
 * @file Core/Data/Ast/Route.h
 * Contains the header of class Core::Data::Ast::Route.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_ROUTE_H
#define CORE_DATA_AST_ROUTE_H

namespace Core { namespace Data { namespace Ast
{

// TODO: DOC

/**
 * @brief Contains parsed information of a single route in a PRT.
 * @ingroup core_data
 *
 * A decision made at an alternative or optional term is recorded in this object
 * by the GenericParsingHandler along with the child data resulting from
 * taking that route. This object is created by the GenericParsingHandler to
 * compose the Parsing Representation Tree (PRT).
 */
class Route : public Node,
              public Binding, public MapContaining<TiObject>, public MetaHaving,
              public Clonable, public Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Route, Node, "Core.Data.Ast", "Core", "alusus.org");
  IMPLEMENT_INTERFACES(Node, Binding, MapContaining<TiObject>, MetaHaving, Clonable, Printable);


  //============================================================================
  // Member Variables

  /**
   * @brief A parameter representing the parsing decision made.
   *
   * The value of this parameter gives an indication to the parsing decision
   * made on the term. The value depends on the type of the term.
   * Alternative Term: For alternative terms this value refers to the index
   *                   of the selected term/path.
   * Optional Term: For optional terms this value can be 0 or 1.
   */
  public: TiInt route;

  /**
   * @brief The child parsed data for the term.
   *
   * The parsed data from the terms contained in the selected route (if any)
   * is recorded to this pointer.
   */
  public: SharedPtr<TiObject> data;


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(Route);

  IMPLEMENT_BINDING(Binding,
    (route, TiInt, VALUE, setRoute(value), &route),
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>,
    (data, TiObject, SHARED_REF, setData(value), data.get())
  );

  IMPLEMENT_AST_LIST_PRINTABLE(Route, << this->route.get());


  //============================================================================
  // Constructor / Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Route);

  IMPLEMENT_ATTR_CONSTRUCTOR(Route);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(Route);


  //============================================================================
  // Member Functions

  /**
   * @brief Set the route index.
   *
   * The value of this parameter gives an indication to the parsing decision
   * made on the term. The value depends on the type of the term.
   * Alternative Term: For alternative terms this value refers to the index
   *                   of the selected term/path.
   * Optional Term: For optional terms this value can be 0 or 1.
   */
  public: void setRoute(Int r)
  {
    this->route = r;
  }
  public: void setRoute(TiInt const *r)
  {
    this->route = r == 0 ? 0 : r->get();
  }

  /**
   * @brief Get the route index.
   *
   * The value of this parameter gives an indication to the parsing decision
   * made on the term. The value depends on the type of the term.
   * Alternative Term: For alternative terms this value refers to the index
   *                   of the selected term/path.
   * Optional Term: For optional terms this value can be 0 or 1.
   */
  public: TiInt const& getRoute() const
  {
    return this->route;
  }

  /**
   * @brief Set the child parsed data.
   *
   * This is the parsed data from the terms contained in the selected route
   * (if any) is recorded to this pointer.
   */
  public: void setData(SharedPtr<TiObject> const &d)
  {
    UPDATE_OWNED_SHAREDPTR(this->data, d);
  }
  private: void setData(TiObject *d)
  {
    this->setData(getSharedPtr(d));
  }

  /**
   * @brief Get a smart pointer to the child parsed data.
   *
   * This is the parsed data from the terms contained in the selected route
   * (if any) is recorded to this pointer.
   */
  public: SharedPtr<TiObject> const& getData() const
  {
    return this->data;
  }


  //============================================================================
  // Clonable Implementation

  public: virtual SharedPtr<TiObject> clone() const;

}; // class

} } } // namespace

#endif
