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
 int claw_input;
 int lift_input;
 int forearm_input;
 int low_lift_input;
 int lift_pot_value;
 int forearm_pot_value;
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
	 		processMotors(); // convert the variables to motor commands
	 		updateScreen();
	 		delay(10);
	 	}
 }

 /**
  *  read the sensors, both on the driver's/drivers' controller(s), and any
  *  on the robot, itself. Update variables that can be read by other methods.
  */
 void checkSensors()
 {
 	// read the joysticks - they control the motors.
 // 	x_input = joystickGetAnalog(1,1);
 	y_input = joystickGetAnalog(1,2);
 	angle_input = joystickGetAnalog(1,1);
  lift_input = joystickGetAnalog(2,2);
  forearm_input = joystickGetAnalog(1,3);
  low_lift_input = joystickGetAnalog(2,3);
  lift_pot_value = analogRead(LIFT_POTENTIOMETER);
  forearm_pot_value = analogRead(FOREARM_POTENTIOMETER);
if (joystickGetDigital(1, 6, JOY_UP))
{
  claw_input = 30;
}
if (joystickGetDigital(1, 6, JOY_DOWN))
{
  claw_input = -127;
}
// LIFT_SAFETY SECTION
//1710-20 Max height
//3180-90 Min height
/*
if (lift_input>0 && lift_pot_value > 3190)//stops lift from going past its highest position
{
    lift_input = 0;
}

if  (lift_input<0 && lift_pot_value <  1720)//stops lift from going past its lowest position
{
    lift_input = 0;
}
*/
// FOREARM_SAFETY SECTION
if (forearm_input < 0 && forearm_pot_value > 3700)//stops forearm from going past its highest position
{
    forearm_input = 0;
}

if  (forearm_input > 0 && forearm_pot_value < 600)//stops forearm from going past its lowest position
{
    forearm_input = 0;
}




 }

 /**
  * Refresh what is shown on the LCD screen.
  */
 void updateScreen()
 {
   if (LCD_ACTIVE)
   {
     char topString[16];
     char bottomString[16];
     snprintf(topString, 16, "L:%d R:%d",encoderGet(leftEncoder),encoderGet(rightEncoder)); // combine string with a variable
     snprintf(bottomString, 16, "Lf:%d arm:%d",lift_pot_value,forearm_pot_value); // combine string with a variable
     lcdSetText(uart1, 1, topString);
     lcdSetText(uart1, 2, bottomString);
     	//lcdPrint(uart1, 1, "Go Falcons!"); //lcdPrint is ok if you don't have formatting.
      //printf("Encoders Are: %d, %d\n",encoderGet(leftEncoder), encoderGet(rightEncoder)); // this would print to serial port - to the computer via orange cable.
   }
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
  manageLiftMotors(lift_input);
  manageClawMotors(claw_input);
  manageForearmMotors(forearm_input);
  manageLowLiftMotors(low_lift_input);
 }
