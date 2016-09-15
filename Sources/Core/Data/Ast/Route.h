/**
 * @file Core/Data/Ast/Route.h
 * Contains the header of class Core::Data::Ast::Route.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_ROUTE_H
#define CORE_DATA_AST_ROUTE_H

namespace Core { namespace Data { namespace Ast
{

// TODO: DOC

/**
 * @brief Contains parsed information of a single route in a PRT.
 * @ingroup data
 *
 * A decision made at an alternative or optional term is recorded in this object
 * by the GenericParsingHandler along with the child data resulting from
 * taking that route. This object is created by the GenericParsingHandler to
 * compose the Parsing Representation Tree (PRT).
 */
class Route : public Node,
              public virtual MetadataHolder, public virtual MapContainer,
              public virtual Clonable, public virtual Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Route, Node, "Core.Data.Ast", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_4(Node, MetadataHolder, MapContainer, Clonable, Printable);


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
  public: Int route;

  /**
   * @brief The child parsed data for the term.
   *
   * The parsed data from the terms contained in the selected route (if any)
   * is recorded to this pointer.
   */
  public: SharedPtr<TiObject> data;

  IMPLEMENT_MAP_CONTAINER((TiObject, data));

  IMPLEMENT_AST_LIST_PRINTABLE(Route, << this->route);


  //============================================================================
  // Constructor / Destructor

  public: Route(Word pid=UNKNOWN_ID, Int r=-1, TiObject *d=0) :
    MetadataHolder(pid), route(r), data(d)
  {
    OWN_SHAREDPTR(this->data);
  }

  public: Route(Word pid, Int r, SharedPtr<TiObject> const &d) :
    MetadataHolder(pid), route(r), data(d)
  {
    OWN_SHAREDPTR(this->data);
  }

  public: Route(Word pid, Int r, SourceLocation const &sl, TiObject *d=0) :
    MetadataHolder(pid, sl), route(r), data(d)
  {
    OWN_SHAREDPTR(this->data);
  }

  public: Route(Word pid, Int r, SourceLocation const &sl, SharedPtr<TiObject> const &d) :
    MetadataHolder(pid, sl), route(r), data(d)
  {
    OWN_SHAREDPTR(this->data);
  }

  public: virtual ~Route()
  {
    DISOWN_SHAREDPTR(this->data);
  }

  public: static SharedPtr<Route> create(Word pid=UNKNOWN_ID, Int r=-1, TiObject *d=0)
  {
    return std::make_shared<Route>(pid, r, d);
  }

  public: static SharedPtr<Route> create(Word pid, Int r, SharedPtr<TiObject> const &d)
  {
    return std::make_shared<Route>(pid, r, d);
  }

  public: static SharedPtr<Route> create(Word pid, Int r, SourceLocation const &sl, TiObject *d=0)
  {
    return std::make_shared<Route>(pid, r, sl, d);
  }

  public: static SharedPtr<Route> create(Word pid, Int r, SourceLocation const &sl,
                                         SharedPtr<TiObject> const &d)
  {
    return std::make_shared<Route>(pid, r, sl, d);
  }


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

  /**
   * @brief Get the route index.
   *
   * The value of this parameter gives an indication to the parsing decision
   * made on the term. The value depends on the type of the term.
   * Alternative Term: For alternative terms this value refers to the index
   *                   of the selected term/path.
   * Optional Term: For optional terms this value can be 0 or 1.
   */
  public: Int getRoute() const
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
