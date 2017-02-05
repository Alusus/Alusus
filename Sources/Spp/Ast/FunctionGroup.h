/**
 * @file Spp/Ast/FunctionGroup.h
 * Contains the header of class Spp::Ast::FunctionGroup.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_FUNCTIONGROUP_H
#define SPP_AST_FUNCTIONGROUP_H

namespace Spp { namespace Ast
{

class FunctionGroup : public Core::Data::SharedList,
                      public virtual Core::Data::IdHolder, public virtual Core::Data::MetadataHolder,
                      public virtual Core::Data::Clonable, public virtual Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(FunctionGroup, Core::Data::SharedList, "Spp.Ast", "Spp", "alusus.net");
  IMPLEMENT_INTERFACES(Core::Data::SharedList, Core::Data::IdHolder, Core::Data::MetadataHolder,
                                               Core::Data::Clonable, Core::Data::Printable);

  IMPLEMENT_AST_LIST_CLONABLE(FunctionGroup);

  IMPLEMENT_AST_LIST_PRINTABLE(FunctionGroup);


  //============================================================================
  // Constructors & Destructor

  public: FunctionGroup()
  {
  }

  public: FunctionGroup(Word pid, SourceLocation const &sl) :
    MetadataHolder(pid, sl)
  {
  }

  public: FunctionGroup(Word pid, const std::initializer_list<SharedPtr<Function>> &args) :
    MetadataHolder(pid), SharedList(args)
  {
  }

  public: FunctionGroup(Word pid, SourceLocation const &sl,
                        const std::initializer_list<SharedPtr<Function>> &args) :
    MetadataHolder(pid, sl), SharedList(args)
  {
  }

  public: virtual ~FunctionGroup()
  {
  }

  public: static SharedPtr<FunctionGroup> create()
  {
    return std::make_shared<FunctionGroup>();
  }

  public: static SharedPtr<FunctionGroup> create(Word pid, SourceLocation const &sl)
  {
    return std::make_shared<FunctionGroup>(pid, sl);
  }

  public: static SharedPtr<FunctionGroup> create(Word pid,
                                                 const std::initializer_list<SharedPtr<Function>> &args)
  {
    return std::make_shared<FunctionGroup>(pid, args);
  }

  public: static SharedPtr<FunctionGroup> create(Word pid, SourceLocation const &sl,
                                                 const std::initializer_list<SharedPtr<Function>> &args)
  {
    return std::make_shared<FunctionGroup>(pid, sl, args);
  }


  //============================================================================
  // Member Functions

  public: virtual Str const& getName() const
  {
    return ID_GENERATOR->getDesc(this->getId());
  }

  public: void addFunction(SharedPtr<Function> const &f)
  {
    this->add(f);
  }

  public: SharedPtr<TiObject> getFunction(Int index) const
  {
    return this->getShared(index).ti_cast<Function>();
  }

  public: virtual SharedPtr<Function> findMatchingFunction(TiObject *input, Core::Data::Seeker *seeker)
  {
    // TODO:
    throw EXCEPTION(NotImplementedException, "Not implemented yet.");
  }

  private: Bool matchSignature(TiObject *input, Function *function, Core::Data::Seeker *seeker)
  {
    // TODO:
    throw EXCEPTION(NotImplementedException, "Not implemented yet.");
  }

}; // class

} } // namespace

#endif
