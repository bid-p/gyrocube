#include "gimbal_control_command.hpp"

#include <tap/architecture/clock.hpp>
#include <tap/communication/gpio/leds.hpp>

namespace src::Gimbal {

static float rightVertical = 0.0f;
static float desiredOut = 0.0f;

static tap::gpio::Leds::LedPin pins[8] = {
    tap::gpio::Leds::A,
    tap::gpio::Leds::B,
    tap::gpio::Leds::C,
    tap::gpio::Leds::D,
    tap::gpio::Leds::E,
    tap::gpio::Leds::F,
    tap::gpio::Leds::G,
    tap::gpio::Leds::H};

GimbalControlCommand::GimbalControlCommand(src::Drivers* drivers,
                                           GimbalSubsystem* gimbalSubsystem,
                                           GimbalChassisRelativeController* gimbalController,
                                           float inputSensitivity)
    : tap::control::Command(),
      drivers(drivers),
      gimbal(gimbalSubsystem),
      controller(gimbalController),
      userInputSensitivityFactor(inputSensitivity),
      previousTime(0),
      ledIndex(0) {
    addSubsystemRequirement(dynamic_cast<tap::control::Subsystem*>(gimbal));
}

void GimbalControlCommand::initialize() {
    previousTime = tap::arch::clock::getTimeMilliseconds();
}

void GimbalControlCommand::execute() {
    uint32_t currentTime = tap::arch::clock::getTimeMilliseconds();
    uint32_t deltaTime = currentTime - previousTime;
    previousTime = currentTime;

    /* Don't worry about the complicated stuff until we can get simple movement
    float targetYawAngle = gimbal->getTargetYawAngleInRadians() +
                           userInputSensitivityFactor * drivers->remote.getChannel(tap::Remote::Channel::RIGHT_HORIZONTAL);

    controller->runYawController(deltaTime, targetYawAngle);
    // Flip the target angle every 2 seconds
    float targetYawAngle = 0.0f;
    if(currentTime % 2000 == 0) {
       targetYawAngle = (targetYawAngle == 0.0f) ? M_PI : 0.0f;
    }

    float targetPitchAngle = gimbal->getTargetPitchAngleInRadians() +
                             userInputSensitivityFactor * drivers->remote.getChannel(tap::Remote::Channel::RIGHT_VERTICAL);

    controller->runPitchController(deltaTime, targetPitchAngle);
    // Flip the target angle every 2 seconds
    float targetPitchAngle = 0.0f;
    if (currentTime % 2000 == 0) {
        targetPitchAngle = (targetPitchAngle == 0.0f) ? M_PI : 0.0f;
    }
    */

    rightVertical = drivers->remote.getChannel(tap::Remote::Channel::RIGHT_VERTICAL);
    // float rightHorizontal = drivers->remote.getChannel(tap::Remote::Channel::RIGHT_HORIZONTAL);
    desiredOut = 3000 * rightVertical;

    gimbal->setPitchMotorOutput(desiredOut);
    gimbal->setYawMotorOutput(desiredOut);
    // gimbal->setYawMotorOutput(200.0f * rightHorizontal);

    // Just a moving led so we know this code is running
    if (currentTime % 500 == 0) {
        drivers->leds.set(pins[ledIndex], true);
        if (ledIndex > 0)
            drivers->leds.set(pins[ledIndex - 1], false);
        else
            drivers->leds.set(pins[7], false);
        ledIndex = (ledIndex + 1) % 8;
    }
}

bool GimbalControlCommand::isReady() { return true; }

bool GimbalControlCommand::isFinished() const { return false; }

void GimbalControlCommand::end(bool) {
    gimbal->setYawMotorOutput(0);
    gimbal->setPitchMotorOutput(0);
}

}  // namespace src::Gimbal