/*
 * SharedMotorControl.c
 *
 *  Created on: Jun 10, 2016
 *      Author: harlan.howe
 */
#include "API.h"
#include "main.h"


//-----------------------------------------------------
// DO NOT MODIFY this section. Make any changes in Constants.h.
const int DIRECTION_MODIFIERS[] = {0,  // not actually on cortex. Do not use..
								   PORT_ORIENTATION_1,
								   PORT_ORIENTATION_2,
								   PORT_ORIENTATION_3,
								   PORT_ORIENTATION_4,
								   PORT_ORIENTATION_5,
								   PORT_ORIENTATION_6,
								   PORT_ORIENTATION_7,
								   PORT_ORIENTATION_8,
								   PORT_ORIENTATION_9,
								   PORT_ORIENTATION_10};
// ------------------------------------------------------

/**
 *  turns on the given motor at the current power level - just like motorSet, but incorporates
 *  DIRECTION_MODIFIERS so we can assume positive is always forward.
 */
void K_setMotor(int whichPort, int power)
{
	motorSet(whichPort, power*DIRECTION_MODIFIERS[whichPort]);
}

/*
 * determines the current setting for thie given motor, -128 <-> + 128. Based on the
 * DIRECTION_MODIFIERS, so it is compatible with K_setMotor.
 */
int K_getMotor(int whichPort)
{
	return motorGet(whichPort)*DIRECTION_MODIFIERS[whichPort];
}

/**
 * sets the motor to power level 1 or -1, so the motor is free to rotate (as opposed to 0,
 * which puts on the brakes). If the motor is already set to zero, it stays at zero.
 */
void K_floatMotor(int whichPort)
{
	int current_level = motorGet(whichPort);
	if (current_level == 0)
		return;
	if (current_level < 0)
		motorSet(whichPort,-1);
	else
		motorSet(whichPort, 1);
}

/**
 * stops the motor - puts the brakes on it. Probably not a good idea if you are at top speed
 * and wish to slow down.
 */
void K_stopMotor(int whichPort)
{
	motorSet(whichPort,0);
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

/**
* turns on the motors in the drive chain to make the wheels move the robot
* in the direction given.
* x_motion - the left/right "drift" of the robot (-127, 127)
* y_motion - the forward/backward "drive" of the robot (-127, 127)
* angle_motion - the rotational "twist" of the robot (-127, 127)
*/
void manageDriveMotors(int x_motion, int y_motion, int angle_motion)
{
	// first make sure that y_motion and angle_motion are within (-127,+127)
	y_motion = normalizeMotorPower(y_motion);
	angle_motion = normalizeMotorPower(angle_motion);

	// K_setMotor(PORT_MOTOR_FRONT_LEFT,y_motion+angle_motion/2);
	// K_setMotor(PORT_MOTOR_FRONT_RIGHT,y_motion-angle_motion/2);
	K_setMotor(PORT_MOTOR_BACK_LEFT,y_motion+angle_motion/2);
	K_setMotor(PORT_MOTOR_BACK_RIGHT,y_motion-angle_motion/2);


 // OLD MECANUM CODE
	// int RF_motor_power = normalizeMotorPower(y_motion + x_motion - angle_motion);
 // 	int RB_motor_power = normalizeMotorPower(y_motion - x_motion - angle_motion);
 // 	int LF_motor_power = normalizeMotorPower(y_motion - x_motion + angle_motion);
 // 	int LB_motor_power = normalizeMotorPower(y_motion + x_motion + angle_motion);
	//
 // 	K_setMotor(PORT_MOTOR_FRONT_LEFT,LF_motor_power);
 // 	K_setMotor(PORT_MOTOR_BACK_LEFT,LB_motor_power);
 // 	K_setMotor(PORT_MOTOR_FRONT_RIGHT,RF_motor_power);
 // 	K_setMotor(PORT_MOTOR_BACK_RIGHT,RB_motor_power);
}
/**
* turns on the lift motor to make the lift move in the direction given
* lift_motion - either up or down
*/
void manageLiftMotors(int lift_motion)
{
	lift_motion = normalizeMotorPower(lift_motion);

	K_setMotor(PORT_MOTOR_LIFT,lift_motion);
}

void manageLowLiftMotors(int low_lift_motion)
{
	low_lift_motion = normalizeMotorPower(low_lift_motion);

	K_setMotor(PORT_MOTOR_LOW_LIFT,low_lift_motion);
}

void manageClawMotors(int claw_motion)
{
	claw_motion = normalizeMotorPower(claw_motion);

	K_setMotor(PORT_MOTOR_CLAW,claw_motion);
}

void manageForearmMotors(int forearm_motion)
{
	forearm_motion = normalizeMotorPower(forearm_motion);

	K_setMotor(PORT_MOTOR_FOREARM,forearm_motion);
}
