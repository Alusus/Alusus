/**
 * @file Scg/macros.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __macros_h__
#define __macros_h__

#ifdef _DEBUG
#define MODULE_CHECK                                                           \
  if (GetModule() == 0)                                                        \
    THROW_EXCEPTION(UnattachedExpressionException,                             \
    "This expression must be contained in a module!");
#else
#define MODULE_CHECK
#endif

#ifdef _DEBUG
#define FUNCTION_CHECK                                                         \
  if (GetFunction() == 0)                                                      \
    THROW_EXCEPTION(UnattachedExpressionException,                             \
    "This expression must be contained in a function!");
#else
#define FUNCTION_CHECK
#endif

#ifdef _DEBUG
#define BLOCK_CHECK                                                            \
  if (GetBlock() == 0)                                                         \
    THROW_EXCEPTION(UnattachedExpressionException,                             \
    "This expression must be contained in a block!");
#else
#define BLOCK_CHECK
#endif

#ifdef _DEBUG
#define MODULE_OR_BLOCK_CHECK                                                  \
  if (GetModule() == nullptr && GetBlock() == nullptr)                         \
    THROW_EXCEPTION(UnattachedExpressionException,                             \
    "This expression must be contained in a module or a block!");
#else
#define MODULE_OR_BLOCK_CHECK
#endif

#ifdef _DEBUG
  #define LOG_TRACE(message)  Logger::GetSingleton().LogMessage(Logger::Trace, (message));
  #define LOG_DEBUG(message)  Logger::GetSingleton().LogMessage(Logger::Debug, (message));
#else
  #define LOG_TRACE(message)
  #define LOG_DEBUG(message)
#endif
#define LOG_INFO(message)     Logger::GetSingleton().LogMessage(Logger::Info, (message));
#define LOG_WARNING(message)  Logger::GetSingleton().LogMessage(Logger::Warning, (message));
#define LOG_ERROR(message)    Logger::GetSingleton().LogMessage(Logger::Error, (message));
#define LOG_FATAL(message)    Logger::GetSingleton().LogMessage(Logger::Fatal, (message));

#define MAX_PRE_CODE_GEN_REPEAT  1000
#define MAX_POST_CODE_GEN_REPEAT  1000

#define SAFE_DELETE_LLVM_INST(inst)                                            \
    if (inst != nullptr)                                                       \
    {                                                                          \
      if (!inst->hasNUses(0))                                                  \
        return CodeGenerationStage::PostCodeGeneration;                        \
      inst->eraseFromParent();                                                 \
      inst = nullptr;                                                          \
    }

#endif // __macros_h__
