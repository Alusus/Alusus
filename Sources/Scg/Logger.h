/**
 * @file Scg/Logger.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __Logger_h__
#define __Logger_h__

// STL header files
#include <string>
#include <fstream>

// Scg header files
#include <typedefs.h>

namespace Scg
{
  /**
   * Enable the user to log different message types during execution.
   */
  class Logger
  {
  public:
    enum MessageType
    {
      Trace,    //!< Very detailed logs, typically only enabled during development.
      Debug,    //!< Debugging information, typically not enabled in production environment.
      Info,     //!< General information messages.
      Warning,  //!< Warning message
      Error,    //!< Error message
      Fatal     //!< Fatal error message
    };

  private:
    //static Logger theLogger;
    std::ofstream tracesStream;
    std::ofstream debugsStream;
    std::ofstream infosStream;
    std::ofstream warningsStream;
    std::ofstream errorsStream;
    std::ofstream fatalsStream;

  public:
    /**
     * Constructs a logger with the given file names for output.
     *
     * @param[in] tracesFileName  The name of the file to write trace messages to.
     * @param[in] debugsFileName  The name of the file to write debug messages to.
     * @param[in] infosFileName   The name of the file to write info messages to.
     * @param[in] warningsileName The name of the file to write warning messages to.
     * @param[in] errorsFileName  The name of the file to write error messages to.
     * @param[in] fatalsFileName  The name of the file to write fatal error messages to.
     */
    Logger(const std::string &tracesFileName = "traces.log",
      const std::string &debugsFileName = "debugs.log",
      const std::string &infosFileName = "infos.log",
      const std::string &warningsFileName = "warnings.log",
      const std::string &errorsFileName = "errors.log",
      const std::string &fatalsFileName = "fatals.log");

    /**
     * Class destructor.
     */
    ~Logger();

    /**
     * Log a message.
     *
     * @param[in] type      The type of the message, e.g. trace.
     * @param[in] message   A string containing the message.
     */
    void LogMessage(MessageType type, const std::string &message);

    /**
     * Get the default logger.
     */
    /*static Logger &GetSingleton()
    {
      return theLogger;
    }*/

  private:
    // Disable copy constructor and assignment because the streams defined above cannot be
    // copied. This way we avoid getting hard-to-understand compilation errors in case
    // the user tried to copy a Logger instance.
    Logger(const Logger&);
    Logger &operator = (const Logger&);
  };
}

#endif // __Logger_h__
