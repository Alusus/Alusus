/**
 * @file Core/Data/ParsedModule.h
 * Contains the header of class Core::Data::ParsedModule.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_PARSEDMODULE_H
#define DATA_PARSEDMODULE_H

namespace Core { namespace Data
{

// TODO: DOC

class ParsedModule : public Module,
                     public virtual ParsingMetadataHolder
{
  //============================================================================
  // Type Info

  TYPE_INFO(ParsedModule, Module, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_1(Module, ParsingMetadataHolder);


  //============================================================================
  // Constructor / Destructor

  public: ParsedModule()
  {
  }

  public: ParsedModule(Word pid, SourceLocation const &sl) :
    ParsingMetadataHolder(pid, sl)
  {
  }

  public: ParsedModule(Word pid, const std::initializer_list<Argument<Char const*>> &args) :
    ParsingMetadataHolder(pid), Module(args)
  {
  }

  public: ParsedModule(Word pid, SourceLocation const &sl,
                       const std::initializer_list<Argument<Char const*>> &args) :
    ParsingMetadataHolder(pid, sl), Module(args)
  {
  }

  public: virtual ~ParsedModule()
  {
  }

  public: static SharedPtr<ParsedModule> create()
  {
    return std::make_shared<ParsedModule>();
  }

  public: static SharedPtr<ParsedModule> creaet(Word pid, SourceLocation const &sl)
  {
    return std::make_shared<ParsedModule>(pid, sl);
  }

  public: static SharedPtr<ParsedModule> create(Word pid, const std::initializer_list<Argument<Char const*>> &args)
  {
    return std::make_shared<ParsedModule>(pid, args);
  }

  public: static SharedPtr<ParsedModule> create(Word pid, SourceLocation const &sl,
                                                const std::initializer_list<Argument<Char const*>> &args)
  {
    return std::make_shared<ParsedModule>(pid, sl, args);
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

}; // class

} } // namespace

#endif
