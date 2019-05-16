/**
 * @file Core/Processing/InputBuffer.cpp
 * Contains the implementation of Processing::InputBuffer.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Processing
{

//==============================================================================
// Member Functions

/**
 * Push a new character into the buffer.
 *
 * @param ch The new character to push into the buffer.
 * @param sl The source location of the given character.
 * @param force Specifies whether to force the push of the character even if
 *              the buffer is full. When the buffer is full and this value
 *              is true, the last character in the buffer will be replaced
 *              with the new one.
 * @return Returns true if the operation was successful, false otherwise. The
 *         call will fail if the buffer is full and the value of 'force' is
 *         false.
 */
Bool InputBuffer::push(WChar ch, Data::SourceLocationRecord const &sl, Bool force)
{
  // Check if the buffer is full.
  if (this->isFull() == true) {
    if (force) {
      // Replace the last character with this one.
      // The size of the character groups array should not be less than 2.
      ASSERT(this->charGroups.size() >= 2);
      // The size of the characters array should not be 0.
      ASSERT(this->charBuffer.size() >= 1);
      // Replace the last character in the buffer.
      this->charBuffer.at(this->charBuffer.size()-1) = ch;
      // Does the last group have more than one character? If it has only
      // one character then we simply replace that group with a new group,
      // otherwise, we extract one character from that group and we create
      // a new group for the new character.
      if (this->charGroups.at(this->charGroups.size()-2).length > 1) {
        // Extract a character from the last character group.
        CharacterGroup * lastCg = &this->charGroups.at(this->charGroups.size()-2);
        lastCg->length--;
        // Create a new character group.
        CharacterGroup * newCg = &this->charGroups.at(this->charGroups.size()-1);
        newCg->sourceLocation = sl;
        newCg->length = 1;
        // Define the ending character group, which only contains the expected
        // position of the next character.
        CharacterGroup endCg;
        endCg.sourceLocation = sl;
        endCg.length = 0;
        computeNextCharPosition(ch, endCg.sourceLocation.line, endCg.sourceLocation.column);
        this->charGroups.push_back(endCg);
      } else {
        // Replace the last character group.
        CharacterGroup * newCg = &this->charGroups.at(this->charGroups.size()-2);
        CharacterGroup * endCg = &this->charGroups.at(this->charGroups.size()-1);
        endCg->sourceLocation = newCg->sourceLocation = sl;
        computeNextCharPosition(ch, endCg->sourceLocation.line, endCg->sourceLocation.column);
      }
    } else {
      // The operation failed.
      return false;
    }
  } else {
    // Push the new character into the characters buffer.
    this->charBuffer.push_back(ch);

    // Do we have any defined char groups?
    if (this->charGroups.size() == 0) {
      // In this case this should be the first character in the buffer
      ASSERT(this->charBuffer.size() == 1);
      // Define a new char group.
      CharacterGroup cg;
      cg.sourceLocation = sl;
      cg.length = 1;
      this->charGroups.push_back(cg);
      // Define the ending character group, which only contains the expected
      // position of the next character.
      computeNextCharPosition(ch, cg.sourceLocation.line, cg.sourceLocation.column);
      cg.length = 0;
      this->charGroups.push_back(cg);
    } else {
      // The size of the character groups array should not be less than 2.
      ASSERT(this->charGroups.size() >= 2);
      // check if the character's position is equivalent to the expected
      // next position.
      CharacterGroup * lastCg = &this->charGroups.at(this->charGroups.size()-1);
      if (sl == lastCg->sourceLocation) {
        // We don't need to create a new group because the current character
        // fits in the last group.
        this->charGroups.at(this->charGroups.size()-2).length++;
        // Update the ending character group to have the new expected
        // position of the next character.
        computeNextCharPosition(ch, lastCg->sourceLocation.line, lastCg->sourceLocation.column);
      } else {
        // Create a new group to contain the current character.
        lastCg->sourceLocation = sl;
        lastCg->length = 1;
        // Define the ending character group, which only contains the expected
        // position of the next character.
        CharacterGroup cg;
        cg.sourceLocation = sl;
        cg.length = 0;
        computeNextCharPosition(ch, cg.sourceLocation.line, cg.sourceLocation.column);
        this->charGroups.push_back(cg);
      }
    }
  }

  return true;
}


/**
 * Remove a given number of characters from the beginning of the buffer. The
 * operation also handles the required changes in the character groups array.
 *
 * @param count The count of characters to remove from the buffer.
 */
void InputBuffer::remove(Int count)
{
  // Are we removing all the characters in the buffer?
  if (count >= static_cast<Int>(this->charBuffer.size())) {
    // Wipe the buffer completely.
    this->charBuffer.clear();
    this->charGroups.clear();
  } else {
    // Count the number of character groups to be removed and the
    // number of characters to remove from the following group.
    Int cc = count;
    Int gc = 0;
    while (this->charGroups.at(gc).length <= cc) {
      // There should never be a situation in which cc is 0 at this point.
      ASSERT(cc > 0);
      // Update the number of remaining characters after removing this
      // group.
      cc -= this->charGroups.at(gc).length;
      gc++;
    }

    // Remove the marked character groups.
    if (gc > 0) {
      this->charGroups.erase(this->charGroups.begin(), this->charGroups.begin()+(gc-1));
    }

    // There should be at least two entires in the charGroups array at this
    // point.
    ASSERT(this->charGroups.size() >= 2);

    // Remove the marked characters from the following group.
    CharacterGroup * cg = &this->charGroups.at(0);
    Int i = count - cc;
    while (cc > 0) {
      computeNextCharPosition(this->charBuffer.at(i), cg->sourceLocation.line, cg->sourceLocation.column);
      cg->length--;
      i++;
      cc--;
    }

    // Remove the characters from the characters buffer.
    this->charBuffer.erase(0, count);
  }
}


/**
 * Check whether the buffer is full or not. The buffer will be full when either
 * the number of characters in the buffer or the number of character groups
 * exceed the maximum allowed for each.
 *
 * @return Returns true if the buffer is full, false otherwise.
 */
Bool InputBuffer::isFull()
{
  // NOTE: Because forcing a new character into a full buffer can cause a new
  //       character group to be created, we leave a space for one more char
  //       group in our calculations.
  if (this->charBuffer.size() >= INPUT_BUFFER_MAX_CHARACTERS ||
      this->charGroups.size() >= INPUT_BUFFER_MAX_GROUPS-1) {
    return true;
  } else {
    return false;
  }
}

} } // namespace
