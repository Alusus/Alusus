/**
 * @file Spp/Notices/MultipleCalleeMatchNotice.h
 * Contains the header of class Spp::MultipleCalleeMatchNotice.
 *
 * @copyright Copyright (C) 2023 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_NOTICES_MULTIPLECALLEEMATCHNOTICE_H
#define SPP_NOTICES_MULTIPLECALLEEMATCHNOTICE_H

namespace Spp::Notices
{

/**
 * @brief Triggered when multiple callees are foudn matching the given params.
 * @ingroup spp_notices
 */
class MultipleCalleeMatchNotice : public Core::Notices::Notice
{
  //============================================================================
  // Type Info

  TYPE_INFO(MultipleCalleeMatchNotice, Core::Notices::Notice, "Spp.Notices", "Spp", "alusus.org");


  //============================================================================
  // Member Variables

  private: SharedPtr<Data::SourceLocation> sourceLocation1;
  private: SharedPtr<Data::SourceLocation> sourceLocation2;


  //============================================================================
  // Constructor / Destructor

  public: MultipleCalleeMatchNotice(
    SharedPtr<Data::SourceLocation> const &sl1,
    SharedPtr<Data::SourceLocation> const &sl2
  ) : sourceLocation1(sl1), sourceLocation2(sl2)
  {
  }

  public: MultipleCalleeMatchNotice(
    SharedPtr<Data::SourceLocation> const &sl,
    SharedPtr<Data::SourceLocation> const &sl1,
    SharedPtr<Data::SourceLocation> const &sl2
  ) : Notice(sl), sourceLocation1(sl1), sourceLocation2(sl2)
  {
  }


  //============================================================================
  // Member Functions

  public: virtual Str const& getCode() const
  {
    static Str _code(S("SPPA1005"));
    return _code;
  }

  public: virtual Int getSeverity() const
  {
    return 1;
  }

  public: virtual void buildDescription(Str &str) const;

  public: virtual Bool isEqual(Notice *notice) const;
};

} // namespace

#endif
