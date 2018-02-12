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

int message = 0;

// Initially, none of these actions will fire...
bool actionStatus[] = {false,  // 0. ahead full
                       false,  // 1. forearm up
                       false,  // 2. forearm down
                       false,  // 3. forearm stop
                       false,  // 4. claw open
                       false,  // 5. reverse
                       false}; // 6. stop driving
int timers[][4] = {{0,true},        //0. activates encoder drive
                   {1000,true},      //1. trigger action 1
                   {3000,true},     //2. trigger action 2
                   {5000,true},     //3. trigger action 3
                   {2750,false},     //4. trigger action 4
                   {3750,false},     //5. trigger action 5
                   {4500,false}};    //6. trigger action 6

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
    // message = 0;
    // loop through all the timers....
    for (int i = 0; i<numTimers; i++)
    {
       // Should this particular timer be triggered?
       if ((timers[i][IS_ACTIVE] == true) && (timers[i][TRIGGER_TIME] <= timeSinceStart))
       {
         switch (i) // decide which "case" to do based on the value of "i"
         {
           case 0: // close the claw
              claw_input = CLAW_CLOSED;
              deactivateTimer(0); // ok, we've done this don't trigger it again.
           break;
           case 1: // lift arm
              activateAction(0);
              deactivateTimer(1);   // a one-time trigger.
            break;
            case 2:
              activateAction(1);
              deactivateTimer(2);
            break;
            case 3:
              activateAction(2);
              deactivateTimer(3);
            break;

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
             arrived = armToTarget(1670);
             message = analogRead(FOREARM_POTENTIOMETER);
             if (arrived)
             {
               deactivateAction(0);  //we're done!
               forearm_input = 0;
             }
           break;
           case 1:
            arrived = driveToTarget(300);
            message = analogRead(encoderGet(leftEncoder));
            if (arrived)
            {
              deactivateAction(1);
              auto_y_motion = 0;
            }
            break;
            case 2:
              arrived = turnToTarget(236);
              message = analogRead(encoderGet(rightEncoder));
            if (arrived)
            {
              deactivateAction(2);
              auto_angle_motion = 0;
            }
            break;
            //case 3:
              //arrived = liftToTarget(2700);

            //break;

         }
       }
     }
                                                  // when we're @ encoder = 3000,
                                                  // "arrived" will be true.

     if (LCD_ACTIVE)
     {
         updateScreenAutonomous();
     }
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

bool turnToTarget(long target)
{
  long diff = encoderGet(leftEncoder) - encoderGet(rightEncoder);
  long error = diff - target;

  auto_angle_motion = -1*error;

  return (abs(error) <10);
}

bool liftToTarget(int target)
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

bool armToTarget(int target)
{
  int armPotentiometer = analogRead(FOREARM_POTENTIOMETER);
  int error = target - armPotentiometer;
  forearm_input = -3* error;
  return abs(error) < 10;
}

// bool Claw(int target)
// {
//   int clawPotentiometer = analogRead(CLAW_POTENTIOMETER);
//   if (clawPotentiometer > target)
//   {
//     //make claw close
//   }
//   if (clawPotentiometer < target)
//   {
//     //make claw open
//   }
//   return (clawPotentiometer == target);
// }

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

void updateScreenAutonomous()
{

  char topString[16];
  char bottomString[16];
  snprintf(topString, 16, "status:%d ",message); // combine string with a variable
  //snprintf(bottomString, 16, "Lf:%d arm:%d",lift_pot_value,forearm_pot_value); // combine string with a variable
  lcdSetText(uart1, 1, topString);
  // lcdPrint(uart1, 1, "Go Falcons!");
  //printf("Encoders Are: %d, %d\n",encoderGet(leftEncoder), encoderGet(rightEncoder));
}

void activateAction(int which)
{
  actionStatus[which] = true;
}

void deactivateAction(int which)
{
  actionStatus[which] = false;
}

void activateTimer(int which)
{
  timers[which][IS_ACTIVE]  = true;
}

void deactivateTimer(int which)
{
  timers[which][IS_ACTIVE] = false;
}
