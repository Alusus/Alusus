/**
 * @file Spp/Ast/Template.h
 * Contains the header of class Spp::Ast::Template.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_TEMPLATE_H
#define SPP_AST_TEMPLATE_H

namespace Spp { namespace Ast
{

using namespace Core;

class Seeker;

class Template : public Core::Data::Node,
                 public virtual Core::Data::MapContainer, public virtual Core::Data::IdHolder,
                 public virtual Core::Data::Ast::MetadataHolder, public virtual Core::Data::Clonable,
                 public virtual Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Template, Core::Data::Node, "Spp.Ast", "Spp", "alusus.net");
  IMPLEMENT_INTERFACES(Core::Data::Node, Core::Data::MapContainer, Core::Data::IdHolder, Core::Data::Ast::MetadataHolder,
                                         Core::Data::Clonable, Core::Data::Printable);


  //============================================================================
  // Types

  public: s_enum(VarType, INTEGER, STRING, ALIAS);

  public: typedef std::pair<Str, VarType> VarDef;


  //============================================================================
  // Member Variables

  private: std::vector<VarDef> varDefs;

  private: std::vector<SharedPtr<Block>> instances;

  private: SharedPtr<Data::Clonable> templateBody;

  IMPLEMENT_MAP_CONTAINER((Data::Clonable, templateBody));


  //============================================================================
  // Constructors & Destructor

  public: Template()
  {
  }

  public: Template(SharedPtr<Data::Clonable> const &body, const std::initializer_list<VarDef> &vars)
    : templateBody(body), varDefs(vars)
  {
    OWN_SHAREDPTR(this->templateBody);
  }

  public: virtual ~Template()
  {
    DISOWN_SHAREDPTR(this->templateBody);
  }


  //============================================================================
  // Member Functions

  public: virtual Str const& getName() const
  {
    return ID_GENERATOR->getDesc(this->getId());
  }

  public: void setTemplateBody(SharedPtr<Data::Clonable> const &body)
  {
    UPDATE_OWNED_SHAREDPTR(this->templateBody, body);
  }

  public: SharedPtr<Data::Clonable> const& getTemplateBody() const
  {
    return this->templateBody;
  }

  public: void setVarDefs(std::vector<VarDef> const *defs)
  {
    this->varDefs = *defs;
  }

  public: void setVarDefs(std::initializer_list<VarDef> const &vars)
  {
    this->varDefs = vars;
  }

  public: std::vector<VarDef> const* getVarDefs() const
  {
    return &this->varDefs;
  }

  public: virtual SharedPtr<Block> const& getDefaultInstance(Seeker *seeker);

  public: virtual SharedPtr<Block> const& getInstance(TiObject *templateInput, Seeker *seeker);

  private: Bool matchTemplateVars(TiObject *templateInput, Block *instance, Seeker *seeker);

  private: void assignTemplateVars(TiObject *templateInput, Block *instance, Seeker *seeker);


  //============================================================================
  // Clonable Implementation

  public: virtual SharedPtr<TiObject> clone() const;


  //============================================================================
  // Printable Implementation

  public: virtual void print(OutStream &stream, Int indents=0) const;

}; // class

} } // namespace

#endif
