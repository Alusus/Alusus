/**
 * @file Core/Data/Ast/Module.h
 * Contains the header of class Core::Data::Ast::Module.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_MODULE_H
#define CORE_DATA_AST_MODULE_H

namespace Core { namespace Data { namespace Ast
{

// TODO: DOC

class Module : public SharedMap,
               public virtual IdHolder, public virtual MetadataHolder, public virtual Clonable, public virtual Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Module, SharedMap, "Core.Data.Ast", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_4(Module, IdHolder, MetadataHolder, Clonable, Printable);


  //============================================================================
  // Constructor / Destructor

  public: Module() : SharedMap(true)
  {
  }

  public: Module(Word pid, SourceLocation const &sl) :
    SharedMap(true), MetadataHolder(pid, sl)
  {
  }

  public: Module(const std::initializer_list<Argument<Char const*>> &args);

  public: Module(Word pid, const std::initializer_list<Argument<Char const*>> &args) :
    Module(args)
  {
    this->setProdId(pid);
  }

  public: Module(Word pid, SourceLocation const &sl,
                 const std::initializer_list<Argument<Char const*>> &args) :
    Module(args)
  {
    this->setProdId(pid);
    this->setSourceLocation(sl);
  }

  public: virtual ~Module()
  {
  }

  public: static SharedPtr<Module> create()
  {
    return std::make_shared<Module>();
  }

  public: static SharedPtr<Module> creaet(Word pid, SourceLocation const &sl)
  {
    return std::make_shared<Module>(pid, sl);
  }

  public: static SharedPtr<Module> create(const std::initializer_list<Argument<Char const*>> &args)
  {
    return std::make_shared<Module>(args);
  }

  public: static SharedPtr<Module> create(Word pid, const std::initializer_list<Argument<Char const*>> &args)
  {
    return std::make_shared<Module>(pid, args);
  }

  public: static SharedPtr<Module> create(Word pid, SourceLocation const &sl,
                                          const std::initializer_list<Argument<Char const*>> &args)
  {
    return std::make_shared<Module>(pid, sl, args);
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
  // Clonable Implementation

  public: virtual SharedPtr<TiObject> clone() const;


  //============================================================================
  // Printable Implementation

  public: virtual void print(OutStream &stream, Int indents=0) const;

}; // class

} } } // namespace

#endif
