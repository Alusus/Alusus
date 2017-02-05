/**
 * @file Core/Basic/validation.h
 * Contains definitions related to validations.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_VALIDATION_H
#define CORE_BASIC_VALIDATION_H

#define _VALIDATE_NOT_NULL1(name) \
  if (name == 0) { \
    throw EXCEPTION(InvalidArgumentException, STR(#name), STR("Should not be null.")); \
  }
#define _VALIDATE_NOT_NULL2(name1, name2) \
  _VALIDATE_NOT_NULL1(name1); \
  _VALIDATE_NOT_NULL1(name2);
#define _VALIDATE_NOT_NULL3(name1, name2, name3) \
  _VALIDATE_NOT_NULL2(name1, name2); \
  _VALIDATE_NOT_NULL1(name3);
#define _VALIDATE_NOT_NULL4(name1, name2, name3, name4) \
  _VALIDATE_NOT_NULL3(name1, name2, name3); \
  _VALIDATE_NOT_NULL1(name4);
#define _VALIDATE_NOT_NULL5(name1, name2, name3, name4, name5) \
  _VALIDATE_NOT_NULL4(name1, name2, name3, name4); \
  _VALIDATE_NOT_NULL1(name5);
#define _VALIDATE_NOT_NULL6(name1, name2, name3, name4, name5, name6) \
  _VALIDATE_NOT_NULL5(name1, name2, name3, name4, name5); \
  _VALIDATE_NOT_NULL1(name6);
#define VALIDATE_NOT_NULL(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, \
               _VALIDATE_NOT_NULL6, \
               _VALIDATE_NOT_NULL5, \
               _VALIDATE_NOT_NULL4, \
               _VALIDATE_NOT_NULL3, \
               _VALIDATE_NOT_NULL2, \
               _VALIDATE_NOT_NULL1)(__VA_ARGS__)

#endif
