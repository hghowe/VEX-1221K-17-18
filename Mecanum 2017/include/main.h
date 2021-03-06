/** @file main.h
 * @brief Header file for global functions
 *
 * Any experienced C or C++ programmer knows the importance of header files. For those who
 * do not, a header file allows multiple files to reference functions in other files without
 * necessarily having to see the code (and therefore causing a multiple definition). To make
 * a function in "opcontrol.c", "auto.c", "main.c", or any other C file visible to the core
 * implementation files, prototype it here.
 *
 * This file is included by default in the predefined stubs in each VEX Cortex PROS Project.
 *
 * Copyright (c) 2011-2016, Purdue University ACM SIGBots.
 * All rights reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * PROS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#ifndef MAIN_H_

// This prevents multiple inclusion, which isn't bad for this file but is good practice
#define MAIN_H_
#define PORT_MOTOR_BACK_LEFT 8
#define PORT_MOTOR_BACK_RIGHT 6
#define PORT_MOTOR_FRONT_LEFT 9
#define PORT_MOTOR_FRONT_RIGHT 7

#define PORT_ORIENTATION_NORMAL 1
#define PORT_ORIENTATION_REVERSED -1

#define PORT_ORIENTATION_1 PORT_ORIENTATION_REVERSED
#define PORT_ORIENTATION_2 PORT_ORIENTATION_NORMAL
#define PORT_ORIENTATION_3 PORT_ORIENTATION_REVERSED
#define PORT_ORIENTATION_4 PORT_ORIENTATION_NORMAL
#define PORT_ORIENTATION_5 PORT_ORIENTATION_REVERSED
#define PORT_ORIENTATION_6 PORT_ORIENTATION_REVERSED
#define PORT_ORIENTATION_7 PORT_ORIENTATION_REVERSED
#define PORT_ORIENTATION_8 PORT_ORIENTATION_NORMAL
#define PORT_ORIENTATION_9 PORT_ORIENTATION_NORMAL
#define PORT_ORIENTATION_10 PORT_ORIENTATION_NORMAL


#include <API.h>
// Allow usage of this file in C++ programs
#ifdef __cplusplus
extern "C" {
#endif

// A function prototype looks exactly like its declaration, but with a semicolon instead of
// actual code. If a function does not match a prototype, compile errors will occur.

// Prototypes for initialization, operator control and autonomous

/**
 * Runs the user autonomous code. This function will be started in its own task with the default
 * priority and stack size whenever the robot is enabled via the Field Management System or the
 * VEX Competition Switch in the autonomous mode. If the robot is disabled or communications is
 * lost, the autonomous task will be stopped by the kernel. Re-enabling the robot will restart
 * the task, not re-start it from where it left off.
 *
 * Code running in the autonomous task cannot access information from the VEX Joystick. However,
 * the autonomous function can be invoked from another task if a VEX Competition Switch is not
 * available, and it can access joystick information if called in this way.
 *
 * The autonomous task may exit, unlike operatorControl() which should never exit. If it does
 * so, the robot will await a switch to another mode or disable/enable cycle.
 */
void autonomous();
/**
 * Runs pre-initialization code. This function will be started in kernel mode one time while the
 * VEX Cortex is starting up. As the scheduler is still paused, most API functions will fail.
 *
 * The purpose of this function is solely to set the default pin modes (pinMode()) and port
 * states (digitalWrite()) of limit switches, push buttons, and solenoids. It can also safely
 * configure a UART port (usartOpen()) but cannot set up an LCD (lcdInit()).
 */
void initializeIO();
/**
 * Runs user initialization code. This function will be started in its own task with the default
 * priority and stack size once when the robot is starting up. It is possible that the VEXnet
 * communication link may not be fully established at this time, so reading from the VEX
 * Joystick may fail.
 *
 * This function should initialize most sensors (gyro, encoders, ultrasonics), LCDs, global
 * variables, and IMEs.
 *
 * This function must exit relatively promptly, or the operatorControl() and autonomous() tasks
 * will not start. An autonomous mode selection menu like the pre_auton() in other environments
 * can be implemented in this task if desired.
 */
void initialize();
/**
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
void operatorControl();

// --------------------------   Common motor functions to be implemented in SharedMotorControl
/**
 *  turns on the given motor at the current power level - just like motorSet, but incorporates
 *  MOTOR_DIRECTION so we can assume positive is always forward.
 */
void K_setMotor(int whichPort, int power);

/*
 * determines the current setting for thie given motor, -128 <-> + 128. Based on the
 * DIRECTION_MODIFIERS, so it is compatible with K_setMotor.
 */
int K_getMotor(int whichPort);

/**
 * sets the motor to power level 1 or -1, so the motor is free to rotate (as opposed to 0,
 * which puts on the brakes). If the motor is already set to zero, it stays at zero.
 */
void K_floatMotor(int whichPort);

/**
 * restricts the motor's power to be within -127 to +127, just in case we are
 trying to apply power out of that range. Also latches power settings that are
 close to 0 to be zero, so we don't have fine drift.
 */
int normalizeMotorPower(int power);

// ---------------------------  Methods in opcontrol.c
/**
 *  read the sensors, both on the driver's/drivers' controller(s), and any
 *  on the robot, itself. Update variables that can be read by other methods.
 */
void checkSensors();

/**
 * Refresh what is shown on the LCD screen.
 */
void updateScreen();

/**
* performs any automatic functions on variables (e.g. lift to a certain height)
*/
void autoProcesses();


/**
 * Based on the variables in this program about desired behavior,
 decide what to do with the motors to try to make this behavior happen.
 */
void processMotors();

/**
* turns on the motors in the drive chain to make the wheels move the robot
* in the direction given.
* x_motion - the left/right "drift" of the robot (-127, 127)
* y_motion - the forward/backward "drive" of the robot (-127, 127)
* angle_motion - the rotational "twist" of the robot (-127, 127)
*/
void manageDriveMotors(int x_motion, int y_motion, int angle_motion);

// -------------------------  Methods in Autonomous --------------------------
/*
* tell all the drive motors to stop.
*/
void allStop();

/*
* turn on the motors full reverse.
*/
void backFull();

/*
*  based on the state of the global variables, update the motors.
*/
void auton_process_motors();






// End C++ export structure
#ifdef __cplusplus
}
#endif

#endif
