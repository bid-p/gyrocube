#include "gimbal_control_command.hpp"

#include <tap/architecture/clock.hpp>
#include <tap/communication/gpio/leds.hpp>

namespace src::Gimbal {

GimbalControlCommand::GimbalControlCommand(src::Drivers* drivers,
                                           GimbalSubsystem* gimbalSubsystem,
                                           GimbalChassisRelativeController* gimbalController,
                                           float inputYawSensitivity,
                                           float inputPitchSensitivity)
    : tap::control::Command(),
      drivers(drivers),
      gimbal(gimbalSubsystem),
      controller(gimbalController),
      userInputYawSensitivityFactor(inputYawSensitivity),
      userInputPitchSensitivityFactor(inputPitchSensitivity) {
    addSubsystemRequirement(dynamic_cast<tap::control::Subsystem*>(gimbal));
}

void GimbalControlCommand::initialize() {}

void GimbalControlCommand::execute() {
    // This just locks it to the the forward direction, specified by YAW_START_ANGLE
    controller->runYawController();

    float targetPitchAngle = gimbal->getTargetPitchAngle(AngleUnit::Degrees) -
                             (userInputPitchSensitivityFactor * drivers->remote.getChannel(tap::communication::serial::Remote::Channel::RIGHT_VERTICAL)) * getPitchMotorDirection();
    controller->runPitchController(AngleUnit::Degrees, targetPitchAngle);
}

bool GimbalControlCommand::isReady() { return true; }

bool GimbalControlCommand::isFinished() const { return false; }

void GimbalControlCommand::end(bool) {
    gimbal->setYawMotorOutput(0);
    gimbal->setPitchMotorOutput(0);
}

}  // namespace src::Gimbal