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

  public: ParsedModule(Word pid=UNKNOWN_ID, Int l=-1, Int c=-1) :
    ParsingMetadataHolder(pid, l, c)
  {
  }

  public: ParsedModule(Word pid, const std::initializer_list<Argument<Char const*>> &args) :
    ParsingMetadataHolder(pid, -1, -1), Module(args)
  {
  }

  public: ParsedModule(Word pid, Int l, Int c, const std::initializer_list<Argument<Char const*>> &args) :
    ParsingMetadataHolder(pid, l, c), Module(args)
  {
  }

  public: virtual ~ParsedModule()
  {
  }

  public: static SharedPtr<ParsedModule> creaet(Word pid=UNKNOWN_ID, Int l=-1, Int c=-1)
  {
    return std::make_shared<ParsedModule>(pid, l, c);
  }

  public: static SharedPtr<ParsedModule> create(Word pid, const std::initializer_list<Argument<Char const*>> &args)
  {
    return std::make_shared<ParsedModule>(pid, args);
  }

  public: static SharedPtr<ParsedModule> create(Word pid, Int l, Int c,
                                                const std::initializer_list<Argument<Char const*>> &args)
  {
    return std::make_shared<ParsedModule>(pid, l, c, args);
  }


  //============================================================================
  // ParsingMetadataHolder Overrides

  /**
   * @brief Override the original implementation to search the tree if needed.
   * If the value is not yet set for this object and it has children, it will
   * call getLine on the children.
   */
  public: virtual Int getLine() const
  {
    if (ParsingMetadataHolder::getLine() == -1) {
      for (Int i = 0; i < this->getCount(); ++i) {
        ParsingMetadataHolder *ptr = ii_cast<ParsingMetadataHolder>(this->get(i).get());
        if (ptr != 0) {
          Int l = ptr->getLine();
          if (l != -1) return l;
        }
      }
    }
    return ParsingMetadataHolder::getLine();
  }

  /**
   * @brief Override the original implementation to search the tree if needed.
   * If the value is not yet set for this object and it has children, it will
   * call getColumn on the children.
   */
  public: virtual Int getColumn() const
  {
    if (ParsingMetadataHolder::getColumn() == -1) {
      for (Int i = 0; i < this->getCount(); ++i) {
        ParsingMetadataHolder *ptr = ii_cast<ParsingMetadataHolder>(this->get(i).get());
        if (ptr != 0) {
          Int l = ptr->getColumn();
          if (l != -1) return l;
        }
      }
    }
    return ParsingMetadataHolder::getColumn();
  }

  public: virtual IdentifiableObject* getAttribute(Char const *name)
  {
    if (SBSTR(name) == STR("line")) {
      if (ParsingMetadataHolder::getLine() == -1) {
        for (Int i = 0; i < this->getCount(); ++i) {
          ParsingMetadataHolder *ptr = ii_cast<ParsingMetadataHolder>(this->get(i).get());
          if (ptr != 0) {
            if (ptr->getLine() != -1) return ptr->getAttribute(name);
          }
        }
      }
    } else if (SBSTR(name) == STR("column")) {
      if (ParsingMetadataHolder::getColumn() == -1) {
        for (Int i = 0; i < this->getCount(); ++i) {
          ParsingMetadataHolder *ptr = ii_cast<ParsingMetadataHolder>(this->get(i).get());
          if (ptr != 0) {
            if (ptr->getColumn() != -1) return ptr->getAttribute(name);
          }
        }
      }
    }
    return ParsingMetadataHolder::getAttribute(name);
  }

}; // class

} } // namespace

#endif
