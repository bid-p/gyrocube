#pragma once
#include <vector>

#include "tap/architecture/clock.hpp"
#include "tap/control/subsystem.hpp"
#include "utils/common_types.hpp"
#include "utils/robot_constants.hpp"

//#ifndef TARGET_ENGINEER
namespace src::Shooter {

enum MotorIndex {
    TOP = 0,
    BOT = 1,
    RIGHT = 0,
    LEFT = 1,
    TOP_RIGHT = 0,
    BOT_RIGHT = 1,
    TOP_LEFT = 2,
    BOT_LEFT = 3,
};

class ShooterSubsystem : public tap::control::Subsystem {
   public:
    ShooterSubsystem(tap::Drivers* drivers);

    // Allows user to call a DJIMotor member function on all shooter motors
    template <class... Args>
    void ForAllShooterMotors(void (DJIMotor::*func)(Args...), Args... args) {
        for (auto i = 0; i < SHOOTER_MOTOR_COUNT; i++) {
            (motors[i][1]->*func)(args...);
        }
    }

    template <class... Args>
    void ForAllShooterMotors(void (ShooterSubsystem::*func)(MotorIndex, Args...), Args... args) {
        for (auto i = 0; i < SHOOTER_MOTOR_COUNT; i++) {
            MotorIndex mi = static_cast<MotorIndex>(i);
            (this->*func)(mi, args...);
        }
    }

    mockable void initialize() override;
    void refresh() override;

    /**
     * @brief Updates velocity PID and motor RPM for a single motor. Intended for use with ForAllShooterMotors().
     * Should be called continuously in subsystem refresh
     * 
     * @param motorIdx index for DJIMotor matrix
     */
    void updateMotorVelocityPID(MotorIndex motorIdx);

    /**
     * @brief Changes the target RPM for a single motor. Intended for use with ForAllShooterMotors(), 
     * and should be called from a command to declare intended RPM. Does not necessarily need to be called continuously
     * 
     * @param motorIdx index for DJIMotor matrix
     * @param targetRPM intended target RPM
     */
    void setTargetRPM(MotorIndex motorIdx, float targetRPM);

#ifndef ENV_UNIT_TESTS
   private:
#else
   public:
#endif

    DJIMotor flywheel1, flywheel2;
    SmoothPID flywheel1PID, flywheel2PID;

#ifdef TARGET_SENTRY
    DJIMotor flywheel3, flywheel4;
    SmoothPID flywheel3PID, flywheel4PID;
#endif

    static constexpr CANBus SHOOTER_BUS = CANBus::CAN_BUS1;

    Matrix<float, SHOOTER_MOTOR_COUNT, 1> targetRPMs;
    Matrix<DJIMotor*, SHOOTER_MOTOR_COUNT, 1> motors;

    Matrix<SmoothPID*, SHOOTER_MOTOR_COUNT, 1> velocityPIDs;

    uint32_t lastTime;
};
};  // namespace src::Shooter
