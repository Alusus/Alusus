/**
 * @file Scg/Logger.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <Logger.h>

namespace Scg
{
//Logger Logger::this->theLogger;

//------------------------------------------------------------------------------------------------

Logger::Logger(const std::string &tracesFileName, const std::string &debugsFileName, const std::string &infosFileName,
               const std::string &warningsFileName, const std::string &errorsFileName, const std::string &fatalsFileName) :
  tracesStream(tracesFileName.c_str(), std::ios::out),
  debugsStream(debugsFileName.c_str(), std::ios::out),
  infosStream(infosFileName.c_str(), std::ios::out),
  warningsStream(warningsFileName.c_str(), std::ios::out),
  errorsStream(errorsFileName.c_str(), std::ios::out),
  fatalsStream(fatalsFileName.c_str(), std::ios::out)
{
}

//------------------------------------------------------------------------------------------------

Logger::~Logger()
{
  this->tracesStream.close();
  this->debugsStream.close();
  this->infosStream.close();
  this->warningsStream.close();
  this->errorsStream.close();
  this->fatalsStream.close();
}

//------------------------------------------------------------------------------------------------

void Logger::logMessage(MessageType type, const std::string &message)
{
  switch (type) {
#ifdef _DEBUG

  case Trace:
    this->tracesStream << message.c_str() << std::endl;
    return;

  case Debug:
    this->debugsStream << message.c_str() << std::endl;
    return;
#endif

  case Info:
    this->infosStream << message.c_str() << std::endl;
    return;

  case Warning:
    this->warningsStream << message.c_str() << std::endl;
    return;

  case Error:
    this->errorsStream << message.c_str() << std::endl;
    return;

  case Fatal:
    this->fatalsStream << message.c_str() << std::endl;
    return;

  default:
    return;
  }
}
}
