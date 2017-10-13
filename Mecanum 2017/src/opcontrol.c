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
 long int startTime;
 long int timeSinceStart;

 void operatorControl()
 {
	 startTime = millis();

	 while (1)
	 {
		  timeSinceStart = millis()-startTime;
	 		checkSensors();
      autoProcesses();
	 		processMotors();
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
 	int RF_motor_power = normalizeMotorPower(y_input - x_input - angle_input);
 	int RB_motor_power = normalizeMotorPower(y_input + x_input - angle_input);
 	int LF_motor_power = normalizeMotorPower(y_input + x_input + angle_input);
 	int LB_motor_power = normalizeMotorPower(y_input - x_input + angle_input);

 	K_setMotor(PORT_MOTOR_FRONT_LEFT,LF_motor_power);
 	K_setMotor(PORT_MOTOR_BACK_LEFT,LB_motor_power);
 	K_setMotor(PORT_MOTOR_FRONT_RIGHT,RF_motor_power);
 	K_setMotor(PORT_MOTOR_BACK_RIGHT,RB_motor_power);

 }

 /**
  * restricts the motor's power to be within -127 to +127, just in case we are
  trying to apply power out of that range. Also latches power settings that are
  close to 0 to be zero, so we don't have fine drift.
  */
 int normalizeMotorPower(int power)
 {
 	if (power>127)
 		return 127;
 	if (power<-127)
 		return -127;
 	if (power<10 && power>-10)
 		return 0;
 	return power;

 }
