/**
 * @file Core/Data/PrtModule.h
 * Contains the header of class Core::Data::PrtModule.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_PRTMODULE_H
#define CORE_DATA_PRTMODULE_H

namespace Core { namespace Data
{

// TODO: DOC

class PrtModule : public Module,
                  public virtual ParsingMetadataHolder, public virtual Clonable
{
  //============================================================================
  // Type Info

  TYPE_INFO(PrtModule, Module, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_2(Module, ParsingMetadataHolder, Clonable);


  //============================================================================
  // Constructor / Destructor

  public: PrtModule()
  {
  }

  public: PrtModule(Word pid, SourceLocation const &sl) :
    ParsingMetadataHolder(pid, sl)
  {
  }

  public: PrtModule(Word pid, const std::initializer_list<Argument<Char const*>> &args) :
    ParsingMetadataHolder(pid), Module(args)
  {
  }

  public: PrtModule(Word pid, SourceLocation const &sl,
                       const std::initializer_list<Argument<Char const*>> &args) :
    ParsingMetadataHolder(pid, sl), Module(args)
  {
  }

  public: virtual ~PrtModule()
  {
  }

  public: static SharedPtr<PrtModule> create()
  {
    return std::make_shared<PrtModule>();
  }

  public: static SharedPtr<PrtModule> creaet(Word pid, SourceLocation const &sl)
  {
    return std::make_shared<PrtModule>(pid, sl);
  }

  public: static SharedPtr<PrtModule> create(Word pid, const std::initializer_list<Argument<Char const*>> &args)
  {
    return std::make_shared<PrtModule>(pid, args);
  }

  public: static SharedPtr<PrtModule> create(Word pid, SourceLocation const &sl,
                                                const std::initializer_list<Argument<Char const*>> &args)
  {
    return std::make_shared<PrtModule>(pid, sl, args);
  }


  //============================================================================
  // ParsingMetadataHolder Overrides

  /**
   * @brief Override the original implementation to search the tree if needed.
   * If the value is not yet set for this object and it has children, it will
   * call getSourceLocation on the children.
   */
  public: virtual SourceLocation const& getSourceLocation() const
  {
    if (ParsingMetadataHolder::getSourceLocation().line == 0) {
      for (Int i = 0; i < this->getCount(); ++i) {
        ParsingMetadataHolder *ptr = ii_cast<ParsingMetadataHolder>(this->get(i));
        if (ptr != 0) {
          SourceLocation const &sl = ptr->getSourceLocation();
          if (sl.line != 0) return sl;
        }
      }
    }
    return ParsingMetadataHolder::getSourceLocation();
  }

  public: virtual IdentifiableObject* getAttribute(Char const *name)
  {
    if (SBSTR(name) == STR("sourceLocation")) {
      if (ParsingMetadataHolder::getSourceLocation().line == 0) {
        for (Int i = 0; i < this->getCount(); ++i) {
          ParsingMetadataHolder *ptr = ii_cast<ParsingMetadataHolder>(this->get(i));
          if (ptr != 0) {
            if (ptr->getSourceLocation().line != 0) return ptr->getAttribute(name);
          }
        }
      }
    }
    return ParsingMetadataHolder::getAttribute(name);
  }


  //============================================================================
  // Clonable Overrides

  public: virtual SharedPtr<IdentifiableObject> clone() const
  {
    SharedPtr<PrtModule> newModule = std::make_shared<PrtModule>();
    newModule->setProdId(this->getProdId());
    for (Word i = 0; i < this->getCount(); ++i) {
      newModule->add(this->getKey(i).c_str(), this->getShared(i));
    }
    newModule->setSourceLocation(this->getSourceLocation());
    return newModule;
  }

}; // class

} } // namespace

#endif
