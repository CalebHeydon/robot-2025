#pragma once

#include <cameraserver/CameraServer.h>
#include <frc/Compressor.h>
#include <frc/TimedRobot.h>
#include <frc/geometry/Pose2d.h>
#include <frc/smartdashboard/SendableChooser.h>
#include <memory>

#include "Constants.h"
#include "Looper.h"
#include "auto/Task.h"
#include "control/PIDController.h"

class Robot : public frc::TimedRobot {
public:
  // Enum to specify the robot's mode to the sub-systems
  enum Mode { kDisabled, kAuto, kTeleop };

  Robot();
  ~Robot();

  void DisabledInit() override;
  void DisabledExit() override;
  void TeleopInit() override;

private:
  frc::Compressor m_compressor{frc::PneumaticsModuleType::REVPH};

  Looper m_looper;

  frc::SendableChooser<int> m_startChooser;
  frc::SendableChooser<std::string> m_autoChooser;

  std::shared_ptr<Task> m_auto;

  bool m_autoAligning = false;
  frc::Pose2d m_autoAlignSetpoint;
  int m_autoAlignSetpointIndex = 0;
  PIDController m_alignControllers[3]{
      {Constants::kPathFollowingKp, Constants::kPathFollowingKi,
       Constants::kPathFollowingKd},
      {Constants::kPathFollowingKp, Constants::kPathFollowingKi,
       Constants::kPathFollowingKd},
      {Constants::kPathFollowingAngleKp, Constants::kPathFollowingAngleKi,
       Constants::kPathFollowingAngleKd}};

  bool m_braking = false;

  inline void ResetAlignControllers() {
    for (int i = 0; i < 3; i++) {
      m_alignControllers[i].Reset();
    }
  }

  frc::Pose2d NearestLeftCoral(frc::Pose2d robotPose, int *i = nullptr);
  frc::Pose2d NearestRightCoral(frc::Pose2d robotPose, int *i = nullptr);
  frc::Pose2d NearestAlgae(frc::Pose2d robotPose, int *i = nullptr);
  frc::Pose2d NearestFeeder(frc::Pose2d robotPose, int *i = nullptr);
};
