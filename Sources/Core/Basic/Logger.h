/**
 * @file Core/Basic/Logger.h
 * Contains the header of class Core::Basic::Logger.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifdef USE_LOGS

#ifndef CORE_BASIC_LOGGER_H
#define CORE_BASIC_LOGGER_H

namespace Core { namespace Basic
{

/**
 * @brief Provides logging mechanism with different logging levels.
 * @ingroup basic_utils
 *
 * This is a singleton class.
 */
class Logger
{
  //============================================================================
  // Member Variables

  /**
   * @brief A bitwise filter for logged events.
   *
   * Logger objects match their level with this filter using bitwise and and
   * only print messsages if the result was not zero. Default value for the
   * filter is -1 (display all).
   */
  public: static Word *filter;

  /**
   * @brief The level of this logging object.
   * @sa filter
   */
  private: const Word level;


  //============================================================================
  // Constructor

  public: Logger(Word l) : level(l)
  {
  }


  //============================================================================
  // Member Functions

  /// Log an argument.
  public: template <class T> const Logger& operator << (const T &arg) const
  {
    if (this->level & Logger::getFilter()) std::clog << arg;
    return *this;
  }

  public: static void setFilter(Word f)
  {
    *(Logger::getFilterPtr()) = f;
  }

  public: static Word getFilter()
  {
    return *(Logger::getFilterPtr());
  }

  private: static Word* getFilterPtr();

}; // class

} } // namespace

#endif

#endif
