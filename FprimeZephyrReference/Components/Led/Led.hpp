// ======================================================================
// \title  Led.hpp
// \author samyu
// \brief  hpp file for Led component implementation class
// ======================================================================

#ifndef Components_Led_HPP
#define Components_Led_HPP

#include "FprimeZephyrReference/Components/Led/LedComponentAc.hpp"

namespace Components {

class Led final : public LedComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct Led object
    Led(const char* const compName  //!< The component name
    );

    //! Destroy Led object
    ~Led();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for commands
    // ----------------------------------------------------------------------

    //! Handler implementation for parameter updates
    void parameterUpdated(FwPrmIdType id) override;

    //! Handler implementation for rate group ticks
    void run_handler(FwIndexType portNum, U32 context) override;

    //! Handler implementation for command BLINKING_ON_OFF
    //!
    //! Command to turn on or off the blinking LED
    void BLINKING_ON_OFF_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                    U32 cmdSeq,           //!< The command sequence number
                                    Fw::On onOff          //!< Indicates whether the blinking should be on or off
                                    ) override;

    //! Helper to drive the physical LED and track state transitions
    void driveLed(Fw::On state);
    
    Fw::On m_state = Fw::On::OFF; //! Keeps track if LED is on or off
    U64 m_transitions = 0; //! The number of on/off transitions that have occurred from FSW boot up
    U32 m_toggleCounter = 0; //! Keeps track of how many ticks the LED has been on for
    bool m_blinking = false; //! Flag: if true then LED blinking will occur else no blinking will happen
};

}  // namespace Components

#endif
