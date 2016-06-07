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
              public virtual MetadataHolder, public virtual Container, public virtual Clonable, public virtual Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Route, Node, "Core.Data.Ast", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_4(Node, MetadataHolder, Container, Clonable, Printable);


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
  // MetadataHolder Overrides

  /**
   * @brief Override the original implementation to search the tree if needed.
   * If the value is not yet set for this object and it has children, it will
   * call getSourceLocation on the children.
   */
  public: virtual SourceLocation const& getSourceLocation() const;

  public: virtual TiObject* getAttribute(Char const *name);


  //============================================================================
  // Container Implementation

  /// Change the element at the specified index.
  public: virtual void set(Int index, TiObject *val)
  {
    if (index == 0) {
      UPDATE_OWNED_SHAREDPTR(this->data, getSharedPtr(val, true));
    } else {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Must be 0 for this class."));
    }
  }

  /// Remove the element at the specified index.
  public: virtual void remove(Int index)
  {
    if (index != 0) {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Must be 0 for this class."));
    }
    RESET_OWNED_SHAREDPTR(this->data);
  }

  /// Get the count of elements in the list.
  public: virtual Word getCount() const
  {
    return 1;
  }

  /// Get the object at the specified index.
  public: virtual TiObject* get(Int index) const
  {
    if (index != 0) {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Must be 0 for this class."));
    }
    return this->data.get();
  }


  //============================================================================
  // Clonable Implementation

  public: virtual SharedPtr<TiObject> clone() const;


  //============================================================================
  // Printable Implementation

  public: virtual void print(OutStream &stream, Int indents=0) const;

}; // class

} } } // namespace

#endif
