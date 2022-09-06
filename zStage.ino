
#include <AccelStepper.h>

#define DEG_PER_STEP 0.1125
#define STEP_PER_REVOLUTION (360 / DEG_PER_STEP) //3200 szteps, 0.3125 microns per step
#define MAX_MESSAGE_LENGTH 64

AccelStepper stepper(1, 3, 2);

// limit switch detection
int lowSwitch =  A0;
//int highSwitch = A1; never got high switch to work

int sr[30]; // lower switch readings, large buffer to prevent false limit switch triggers -- no idea if this is overkill long or too short. 
int srSum = 123456;
bool justHitSwitch = false; // on first startup of arduino, make sure this agrees -- stage is not on limit switch
// x is tracked by stepper position, in steps 

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(1);

  pinMode(lowSwitch, INPUT);

  stepper.setCurrentPosition(0); // set position
  stepper.setAcceleration(800);  // set acceleration
  stepper.setMaxSpeed(3200);     // set initial speed

  Serial.println("Z Stage Ready.");
}

bool hitLimitSwitch(){
  
  //Serial.println(srSum);

  // read switch, update records of sums
  for(int i=0; i<29; i++)
    sr[i] = sr[i+1];

  srSum += analogRead(lowSwitch);

  if(srSum == sr[0]){ // new readings have been 0 for 30 ticks (sum hasn't changed)
    Serial.println("hit lower switch");
    //Serial.print(srSum); Serial.print(','); Serial.println(sr[0]);

    //protect against thinking it's still hit later
    srSum +=1; 

    // erase overflow distance
    stepper.moveTo(0);

    justHitSwitch = true;
    return true;
  }
  sr[29] = srSum;

  return false;
}

void home_motor()
{
  Serial.println("homing z-stage.");
  //Serial.println(-2000000*64L); // just a big number
  moveToWithLimitSwitch(-2000000*64L);
  
  stepper.setCurrentPosition(0);
  // erase overflow distance
  stepper.moveTo(0);

  moveToWithoutLimitSwitch(3200);
  moveToWithLimitSwitch(-6400);
  stepper.setCurrentPosition(0);
  // erase overflow distance
  stepper.moveTo(0);

  moveToWithoutLimitSwitch(1600);
  stepper.setCurrentPosition(0);

  Serial.println("homing completed");
}

// returns false if cannot move
// only relative movement. python handles absolute movement
bool moveMaster(long steps)
{
  
  //Serial.print("moveMaster received "); Serial.println(steps);
  // case hit lower switch, trying to move further in that direction
  // this first branch should never hit, python should handle moving past 0 once homed. 
  if(justHitSwitch && steps < 0){ 
    Serial.println("Hit limit switch--cannot move in that direction."); 
    return false;
  }
  // case hit lower switch, moving away from switch (avoid period of easing off the switch)
  else if (steps > 0){ 
    justHitSwitch = false;
    moveToWithoutLimitSwitch(steps);
  }
  // moving towards limit switch 
  else 
    moveToWithLimitSwitch(steps);

  return true;
}

void moveToWithLimitSwitch(long relative)
{
  //Serial.print("moving relative steps "); Serial.println(-1L * relative);
  stepper.move(-1L * relative); 
  //Serial.print("distance to go before: "); Serial.println(stepper.distanceToGo());
  
  while (!hitLimitSwitch() && stepper.distanceToGo() != 0){
    stepper.run();
    //Serial.print("distance to go: "); Serial.println(stepper.distanceToGo());
  }

  stepper.stop();
  //Serial.print("distance to go after: "); Serial.println(stepper.distanceToGo());
}

void moveToWithoutLimitSwitch(long relative)
{
  //Serial.print("distance to go before: "); Serial.println(stepper.distanceToGo());
  //Serial.print("moving relative steps "); Serial.println(-1L * relative); 
  stepper.move(-1L * relative); 
  //Serial.print("distance to go middle: "); Serial.println(stepper.distanceToGo());

  while (stepper.distanceToGo() != 0){
    stepper.run();
    //Serial.print("distance to go: "); Serial.println(stepper.distanceToGo());
  }

  stepper.stop();
  //Serial.print("distance to go after: "); Serial.println(stepper.distanceToGo());
}

void loop() {
 //Check to see if anything is available in the serial receive buffer

 while (Serial.available() > 0)
 {
   //Create a place to hold the incoming message
   static char message[MAX_MESSAGE_LENGTH];
   static unsigned int message_pos = 0;

   //Read the next available byte in the serial receive buffer
   char inByte = Serial.read();

  // message end flag 'E'
   if (inByte != 'E')
   {
     //Add the incoming byte to our message
     message[message_pos] = inByte;
     message_pos++;
   }
   //Full message received...
   else
   {
      //Add null character to string
      message[message_pos] = '\0';

      //Reset for the next message
      message_pos = 0;

      //int number = atoi(message);
      //Serial.print("received ");
      //Serial.println(message);

      //Serial.print('\''); Serial.print(message); Serial.println('\'');
      if(message[0] == 'h')
        home_motor(); 
      else
        moveMaster(atol(message));
      
      //Serial.println("done");
   }
 }
 
}