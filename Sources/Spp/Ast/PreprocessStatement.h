/**
 * @file Spp/Ast/PreprocessStatement.h
 * Contains the header of class Spp::Ast::PreprocessStatement.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_PREPROCESSSTATEMENT_H
#define SPP_AST_PREPROCESSSTATEMENT_H

namespace Spp::Ast
{

class PreprocessStatement : public Core::Ast::Node,
                            public MapContaining<Core::Ast::Node>
{
  //============================================================================
  // Type Info

  TYPE_INFO(PreprocessStatement, Core::Ast::Node, "Spp.Ast", "Spp", "alusus.org");
  IMPLEMENT_INTERFACES(
    Core::Ast::Node, MapContaining<Core::Ast::Node>
  );
  OBJECT_FACTORY(PreprocessStatement);


  //============================================================================
  // Member Variables

  private: SharedPtr<Core::Ast::Node> body;


  //============================================================================
  // Implementations

  IMPLEMENT_MAP_CONTAINING(MapContaining<Core::Ast::Node>,
    (body, Core::Ast::Node, SHARED_REF, setBody(value), body.get())
  );


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(PreprocessStatement);

  IMPLEMENT_ATTR_CONSTRUCTOR(PreprocessStatement);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(PreprocessStatement);

  public: virtual ~PreprocessStatement()
  {
    DISOWN_SHAREDPTR(this->body);
  }


  //============================================================================
  // Member Functions

  public: void setBody(SharedPtr<Core::Ast::Node> const &b)
  {
    UPDATE_OWNED_SHAREDPTR(this->body, b);
  }
  private: void setBody(Core::Ast::Node *b)
  {
    this->setBody(getSharedPtr(b));
  }

  public: SharedPtr<Core::Ast::Node> const& getBody() const
  {
    return this->body;
  }

}; // class

} // namespace

#endif
