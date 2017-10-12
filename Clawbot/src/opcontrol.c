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


void operatorControl()
{
		bool clawOpen;
		bool clawClose;

		bool armUp;
		bool armDown;

		int power;
	  int turn;
		int clawPower;
		int armPower;
		while (1)
		{
					clawOpen = joystickGetDigital(1,5,JOY_UP);
					clawClose = joystickGetDigital(1,5,JOY_DOWN);

					armUp = joystickGetDigital(1,6,JOY_UP);
					armDown = joystickGetDigital(1,6,JOY_DOWN);

	        turn = (joystickGetAnalog(1, 4) + joystickGetAnalog(1, 1))/2; // vertical axis on right joystick
	        power = (joystickGetAnalog(1, 3) + joystickGetAnalog(1, 2))/2; // horizontal axis on right joystick

					clawPower = 0;
					if (clawOpen == 1)
					{
						clawPower = 110;
					}
					if (clawClose == 1)
					{
						clawPower = clawPower  -110;
					}

					armPower = 0;
					if (armDown == 1)
					{
						armPower = 110;
					}
					if (armUp == 1)
					{
						armPower = armPower  -110;
					}

					motorSet(6,clawPower);
					motorSet(7,armPower);

					motorSet(1, power + turn);
					motorSet(10, -1*(power-turn)); // the -1 is because the motor is reversed.

					// printf ("Test.");
					// printf("%d",digitalRead(BUTTON_PORT));

					if (ultrasonicGet(sonar) > 70)
					{
						digitalWrite(GREEN_LED_PIN, HIGH);
					}
					else
					{
						digitalWrite(GREEN_LED_PIN, LOW);
					}

					delay(20);
		}
}
