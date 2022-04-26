/*****************************************************
 *  RISC-V Stack-frame Manager (part of the machine description).
 *
 *
 */

#pragma once
#include <set>
#include "tac.hpp"
// #include "define.hpp"

// namespace mind {
// namespace assembly {

/**
 * RISC-V Stack-frame Manager.
 *
 * The main task of this manager is to use the stack-frame
 * efficiently (i.e. reuse stack-frame space as much as possible).
 *
 * NOTE: this manager assumes that every temporary variable is 32-bit.
 */
class RiscvStackFrameManager {
  public:
    // constructor
    RiscvStackFrameManager(int start);
    // resets the temporary variable area
    void reset(void);
    // reserves a variable in the local variable area
    void reserve(Temp v);
    // gets a slot to spill some register (i.e. to save some temporary variable)
    int getSlotToWrite(Temp v, std::set<Temp> *liveness);
    // gets the size of the stack frame
    int getStackFrameSize(void);

  private:
    int reserved_size; // reserved area size
    int size;          // current stackframe size
    int max_size;      // maximum stackframe size
    int start_offset;  // start offset
    int capacity;      // how many slots
    Temp *slots;  // dynamic slots

    // computes the offset of a specified slot
    int offsetOf(int slot_num);
    // ensures the capacity of the underlying buffer
    void ensureCapacity(void);
    // finds a slot with the given variable as its content
    int findSlotOf(Temp v);
};

// } // namespace assembly
// } // namespace mind

