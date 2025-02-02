#pragma once

#include <frc/geometry/Transform3d.h>
#include <units/length.h>

class Constants {
public:
  // Driverstation constants
  static constexpr int kDriverControllerId = 0;
  static constexpr int kOperatorControllerId = 1;

  // Constants for the drivetrain
  static constexpr int kPigeonCanId = 10;

  static constexpr auto kWheelBaseWidth = 0.5_m;

  static constexpr int kFlSteeringMotorId = 15;
  static constexpr int kFrSteeringMotorId = 16;
  static constexpr int kBlSteeringMotorId = 17;
  static constexpr int kBrSteeringMotorId = 18;
  static constexpr int kFlDriveMotorId = 11;
  static constexpr int kFrDriveMotorId = 12;
  static constexpr int kBlDriveMotorId = 13;
  static constexpr int kBrDriveMotorId = 14;

  static constexpr int kFlEncoderId = 15;
  static constexpr int kFrEncoderId = 16;
  static constexpr int kBlEncoderId = 17;
  static constexpr int kBrEncoderId = 18;

  // kS, kV, kP, kI, kD
  static constexpr std::tuple<double, double, double, double, double>
      kSteeringMotorGains{0.1, 0.1, 0.05, 0.0, 0.0};

  static constexpr double kWheelRadius = 0.7 / 2; // meters

  static constexpr double kDriveCurrentLimit = 60; // Amps
  static constexpr double kDriveRampRate = 0.25;   // Seconds
  static constexpr double kDriveVelocityMultiplier = 0.25;
  static constexpr double kDriveGearRatio = 22.0 / 72.0;

  static constexpr double kDefaultMaxV = 1.0;              // meters per second
  static constexpr double kDefaultMaxW = 1.0;              // radians per second
  static constexpr double kDefaultPositionTolerance = 0.1; // meters
  static constexpr double kDefaultAngleTolerance = 0.1;    // radians
  static constexpr double kPathFollowingKp = 0.1;
  static constexpr double kPathFollowingKi = 0;
  static constexpr double kPathFollowingKd = 0;
  static constexpr double kPathFollowingAngleKp = 0.1;
  static constexpr double kPathFollowingAngleKi = 0;
  static constexpr double kPathFollowingAngleKd = 0;

  // Vision
  static constexpr frc::Transform3d kFrontCameraTransform{
      frc::Translation3d{0_m, 0_m, 0_m}, frc::Rotation3d{0_rad, 0_rad, 0_rad}};
  static constexpr frc::Transform3d kBackCameraTransform{
      frc::Translation3d{0_m, 0_m, 0_m}, frc::Rotation3d{0_rad, 0_rad, 0_rad}};

  // TalonFX ids for elevator motors.
  static constexpr int kElevatorMainMotorId = 37;
  static constexpr int kElevatorSecondMotorId = 28;
  static constexpr double kElevatorMetersPerRotationE = 0.1715 / 254;

  // Value is in meters per second
  static constexpr double kElevatorMaxVelocity =
      0.1 / kElevatorMetersPerRotationE;
  // Value is in meters per second per second
  static constexpr double kElevatorAcceleration =
      0.1 / kElevatorMetersPerRotationE;
  // Value in meters per second per second per second
  static constexpr double kElevatorJerk = 0.1 / kElevatorMetersPerRotationE;
  // P I D CruiseVelocity Velocity S(overcome static friction) A(output per unit
  // of target acceleration)       Accel Jerk
  static constexpr std::tuple<double, double, double, double, double, double,
                              double, double, double>
      kElevatorMotorGains{0.1,
                          0,
                          0,
                          10,
                          kElevatorMaxVelocity,
                          0.01,
                          0.24,
                          kElevatorAcceleration,
                          kElevatorJerk};
  // Starting offset in meters
  static constexpr float kElevatorStartingPositionMeters = 0.01;
  static constexpr double kElevatorStartingPositionRotations =
      kElevatorStartingPositionMeters / kElevatorMetersPerRotationE;
  // Ending machine bounds in meters
  static constexpr float kElevatorFeedPositionMeters = 0.5;
  static constexpr double kElevatorFeedPositionRotations =
      kElevatorFeedPositionMeters / kElevatorMetersPerRotationE;
};
