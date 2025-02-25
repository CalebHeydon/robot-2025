#include "Robot.h"

#include <frc/DriverStation.h>
#include <frc/Timer.h>
#include <frc/geometry/Pose2d.h>
#include <frc/smartdashboard/SmartDashboard.h>
#include <memory>

#include "Constants.h"
#include "Controllers.h"
#include "Locations.h"
#include "Util.h"
#include "auto/AutoCrossLine.h"
#include "auto/AutoDoNothing.h"
#include "systems/Cameras.h"
#include "systems/Elevator.h"
#include "systems/Feeder.h"
#include "systems/Manipulator.h"
#include "systems/SwerveDrive.h"

// This gets called first. So, initialize everything here.
Robot::Robot() {
  m_startChooser.SetDefaultOption("1", 1);
  m_startChooser.AddOption("2", 2);
  m_startChooser.AddOption("3", 3);
  frc::SmartDashboard::PutData("Start Location", &m_startChooser);

  m_autoChooser.SetDefaultOption("Do Nothing", "DoNothing");
  m_autoChooser.AddOption("Cross the Line", "CrossLine");
  m_autoChooser.AddOption("One Coral", "OneCoral");
  frc::SmartDashboard::PutData("Auto", &m_autoChooser);

  // Call GetInstance() so the constructors get called
  Cameras::GetInstance();
  SwerveDrive::GetInstance();
  Elevator::GetInstance();
  Feeder::GetInstance();
  Manipulator::GetInstance();

  // Start the compressor
  m_compressor.EnableAnalog(
      units::pounds_per_square_inch_t{Constants::kMinPressure},
      units::pounds_per_square_inch_t{Constants::kMaxPressure});

  // This initializes the main looper. What you put here will run @200 Hz while
  // the robot is on.
  m_looper = Looper{[this] {
    Mode mode = kDisabled;
    if (IsEnabled()) {
      if (IsAutonomous()) {
        mode = kAuto;
      } else if (IsTeleop()) {
        mode = kTeleop;
      }
    }

    double t = frc::Timer::GetFPGATimestamp().value();

    if (mode == kAuto) {
      if (m_auto) {
        m_auto->Update(t);
      }
    } else if (mode == kTeleop) {
      // Get the inputs from the controller during teleop mode. Note this uses
      // the split setup where the left joystick controls velocity, and the
      // right joystick controls the rotation. The Util::exp() function squares
      // the input while keeping the sign.
      double leftY =
          Controllers::GetInstance().GetDriverController().GetLeftY();
      double vx = Util::Exp(-leftY) * Constants::kDriveControlMultipler;

      double leftX =
          Controllers::GetInstance().GetDriverController().GetLeftX();
      double vy = Util::Exp(-leftX) * Constants::kDriveControlMultipler;
      // GetRightX() doesn't work in the Linux simulation for some reason
      double rightX =
          Controllers::GetInstance().GetDriverController().GetRightX();
      double w = Util::Exp(-rightX) * Constants::kDriveAngularControlMultiplier;

      // The auto will reset the pose to be facing towards the driver on the red
      // alliance so it needs to be corrected
      auto alliance = frc::DriverStation::GetAlliance();
      if (alliance.has_value() &&
          alliance.value() == frc::DriverStation::Alliance::kRed) {
        vx *= -1;
        vy *= -1;
      }

      if (Controllers::GetInstance().GetDriverController().GetRawButton(9) &&
          Controllers::GetInstance().GetDriverController().GetRawButton(10)) {
        if (leftX >= 0.5 && rightX <= -0.5) {
          if (alliance.has_value() &&
              alliance.value() == frc::DriverStation::Alliance::kRed) {
            SwerveDrive::GetInstance().ResetPose(frc::Pose2d{
                frc::Translation2d{}, frc::Rotation2d{units::radian_t{M_PI}}});
          } else {
            SwerveDrive::GetInstance().ResetPose(frc::Pose2d{});
          }
        }

        SwerveDrive::GetInstance().DriveVelocity(0, 0, 0);
      } else {
        SwerveDrive::GetInstance().DriveVelocity(vx, vy, w);
      }

      if (Controllers::GetInstance()
              .GetDriverController()
              .GetAButtonPressed()) {
        // Coral scoring location
      } else if (Controllers::GetInstance()
                     .GetDriverController()
                     .GetBButtonPressed()) {
        // Algae scoring location
      }

      if (Controllers::GetInstance()
              .GetOperatorController()
              .GetAButtonPressed()) {
        // L1
        Elevator::GetInstance().SetPosition(Elevator::Position::kL1);
      } else if (Controllers::GetInstance()
                     .GetOperatorController()
                     .GetBButtonPressed()) {
        // L2
        Elevator::GetInstance().SetPosition(Elevator::Position::kL2);
      } else if (Controllers::GetInstance()
                     .GetOperatorController()
                     .GetXButtonPressed()) {
        // L3
        Elevator::GetInstance().SetPosition(Elevator::Position::kL3);
      } else if (Controllers::GetInstance()
                     .GetOperatorController()
                     .GetYButtonPressed()) {
        // L4
        Elevator::GetInstance().SetPosition(Elevator::Position::kL4);
      } else if (Controllers::GetInstance().GetOperatorController().GetPOV() ==
                 180) {
        // L1 Algae
        Elevator::GetInstance().SetPosition(Elevator::Position::kAlgae1);
      } else if (Controllers::GetInstance().GetOperatorController().GetPOV() ==
                 0) {
        // L2 Algae
        Elevator::GetInstance().SetPosition(Elevator::Position::kAlgae2);
      } else if (Controllers::GetInstance().GetOperatorController().GetPOV() ==
                 270) {
        // Algae Auto
        auto algaePosition = Cameras::GetInstance().GetAlgaeState();
        if (algaePosition == Cameras::kBottom ||
            algaePosition == Cameras::kBoth) {
          Elevator::GetInstance().SetPosition(Elevator::Position::kAlgae1);
        } else if (algaePosition == Cameras::kTop) {
          Elevator::GetInstance().SetPosition(Elevator::Position::kAlgae2);
        }
      } else if (Controllers::GetInstance()
                     .GetOperatorController()
                     .GetLeftBumperButtonPressed()) {
        // Algae Arm Down
        Manipulator::GetInstance().ExtendArm();
      } else if (Controllers::GetInstance()
                     .GetOperatorController()
                     .GetRightBumperButtonPressed()) {
        // Algae Arm Up
        Manipulator::GetInstance().RetractArm();
      } else if (Controllers::GetInstance()
                     .GetOperatorController()
                     .GetLeftTriggerAxis() > 0.5) {
        // Algae In
        Manipulator::GetInstance().SetAlgaeSpeed(
            Constants::kManipulatorAlgaeManipulatorSpeed);
      } else if (Controllers::GetInstance()
                     .GetOperatorController()
                     .GetRightTriggerAxis() > 0.5) {
        // Algae Out
        Manipulator::GetInstance().SetAlgaeSpeed(
            Constants::kManipulatorAlgaeOuttakeSpeed);
      } else if (Controllers::GetInstance()
                     .GetOperatorController()
                     .GetRightX() > 0.5) {
        Manipulator::GetInstance().StartIntakingCoral();
      } else {
        Manipulator::GetInstance().SetAlgaeSpeed(0);
        Manipulator::GetInstance().StopIntakingcoral();
      }
    }

    Cameras::GetInstance().Update(mode, t);
    SwerveDrive::GetInstance().Update(mode, t);
    Elevator::GetInstance().Update(mode, t);
    Feeder::GetInstance().Update(mode, t);
    Manipulator::GetInstance().Update(mode, t);
  }};
}

// This destructor gets called when the robot program shuts down.
// Cleanup any resources (especially files) before the robot code gets
// restarted.
Robot::~Robot() {}

void Robot::DisabledExit() {
  auto alliance = frc::DriverStation::GetAlliance();
  if (alliance.has_value()) {
    Locations::GetInstance().Generate(alliance.value());

    frc::Pose2d start = Locations::GetInstance().GetStartPosition(
        alliance.value(), m_startChooser.GetSelected());
    SwerveDrive::GetInstance().ResetPose(start);

    std::string autoName = m_autoChooser.GetSelected();
    double t = frc::Timer::GetFPGATimestamp().value();

    if (autoName == "DoNothing") {
      m_auto = std::make_shared<AutoDoNothing>();
      m_auto->Start(t);
    } else if (autoName == "CrossLine") {
      m_auto = std::make_shared<AutoCrossLine>(alliance.value(), start);
      m_auto->Start(t);
    } else if (autoName == "OneCoral") {
      m_auto = std::make_shared<AutoDoNothing>();
      m_auto->Start(t);
    }
  }
}

#ifndef RUNNING_FRC_TESTS
int main(int argc, char **argv) { frc::StartRobot<Robot>(); }
#endif
