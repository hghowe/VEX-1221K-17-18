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

// These are the two types of things stored in the timers array -
#define TRIGGER_TIME 0
#define IS_ACTIVE 1

// long-term variables..... these are about the desired motion of the chassis.
int auto_x_motion;
int auto_y_motion;
int auto_angle_motion;
int claw_input;
int lift_input;
int forearm_input;

// example for the blinking light.
bool LED_state;

// time-based variables
long timeSinceStart;
long startOfAuton;

// Initially, none of these actions will fire...
bool actionStatus[] = {false,  // 0. ahead full
                       false,  // 1. forearm up
                       false,  // 2. forearm down
                       false,  // 3. forearm stop
                       false,  // 4. claw open
                       false,  // 5. reverse
                       false}; // 6. stop driving
int timers[][2] = {{0,true},        //0. activates encoder drive
                   {250,true},      //1. trigger action 1
                   {500,true},     //2. trigger action 2
                   {1250,true},     //3. trigger action 3
                   {2750,true},     //4. trigger action 4
                   {3750,true},     //5. trigger action 5
                   {4500,true}};    //6. trigger action 6

int numTimers;
int numActions;
bool arrived;

void autonomous()
{
  startOfAuton = millis();

  // calculate num items in the arrays - the memory usage of the array
  //       divided by the memory usage of each item. <-- an old "C" trick.
  numTimers = sizeof(timers)/sizeof(timers[0]);
  numActions = sizeof(actionStatus)/sizeof(actionStatus[0]);

  while(true)
  {
    timeSinceStart = millis() - startOfAuton;

    // loop through all the timers....
    for (int i = 0; i<numTimers; i++)
    {
       // Should this particular timer be triggered?
       if ((timers[i][IS_ACTIVE] == true) && (timers[i][TRIGGER_TIME] <= timeSinceStart))
       {
         switch (i) // decide which "case" to do based on the value of "i"
         {
           case 0:
              actionStatus[0] = true;
              timers[0][IS_ACTIVE]= false;   // a one-time trigger.
            break;
           case 1:
              actionStatus[1] = true;
              timers[1][IS_ACTIVE] = false;   // a one-time trigger.
           break;
           case 2:
              actionStatus[2] = true;
              timers[2][IS_ACTIVE] = false;
           break;
           case 3:
              actionStatus[3] = true;
              timers[3][IS_ACTIVE] = false;
           break;
           case 4:
              actionStatus[4] = true;  //  example - reusing the same action!
              timers[4][IS_ACTIVE] = false;
           break;
           case 5:
              actionStatus[5] = true;
              timers[5][IS_ACTIVE] = false;
           break;
           case 6:
              actionStatus[6] = true;
              encoderReset(leftEncoder);
              encoderReset(rightEncoder);
              timers[6][IS_ACTIVE] = false;
         }
       }
     }

     // loop through all the actions....
     for (int i = 0; i<numActions; i++)
     {
       // should this action be started?
       if (actionStatus[i] == true)
       {
         switch (i) // decide which "case" to do based on the value of "i"
         {
           case 0:  // all ahead full....
             // this is an example of writing the code in the case....
             arrived = driveToTarget(1500);

             if (arrived)
             {
               actionStatus[0] = false; // deactivate this action; we're done!
               auto_y_motion = 0;
               auto_angle_motion =0;
             }
             break;
             actionStatus[0] = false;
           break;
           case 1:  // all stop.
             // this is an example of delegating the code to a reusable method.
             lift_input = 127;
             actionStatus[1] = false;
           break;
           case 2: // toggle the LED
             // if LED_state was true, make it false, or vice versa.
             lift_input = -127;
             actionStatus[2] = false;
           break;
           case 3: // reverse
              lift_input = 0;
              actionStatus[3] = false;
           break;
           case 4: // drive forward via encoders.
           claw_input = 50; // move forward straight;
           actionStatus[4] = false;
           break;
           case 5:
           backFull();
           actionStatus[5] = false;
           break;
           case 6:
           allStop();
           actionStatus[6] = false;
           break;
         }
       }
     }
                                                  // when we're @ encoder = 3000,
                                                  // "arrived" will be true.


     auton_process_motors();
     delay(1);
     // probably unneccesary, but if we aren't in auton mode but we are here somehow,
     //       lets leave this loop!
     if (!isAutonomous())
      break;
  }

}



/*
* tell all the drive motors to stop.
*/
void allStop()
{
    auto_x_motion = 0;
    auto_y_motion = 0;
    auto_angle_motion = 0;
}

/*
* turn on the motors full reverse.
*/
void backFull()
{
  auto_x_motion = 0;
  auto_y_motion = -127;
  auto_angle_motion = 0;
}

/*
*  moves the robot forward until the average encoder distance is roughly equal
*   to the target.
*  @return - whether we have arrived yet.
*/
bool driveToTarget(long target)
{
  long avg = (encoderGet(leftEncoder) + encoderGet(rightEncoder))/2;
  long diff = encoderGet(leftEncoder) - encoderGet(rightEncoder);
  long error = avg - target;
  auto_y_motion = -1*error;  // forward drive is proportional to error.
  auto_angle_motion = -1*diff; // turn proportional to angular error.

  return (abs(error) < 10); // say we're there if we get encoder within 10. (?)
}

bool Lift(int target)
{
  int liftPotentiometer = analogRead(LIFT_POTENTIOMETER);
  if (liftPotentiometer > target)
  {
    //make motor go down
  }
  if (liftPotentiometer < target)
  {
    //make motor go up
  }
  return (liftPotentiometer == target);
}

bool Claw(int target)
{
  int clawPotentiometer = analogRead(CLAW_POTENTIOMETER);
  if (clawPotentiometer > target)
  {
    //make claw close
  }
  if (clawPotentiometer < target)
  {
    //make claw open
  }
  return (clawPotentiometer == target);
}

bool Forearm(int target)
{
  int forearmPotentiometer = analogRead(FOREARM_POTENTIOMETER);
  if (forearmPotentiometer > target)
  {
    //make forearm go down
  }
  if (forearmPotentiometer < target)
  {
    //make forearm fo up
  }
  return (forearmPotentiometer == target);
}


/*
*  based on the state of the global variables, update the motors.
*/
void auton_process_motors()
{
   // tell the chassis how to drive...
   manageDriveMotors(auto_x_motion, auto_y_motion, auto_angle_motion);
   manageClawMotors(claw_input);
   manageForearmMotors(forearm_input);
   // turn on (true) or off (false) the LED on digital pin 3. (Not motor 3.)
   digitalWrite(3,LED_state);
}
