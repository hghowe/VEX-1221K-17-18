/** @file opcontrol.c
 * @brief File for operator control code
 *
 * This file should contain the user operatorControl() function and any functions related to it.
 *
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
 *
 * PROS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#include "main.h"

/*
 * Runs the user operator control code. This function will be started in its own task with the
 * default priority and stack size whenever the robot is enabled via the Field Management System
 * or the VEX Competition Switch in the operator control mode. If the robot is disabled or
 * communications is lost, the operator control task will be stopped by the kernel. Re-enabling
 * the robot will restart the task, not resume it from where it left off.
 *
 * If no VEX Competition Switch or Field Management system is plugged in, the VEX Cortex will
 * run the operator control task. Be warned that this will also occur if the VEX Cortex is
 * tethered directly to a computer via the USB A to A cable without any VEX Joystick attached.
 *
 * Code running in this task can take almost any action, as the VEX Joystick is available and
 * the scheduler is operational. However, proper use of delay() or taskDelayUntil() is highly
 * recommended to give other tasks (including system tasks such as updating LCDs) time to run.
 *
 * This task should never exit; it should end with some kind of infinite loop, even if empty.
 */
 int x_input, y_input, angle_input;
 int lift_input;
 int grabber_value;
 long int startTime;
 long int timeSinceStart;

 void operatorControl()
 {
	 startTime = millis();
   grabber_value = 0;
	 while (1)
	 {
		  timeSinceStart = millis()-startTime;
	 		checkSensors();
      autoProcesses();
	 		processMotors(); // convert the variables to motor commands
	 		updateScreen();
	 		delay(20);
	 	}
 }

 /**
  *  read the sensors, both on the driver's/drivers' controller(s), and any
  *  on the robot, itself. Update variables that can be read by other methods.
  */
 void checkSensors()
 {
 	// read the joysticks - they control the motors.
 	x_input = joystickGetAnalog(1,1);
 	y_input = joystickGetAnalog(1,2);
 	angle_input = joystickGetAnalog(1,4);

  if (digitalRead(JUMPER_TWO_DRIVERS)==LOW)
  {
    lift_input = joystickGetAnalog(2, 3);
    if (joystickGetAnalog(2,2)>32 || joystickGetAnalog(2,2) < -32)
      grabber_value += joystickGetAnalog(2,2)/8;
  }
  else
  {
    lift_input = 0;
    if (joystickGetDigital(1, 7, JOY_UP))
      lift_input += 100;
    if (joystickGetDigital(1, 7, JOY_DOWN))
      lift_input -= 100;

    if (joystickGetDigital(1, 6, JOY_UP))
      grabber_value += 10;
    if (joystickGetDigital(1, 6, JOY_DOWN))
      grabber_value -= 10;

  }
  if (grabber_value>127)
    grabber_value = 127;
  if (grabber_value < -127)
    grabber_value = -127;
 }

 /**
  * Refresh what is shown on the LCD screen.
  */
 void updateScreen()
 {
 	lcdPrint(uart1, 1, "Go Falcons!");
 }

 /**
 * performs any automatic functions on variables (e.g. lift to a certain height)
 */
 void autoProcesses()
 {
   ; // nothing, for now....
 }

/**
 * Based on the variables in this program about desired behavior,
 decide what to do with the motors to try to make this behavior happen.
 */
 void processMotors()
 {
  manageDriveMotors(x_input, y_input, angle_input);
  manageLiftMotor(lift_input);
  manageGrabberMotor(grabber_value);
 }
