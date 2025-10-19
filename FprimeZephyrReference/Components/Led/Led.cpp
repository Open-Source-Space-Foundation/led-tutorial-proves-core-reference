// ======================================================================
// \title  Led.cpp
// \author samyu
// \brief  cpp file for Led component implementation class
// ======================================================================

#include "FprimeZephyrReference/Components/Led/Led.hpp"
#include <Fw/Types/Assert.hpp>

namespace Components {

namespace {
//! Clamp interval to at least one scheduler tick
constexpr U32 MIN_BLINK_INTERVAL = 1U;
}  // namespace

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

Led ::Led(const char* const compName) : LedComponentBase(compName) {}

Led ::~Led() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void Led ::parameterUpdated(FwPrmIdType id) {
    switch (id) {
        case Led::PARAMID_BLINK_INTERVAL: {
            Fw::ParamValid valid;
            const U32 interval = this->paramGet_BLINK_INTERVAL(valid);
            if ((valid == Fw::ParamValid::VALID) && (interval >= MIN_BLINK_INTERVAL)) {
                // Start a fresh timing cycle whenever the interval changes
                this->m_toggleCounter = 0;
                this->log_ACTIVITY_HI_BlinkIntervalSet(interval);
            }
        } break;
        default:
            FW_ASSERT(0);
            break;  // Fallthrough from assert (static analysis)
    }
}

void Led ::run_handler(FwIndexType portNum, U32 context) {
    FW_ASSERT(portNum == 0);
    static_cast<void>(context);

    if (!this->m_blinking) {
        if (this->m_state == Fw::On::ON) {
            // Ensure the LED is off when blinking is disabled
            this->driveLed(Fw::On::OFF);
        }
        this->m_toggleCounter = 0;
        return;
    }

    Fw::ParamValid valid;
    U32 interval = this->paramGet_BLINK_INTERVAL(valid);
    if ((valid != Fw::ParamValid::VALID) || (interval < MIN_BLINK_INTERVAL)) {
        interval = MIN_BLINK_INTERVAL;
    }

    this->m_toggleCounter++;
    if (this->m_toggleCounter >= interval) {
        this->m_toggleCounter = 0;
        const Fw::On nextState = (this->m_state == Fw::On::ON) ? Fw::On::OFF : Fw::On::ON;
        // Toggle the hardware when the interval elapses
        this->driveLed(nextState);
    }
}

void Led ::driveLed(Fw::On state) {
    if (this->isConnected_gpioSet_OutputPort(0)) {
        const Fw::Logic logicState = (state == Fw::On::ON) ? Fw::Logic::HIGH : Fw::Logic::LOW;
        // Mirror the desired state to the Zephyr GPIO driver
        this->gpioSet_out(0, logicState);
    }

    if (this->m_state != state) {
        ++this->m_transitions;
        this->log_ACTIVITY_LO_LedState(state);
    }

    this->m_state = state;
}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void Led ::BLINKING_ON_OFF_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, Fw::On onOff) {
    this->m_toggleCounter = 0;               // Reset count on any successful command
    const bool enableBlinking = (Fw::On::ON == onOff);

    if (!enableBlinking) {
        this->driveLed(Fw::On::OFF);
    } else if (!this->m_blinking) {
        this->driveLed(Fw::On::ON);
    }
    this->m_blinking = enableBlinking;

    // Emit an event to report the blinking state
    this->log_ACTIVITY_HI_SetBlinkingState(onOff);

    // Report the blinking state on telemetry channel
    this->tlmWrite_BlinkingState(onOff);

    // Provide command response
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

}  // namespace Components
