/** @file auto.c
 * @brief File for autonomous code
 *
 * This file should contain the user autonomous() function and any functions related to it.
 *
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
 *
 * PROS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#include "main.h"
/*
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
 void moveForward(int power, int timeInterval)
 {
   motorSet(1, power);
   motorSet(10, -power);
   delay(timeInterval);
 }
 void moveForwardUntil(int power)
 {
   motorSet(1, power);
   motorSet(10, -power);
   while (ultrasonicGet(sonar) > 70)
   {
     delay(1);
     if (!isAutonomous())
      break;
   }
     motorSet(1, 0);
     motorSet(10, 0);

 }
 void stopall()
 {
   motorSet(1, 0);
   motorSet(10, 0);
   motorSet(6, 0);
   motorSet(7, 0);
 }
 void stopMotors()
 {
   motorSet(1, 0);
   motorSet(10, 0);
 }

void autonomous()
{
  // moveForward(127, 1000);
  // motorSet(7,-127);
  // delay(700);
  // stopall();
  // delay(2500);
  // motorSet(6,110);
  // delay(750);
  // motorSet(6,0);
  // moveForward(-127,100);
  // stopall();
  // delay(500);
  delay(200);
  moveForwardUntil(127);
}

// printf("Distance is: %d\n",ultrasonicGet(sonar));
// delay(200);
