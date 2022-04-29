#pragma once

#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/processing/resumable.hpp>

namespace utils {

class Ist8310 : modm::I2cDevice<modm::I2cMaster, 4> {
   public:
    Ist8310();

    void initialize();
    void update();
    
    inline float getX() { return x; };
    inline float getY() { return y; };
    inline float getZ() { return z; };

   private:
    inline modm::ResumableResult<bool> readRegister(Ist8310Data::Register reg, size_t length = 1) {
        RF_BEGIN();

        buffer[0] = uint8_t(reg);
        this->transaction.configureWriteRead(buffer, 1, buffer, length);

        RF_END_RETURN_CALL(this->runTransaction());
    }

    inline modm::ResumableResult<bool> writeToRegister(Ist8310Data::Register reg, uint8_t* output, size_t length = 1) {
        RF_BEGIN();

        buffer[0] = uint8_t(reg);
        this->transaction.configureWriteRead(buffer, 1, output, length);

        RF_END_RETURN_CALL(this->runTransaction());
    }

    inline uint8_t readDeviceID() {
        readRegister(Ist8310Data::Register::WHO_AM_I);
        return buffer[0];
    }

    inline bool tryReadAxisData() {
        readRegister(Ist8310Data::Register::STATUS_1);
        // Don't continue if the data-ready bit isn't set
        if(!(buffer[0] & uint8_t(Ist8310Data::RegisterData::STAT1_DATA_READY_MASK))) { return false; }

        readRegister(Ist8310Data::Register::X_DATA_LOW, 6);
    }

    inline void setInterruptFlag(bool value) {
        readRegister(Ist8310Data::Register::CONTROL_2);

        // There are some "reserved" fields in this register, so
        // We need to preserve whatever is in those bits. Which is
        // why we bitwise and the register with the clear mask.
        uint8_t output = buffer[0] | (((value) ? Ist8310Data::RegisterData::INTERRUPT_BIT : 0));
        writeToRegister(Ist8310Data::Register::CONTROL_2, &output);
    }

    inline void setDefaultPulseDuration() {
        readRegister(Ist8310Data::Register::PULSE_DURATION_CONTROL);

        // There are some "reserved" fields in this register, so
        // We need to preserve whatever is in those bits. Which is
        // why we bitwise and the register with the clear mask.
        uint8_t output = (buffer[0] & Ist8310Data::RegisterData::PDCTRL_CLEAR_MASK) | Ist8310Data::RegisterData::NORMAL_PULSE_DURATION;
        writeToRegister(Ist8310Data::Register::PULSE_DURATION_CONTROL, &output);
    }

    float x;
    float y;
    float z;

    bool isDeviceVerified;
    uint8_t buffer[6];
};

}