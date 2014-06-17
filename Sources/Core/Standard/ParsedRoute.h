/**
 * @file Core/Standard/ParsedRoute.h
 * Contains the header of class Core::Standard::ParsedRoute.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef STANDARD_PARSED_ROUTE_H
#define STANDARD_PARSED_ROUTE_H

namespace Core { namespace Standard
{

/**
 * @brief Contains parsed information of a single route.
 * @ingroup standard
 *
 * A decision made at an alternative or optional term is recorded in this object
 * by the GenericParsingHandler along with the child data resulting from
 * taking that route. This object is created by the GenericParsingHandler to
 * compose the parsed tree.
 */
class ParsedRoute : public ParsedItem
{
  //============================================================================
  // Type Info

  TYPE_INFO(ParsedRoute, ParsedItem, "Core.Standard", "Core", "alusus.net");


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
  public: SharedPtr<IdentifiableObject> data;


  //============================================================================
  // Constructor / Destructor

  public: ParsedRoute(Word pid=UNKNOWN_ID, Int r=-1, IdentifiableObject *d=0) :
    ParsedItem(pid), route(r), data(d)
  {
  }

  public: ParsedRoute(Word pid, Int r, SharedPtr<IdentifiableObject> const &d) :
    ParsedItem(pid), route(r), data(d)
  {
  }

  public: ParsedRoute(Word pid, Int r, Int l, Int c, IdentifiableObject *d=0) :
    ParsedItem(pid, c, l), route(r), data(d)
  {
  }

  public: ParsedRoute(Word pid, Int r, Int l, Int c, SharedPtr<IdentifiableObject> const &d) :
    ParsedItem(pid, c, l), route(r), data(d)
  {
  }

  public: virtual ~ParsedRoute()
  {
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
  public: void setData(SharedPtr<IdentifiableObject> const &d)
  {
    this->data = d;
  }

  /**
   * @brief Get a smart pointer to the child parsed data.
   *
   * This is the parsed data from the terms contained in the selected route
   * (if any) is recorded to this pointer.
   */
  public: SharedPtr<IdentifiableObject> const& getData() const
  {
    return this->data;
  }

  /**
   * @brief Override the original implementation to search the tree if needed.
   * If the value is not yet set for this object and it has children, it will
   * call getLine on the children.
   */
  public: virtual Int getLine() const
  {
    if (ParsedItem::getLine() == -1) {
      SharedPtr<ParsedItem> ptr = this->getData().io_cast<ParsedItem>();
      if (ptr != 0) {
        Int l = ptr->getLine();
        if (l != -1) return l;
      }
    }
    return ParsedItem::getLine();
  }

  /**
   * @brief Override the original implementation to search the tree if needed.
   * If the value is not yet set for this object and it has children, it will
   * call getColumn on the children.
   */
  public: virtual Int getColumn() const
  {
    if (ParsedItem::getColumn() == -1) {
      SharedPtr<ParsedItem> ptr = this->getData().io_cast<ParsedItem>();
      if (ptr != 0) {
        Int l = ptr->getColumn();
        if (l != -1) return l;
      }
    }
    return ParsedItem::getColumn();
  }

}; // class

} } // namespace

#endif
