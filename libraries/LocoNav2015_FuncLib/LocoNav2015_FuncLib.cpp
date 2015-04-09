#include "Arduino.h"
#include "LocoNav2015_FuncLib.h"
#include <LineFollowerV2.h>
#include <DualVNH5019MotorShield.h>
#include <digitalWriteFast.h>
#include <MEGAEncoders.h>

// Set speed for robot during line following
#define DWHEEL 4.8125
#define ROBOSPEED 160
//#define ROBOSPEED 110
#define TURNSPEED 120
//#define ROBOSPEED 120
#define SIMON 0
#define ETCHA 1
#define RUBIK 2
#define CARDS 3
#define ENDGM 4

// BEGIN FUNCTION DEFINITIONS 
// Dummy function for docking games : TO BE FILLED IN BY INTEGRATION
void dockGame(bool direc, int Game)
{
  int Amount=0;
  // do something like a switch case specifying the amount the bot should roll forwards
  switch(Game)
  {
    case SIMON:
    Amount=4;
    break;
    case ETCHA:
    Amount=3;
    break;
    case RUBIK:
    Amount=1;
    break;
    case CARDS:
    Amount=7;
    break;
    default:
    case ENDGM:
    while(true)
    {
      delay(1000);
      Serial.print("END GAME PAUSE");
    }
    break; 
  }
  //RollForwards(direc, Amount);
  return;
}

// Dummy function for enabling game-playing slave microcontrollers : TO BE FILLED IN BY INTEGRATION
void playGame(int Game)
{
  shield.setM1Speed(0);
  shield.setM2Speed(0);
  shield.setBrakes(400,400);  
  delay(1000);
  shield.setBrakes(0,0);
  // do a switch case to send the enable signal to the slave microcontrollers to play the games
  int pin=0;
  int TimeCap=0;
  // do something like a switch case specifying the amount the bot should roll forwards
  switch(Game)
  {
    case SIMON:
    pin=14;
    TimeCap=2;
    break;
    case ETCHA:
    pin=15;
    TimeCap=2;
    break;
    case RUBIK:
    pin=16;
    TimeCap=2;
    break;
    case CARDS:
    pin=17;
    TimeCap=2;
    break;
    default:
    case ENDGM:
    Serial.print("ERROR: Miscall to PlayGame function (not one of four main games)");
    break; 
  }
  digitalWrite(pin,HIGH);
  int Count=0;
  while(Count<TimeCap)
  {
    delay(1000);
    Count++;
  }
  digitalWrite(pin,LOW);
  return;
}

// Dummy function for undocking games
void undock(bool direc, int Game)
{
  // reverse the actions of "dockGame()" function. Just back out of the square until the back line follower is in contact with the road.
    if(direc==0)
  {
    shield.setM1Speed(ROBOSPEED);
    shield.setM2Speed(ROBOSPEED);
  }
  else
  {
    shield.setM1Speed(-ROBOSPEED);
    shield.setM2Speed(-ROBOSPEED);
  }
  
  bool* BLF;
  BLF=ReadBackLF(direc);
  while(Anomalous(BLF,4)==1)
  {
    delay(10);
    free(BLF);
    BLF=ReadBackLF(direc);
  }
  shield.setM1Speed(0);
  shield.setM2Speed(0);

  direc^=1;
  // Declare variables
  int Interest=200;
  int Time=0;
  float LastAngErr=9999; // Initialize to unrealistic value to recognize the first loop
  float AngErr=0;
  bool* FLF=ReadFrontLF(direc);
  // Measure the front LF's offset
  float FrOff=RawLFtoOffset(FLF);
  // Calculate the angular error (radians) and print
  AngErr=atan((FrOff-0)/125.0) * (180/3.141592); // Degrees
  while(Interest>0)
  {
    // Free the LF's Boolean arrays
    free(FLF);
    // Read the Line Followers
    FLF=ReadFrontLF(direc);
    // Measure the front LF's offset
    FrOff=RawLFtoOffset(FLF);
    // Calculate the angular error (radians) and print
    AngErr=atan((FrOff-0)/125.0) * (180/3.141592); // Degrees
    //AngErr=atan((FrOff-LatErr)/125.0);                // Radians
        // Calculate the angular error difference...
    float DAng=0;
    if(LastAngErr != 9999) // ...unless this is the first iteration of the FollowLine loop
    {
      DAng=LastAngErr-AngErr;
    }
    // Save the angle for later difference calculations 
    LastAngErr=AngErr; 
    // PD Controller on angle and offset
    int deltaMotor = (int)( 15*AngErr - 5*DAng );
    // Interest
    if(AngErr > 6 || AngErr<-6)
    {
      Interest=200;
    }
    else
    {
      Interest--;
    }
    // Print the angular error and time if the angular error has been nonzero recently
    /*
    Serial.print(Time);
    Serial.print(",");
    Serial.print(AngErr);
    Serial2.print(Time);
    Serial2.print(",");
    Serial2.print(AngErr);
    // Print a return carriage
    Serial.println("");
    Serial2.println("");
    */
    // Set the motor speeds
    shield.setM1Speed(deltaMotor*-2);
    shield.setM2Speed(deltaMotor*2);
    // Delay a bit
    delay(1);
    Time += 1;
  }
  shield.setM1Speed(0);
  shield.setM2Speed(0);
  shield.setBrakes(400,400);
  delay(1000);
  shield.setBrakes(0,0);

  return;
}

// Function to roll robot forward an arbitrary amount
void RollForwards(bool direc, int Amount)
{
  // Turn motors on to move forwards
  if(direc==0)
  {
    shield.setM1Speed(-ROBOSPEED);
    shield.setM2Speed(-ROBOSPEED);
  }
  else
  {
    shield.setM1Speed(ROBOSPEED);
    shield.setM2Speed(ROBOSPEED);
  }
  // Wait a bit to give the robot time to roll
  //delay(200*Amount);
  int dist=(int)((float)Amount/((3.141592*DWHEEL)/12000.0));
  dist=Amount*450;
  int startRCount=getEncRCount();
  if(direc==0)
  {
    while(getEncRCount()>startRCount-dist)
    {
      Serial.println(getEncRCount());
      delay(10);
    }
  }
  else
  {
    while(getEncRCount()<startRCount+dist)
    {
      Serial.println(getEncRCount());
      delay(10);
    }
  }
  // Turn off motors
  shield.setM1Speed(0);
  shield.setM2Speed(0);
  shield.setBrakes(400,400);  
  delay(1000);
  shield.setBrakes(0,0);
  return;
}

// Executes a turn onto a new branch
void Turn(bool direc, bool Direction)
{
  
  // Declare variables
  int motorTarget=0;
  int curSpeed=0;
  int delCount=0;
  //  Decide whether clockwise or counterclockwise is right/left 
  if(Direction==0)
  {
    curSpeed=-TURNSPEED;
    motorTarget=-TURNSPEED;
    SetMotorsToRotate(curSpeed);
    int startCount=getEncRCount();
    while(getEncRCount()>startCount-2500)
    {
      delay(1);
    }
    SetMotorsToRotate(0);
  }
  else
  {
    curSpeed=TURNSPEED;
    motorTarget=TURNSPEED;
    SetMotorsToRotate(curSpeed);
    int startCount=getEncRCount();
    while(getEncRCount()<startCount+2500)
    {
      delay(1);
    }
    SetMotorsToRotate(0);
  }
  /*
  // Keep on turning until current line is no longer seen
  FLF=ReadFrontLF(direc);
  while(FrontLine(FLF)==1)
  {
    // Read line follower
    free(FLF);
    FLF=ReadFrontLF(direc);
    // Slowly ramp up turning speed to avoid jerkiness
    if(curSpeed>motorTarget)
      curSpeed--;
    if(curSpeed<motorTarget)
      curSpeed++;
    SetMotorsToRotate(curSpeed);
    // Delay a bit
    delay(10);
  }
  */
  bool* FLF;
  FLF=ReadFrontLF(direc);
  // Keep on turning until the new line appears
  while(FrontLine(FLF)==0)
  {
    free(FLF);
    // Read front line follower
    FLF=ReadFrontLF(direc);
    // Slowly ramp up turn speed to avoid jerkiness
    if(curSpeed>motorTarget)
      curSpeed--;
    if(curSpeed<motorTarget)
      curSpeed++;
    SetMotorsToRotate(curSpeed);
    // Delay a bit
    delay(1);
  }
  
  // Brake right on top of the line
  SetMotorsToRotate(0);
  shield.setBrakes(400,400);
  delay(1000);
  shield.setBrakes(0,0);
  //bool* FLF;
  
  // Use the PD Controller to center robot over the new line
 
  // Declare variables
  int Interest=200;
  int Time=0;
  float LastAngErr=9999; // Initialize to unrealistic value to recognize the first loop
  float AngErr=0;
  FLF=ReadFrontLF(direc);
  // Measure the front LF's offset
  float FrOff=RawLFtoOffset(FLF);
  // Calculate the angular error (radians) and print
  AngErr=atan((FrOff-0)/125.0) * (180/3.141592); // Degrees
  while(Interest>0)
  {
    // Free the LF's Boolean arrays
    free(FLF);
    // Read the Line Followers
    FLF=ReadFrontLF(direc);
    // Measure the front LF's offset
    FrOff=RawLFtoOffset(FLF);
    // Calculate the angular error (radians) and print
    AngErr=atan((FrOff-0)/125.0) * (180/3.141592); // Degrees
    //AngErr=atan((FrOff-LatErr)/125.0);                // Radians
        // Calculate the angular error difference...
    float DAng=0;
    if(LastAngErr != 9999) // ...unless this is the first iteration of the FollowLine loop
    {
      DAng=LastAngErr-AngErr;
    }
    // Save the angle for later difference calculations 
    LastAngErr=AngErr; 
    // PD Controller on angle and offset
    int deltaMotor = (int)( 15*AngErr - 5*DAng );
    // Interest
    if(AngErr > 6 || AngErr<-6)
    {
      Interest=200;
    }
    else
    {
      Interest--;
    }
    // Print the angular error and time if the angular error has been nonzero recently
    /*
    Serial.print(Time);
    Serial.print(",");
    Serial.print(AngErr);
    Serial2.print(Time);
    Serial2.print(",");
    Serial2.print(AngErr);
    // Print a return carriage
    Serial.println("");
    Serial2.println("");
    */
    // Set the motor speeds
    shield.setM1Speed(deltaMotor*-2);
    shield.setM2Speed(deltaMotor*2);
    // Delay a bit
    delay(1);
    Time += 1;
  }
  /*
  if(direc==0)
  {
    RollForwards(1,3);
  }
  else
  {
    RollForwards(0,3);
  }
  */
  return;
}

void LiftMotor(float Amount)
{
  //step configuration pins
  pinMode(14, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(17, OUTPUT);
  
 //Step, direction, and nSLP
  pinMode(53, OUTPUT); //Step
  pinMode(49, OUTPUT); //Direction
  pinMode(45, OUTPUT); //not Sleep

  // Turn On
  digitalWrite(45,HIGH);
  delay(400);

  // 1/32 Step Configuration
  digitalWrite(14,HIGH);
  digitalWrite(15,HIGH);
  digitalWrite(17,HIGH);

  // Set to spin
  if(Amount<0) 
  {
    digitalWrite(49,LOW); 
  }
  else
  {
    digitalWrite(49,HIGH);
  }
  tone(53,20000);
  delay(((abs(Amount)-0.5)/1.2)*1000.0);
  // Slow phase
  tone(53,5000);
  delay(1667);

  tone(53,0);
  digitalWrite(45,LOW);

  return;
}

void BackParallelPark(bool direc, int motorBase = -150, int BackTime=500)
{
  // Line follower arrays
  bool* FLF;
  // Motor speed
  direc^=1;
  if(direc==1)
    motorBase=motorBase*-0.85;
  // Time index
  int Time=0;
  int JunctionType=0;
  // Debugging feature variables
  int Interest=0;
  // State variables
  float LastAngErr=9999; // Initialize to unrealistic value to recognize the first loop
  // Junction recognition flags
  bool Lefto  = 0;
  bool Righto = 0;
  bool JunctionSighted=0;
  bool Fronto = 0;
  // Keep on moving until the center LF has seen an anomaly and the middle LF is over an anomaly
  int Counter=0;
  while(!(JunctionSighted==1 || Counter<BackTime))
  {
    Counter++;
    // Read the Line Followers
    FLF=ReadFrontLF(direc);
    // Print the raw line follower reads for debugging
    for(int i=0;i<8;i++)
    {
      Serial.print((int)FLF[i]);
    }
    Serial.print("::");
    // Measure the front LF's offset
    float FrOff=RawLFtoOffset(FLF);
    // Calculate the angular error (radians) and print
    float AngErr=0;
    AngErr=atan((FrOff)/125.0) * (180/3.141592); // Degrees
    // Calculate the angular error difference...
    float DAng=0;
    if(LastAngErr != 9999) // ...unless this is the first iteration of the FollowLine loop
    {
      DAng=LastAngErr-AngErr;
    } 
    if(Anomalous(FLF,4)==1 || FrontLine(FLF)==0)
    {
      JunctionSighted=1;
    }
    // Save the angle for later difference calculations 
    LastAngErr=AngErr; 
    // Set the rotation speed
    int deltaMotor=0;
    // PD Control
    deltaMotor = (int)( 12*AngErr - 20*DAng );
    // Print the angular error and time if the angular error has been nonzero recently
    if(AngErr > 0.001 || AngErr<-0.001)
      Interest=10;
    if(Interest>0)
    {
      Interest--;
      Serial.print(Time);
      Serial.print(",");
      Serial.print(AngErr);
      Serial.print(",");
      Serial.print(FrOff);        
    }
    // Print a return carriage
    Serial.println("");
    // Set the motor speeds
    shield.setM1Speed(motorBase-deltaMotor*2);
    shield.setM2Speed(motorBase+deltaMotor*2);
    // Free the LF's Boolean arrays
    free(FLF);
    // Delay a bit
    delay(1);
    Time += 1;
  }
  shield.setM1Speed(0);
  shield.setM2Speed(0);
  shield.setBrakes(400,400);  
  delay(1000);
  shield.setBrakes(0,0);
  return;
}

void Parallelize(bool direc, int motorBase)
{
  if(direc==0)
  {
    RollForwards(1,4);
  }
  else
  {
    RollForwards(0,4);
  }
  // Line follower arrays
  bool* FLF;
  // Motor speed
  if(direc==1)
    motorBase=motorBase*-0.85;
  // Time index
  int Time=0;
  int JunctionType=0;
  // Debugging feature variables
  int Interest=0;
  // State variables
  float LastAngErr=9999; // Initialize to unrealistic value to recognize the first loop
    static int LastTicks=0;
  // Junction recognition flags
  bool Lefto  = 0;
  bool Righto = 0;
  bool JunctionSighted=0;
  bool Fronto = 0;
  // Keep on moving until the center LF has seen an anomaly and the middle LF is over an anomaly
  int Counter=0;
  while(Counter<1000)
  {
    Counter++;
    // Read the Line Followers
    FLF=ReadFrontLF(direc);
    // Print the raw line follower reads for debugging
    for(int i=0;i<8;i++)
    {
      Serial.print((int)FLF[i]);
    }
    Serial.print("::");
    // Measure the front LF's offset
    float FrOff=RawLFtoOffset(FLF);
    // Calculate the angular error (radians) and print
    float AngErr=0;
    AngErr=atan((FrOff)/125.0) * (180/3.141592); // Degrees
    // Calculate the angular error difference...
    float DAng=0;
    if(LastAngErr != 9999) // ...unless this is the first iteration of the FollowLine loop
    {
      DAng=LastAngErr-AngErr;
    }
    // Save the angle for later difference calculations 
    LastAngErr=AngErr; 
    // Set the rotation speed
    int deltaMotor=0;
    // If the front line follower sees a junction take a snapshot and roll over it.
    if(Anomalous(FLF,3))
    {
      deltaMotor=0;
      // Snapshot: Recognize whether this anomaly has left or right branches
      if(FLF[0]==1 || FLF[1] == 1)
      {
        Lefto=1;
      }
      if(FLF[6]==1)
      {
        Righto=1;
      } 
      // Flag that the front line follower has crossed a junction
      JunctionSighted=1;    
      Serial.print("LF: Left/Right = ");
      Serial.print(Lefto);
      Serial.print("/");
      Serial.print(Righto);
      Serial.print(" ::  ");
    }
    else
    {
      // PD Control
      deltaMotor = (int)( 15*AngErr - 20*DAng );
    }
    // Print the angular error and time if the angular error has been nonzero recently
    if(AngErr > 0.001 || AngErr<-0.001)
      Interest=10;
    if(Interest>0 && 0)
    {
      Interest--;
      Serial.print(Time);
      Serial.print(",");
      Serial.print(AngErr);
      Serial.print(",");
      Serial.print(FrOff);      
    }
    // Print the speed data
    int CurTicks=getEncRCount();
    Serial.print(Time);
    Serial.print(",");
    Serial.print(CurTicks-LastTicks);
    LastTicks=CurTicks;
    // Print a return carriage
    Serial.println("");
    // Set the motor speeds
    shield.setM1Speed(motorBase-2*deltaMotor);
    shield.setM2Speed(motorBase+2*deltaMotor);
    // Free the LF's Boolean arrays
    free(FLF);
    // Delay a bit
    delay(1);
    Time += 1;
  }
}

int FollowLine(bool direc, int motorBase = -150)
{
  // Line follower arrays
  bool* FLF;
  // Motor speed
  if(direc==1)
    motorBase=motorBase*-0.85;
  // Time index
  float Time=0;
  int JunctionType=0;
  // Debugging feature variables
  int Interest=0;
  // State variables
  float LastAngErr=9999; // Initialize to unrealistic value to recognize the first loop
    static int LastTicks=0;
  // Junction recognition flags
  bool Lefto  = 0;
  bool Righto = 0;
  int JunctionSighted=0;
  bool Fronto = 0;
  long DelTime=0;
  long LastMic=0;
  // Keep on moving until the center LF has seen an anomaly and the middle LF is over an anomaly
  //KeepGoing:
  while(!(JunctionSighted==1))
  {
    // Read the Line Followers
    FLF=ReadFrontLF(direc);
    // Print the raw line follower reads for debugging
    for(int i=0;i<8;i++)
    {
      Serial.print((int)FLF[i]);
    }
    Serial.print("::");
    // Measure the front LF's offset
    float FrOff=RawLFtoOffset(FLF);
    // Calculate the angular error (radians) and print
    float AngErr=0;
    AngErr=atan((FrOff)/125.0) * (180/3.141592); // Degrees
    // Calculate the angular error difference...
    float DAng=0;
    if(LastAngErr != 9999) // ...unless this is the first iteration of the FollowLine loop
    {
      DAng=LastAngErr-AngErr;
    }
    // Save the angle for later difference calculations 
    LastAngErr=AngErr; 
    // Set the rotation speed
    int deltaMotor=0;
    // If the front line follower sees a junction take a snapshot and roll over it.
    if(Anomalous(FLF,3))
    {
      deltaMotor=0;
      // Snapshot: Recognize whether this anomaly has left or right branches
      if(FLF[0]==1 || FLF[1] == 1)
      {
        Lefto=1;
      }
      if(FLF[6]==1)
      {
        Righto=1;
      } 
      // Flag that the front line follower has crossed a junction
      JunctionSighted=1; 
      Serial.print("LF: Left/Right = ");
      Serial.print(Lefto);
      Serial.print("/");
      Serial.print(Righto);
      Serial.print(" ::  ");
    }
    else
    {
      // PD Control
      //deltaMotor = (int)( 17*AngErr - 20*DAng );
      deltaMotor = (int)( 12*AngErr - 20*DAng );
    }
    // Print the angular error and time if the angular error has been nonzero recently
    if(AngErr > 0.001 || AngErr<-0.001)
      Interest=10;
    if(Interest>0 && 0)
    {
      Interest--;
      Serial.print(",");
      Serial.print(AngErr);
      Serial.print(",");
      Serial.print(FrOff);      
    }
    // Print the speed data
    int CurTicks=getEncRCount();
      DelTime=micros()-LastMic;
      LastMic=micros();
    Serial.print(DelTime);
    Serial.print(",");
    Serial.print(CurTicks-LastTicks);
    LastTicks=CurTicks;
    // Print a return carriage
    Serial.println("");
    // Set the motor speeds
    shield.setM1Speed(motorBase-2*deltaMotor);
    shield.setM2Speed(motorBase+2*deltaMotor);
    // Free the LF's Boolean arrays
    free(FLF);
    // Delay a bit
    delay(1);
    Time += 1;
  }
  /*
  shield.setM1Speed(0);
  shield.setM2Speed(0);
  shield.setBrakes(400,400);  
  delay(1000);
  shield.setBrakes(0,0);
  int JunctionCounting=0;
  while(Anomalous(FLF,3)==1 && JunctionCounting<30)
  {
    FLF=ReadFrontLF(direc);
    Serial.print("Double checking:");
    for(int i=0;i<8;i++)
    {
      Serial.print((int)FLF[i]);
    }
    Serial.println("");
    JunctionCounting++;
    free(FLF);
    delay(30);
  }
  if(JunctionCounting<25)
  {
    JunctionSighted=0;
    JunctionCounting=0;
    goto KeepGoing;
  }
  */
  
  // The wibble wobbele
  shield.setM1Speed(0);
  shield.setM2Speed(0);
  shield.setBrakes(400,400);  
  delay(1000);
  shield.setBrakes(0,0);
  JunctionSighted=0;
  direc^=1;
  while(!(JunctionSighted==1))
  {
    // Read the Line Followers
    FLF=ReadFrontLF(direc);
    // Print the raw line follower reads for debugging
    for(int i=0;i<8;i++)
    {
      Serial.print((int)FLF[i]);
    }
    Serial.print("::");
    // Measure the front LF's offset
    float FrOff=RawLFtoOffset(FLF);
    if( (FrOff>-8 && FrOff<8) || FrontLine(FLF)==0)
      JunctionSighted=1;
    // Calculate the angular error (radians) and print
    float AngErr=0;
    AngErr=atan((FrOff)/125.0) * (180/3.141592); // Degrees
    // Calculate the angular error difference...
    float DAng=0;
    if(LastAngErr != 9999) // ...unless this is the first iteration of the FollowLine loop
    {
      DAng=LastAngErr-AngErr;
    }
    // Save the angle for later difference calculations 
    LastAngErr=AngErr; 
    // Set the rotation speed
    int deltaMotor=0;
    // If the front line follower sees a junction take a snapshot and roll over it.
    if(Anomalous(FLF,3))
    {
      deltaMotor=0;
      // Snapshot: Recognize whether this anomaly has left or right branches
      if(FLF[0]==1 || FLF[1] == 1)
      {
        Lefto=1;
      }
      if(FLF[6]==1)
      {
        Righto=1;
      } 
      // Flag that the front line follower has crossed a junction
      JunctionSighted=1;    
      Serial.print("LF: Left/Right = ");
      Serial.print(Lefto);
      Serial.print("/");
      Serial.print(Righto);
      Serial.print(" ::  ");
    }
    else
    {
      // PD Control
      deltaMotor = (int)( 15*AngErr - 15*DAng );
    }
    // Print the angular error and time if the angular error has been nonzero recently
    if(AngErr > 0.001 || AngErr<-0.001)
      Interest=10;
    if(Interest>0 && 0)
    {
      Interest--;
      Serial.print(Time);
      Serial.print(",");
      Serial.print(AngErr);
      Serial.print(",");
      Serial.print(FrOff);      
    }
    // Print the speed data
    int CurTicks=getEncRCount();
    Serial.print(Time);
    Serial.print(",");
    Serial.print(CurTicks-LastTicks);
    LastTicks=CurTicks;
    // Print a return carriage
    Serial.println("");
    // Set the motor speeds
    shield.setM1Speed(-2*deltaMotor);
    shield.setM2Speed(2*deltaMotor);
    // Free the LF's Boolean arrays
    free(FLF);
    // Delay a bit
    delay(1);
    Time += 1;
  }
  direc^=1;
  
  //RollForwards(direc,6);
  //int dist=450*6;
  int dist=450*2;
  int startRCount=getEncRCount();
  shield.setM1Speed(motorBase);
  shield.setM2Speed(motorBase);
  if(direc==0)
  {
    while(getEncRCount()>startRCount-dist)
    {
      FLF=ReadFrontLF(direc);
      for(int i=0;i<8;i++)
      {
        Serial.print((int)FLF[i]);
      }
      Serial.print(",");
      if(FLF[0]==1 || FLF[1] == 1)
      {
        Lefto=1;
      }
      if(FLF[6]==1)
      {
        Righto=1;
      } 
      free(FLF);
      Serial.println(getEncRCount());
      delay(1);
    }
  }
  else
  {
    while(getEncRCount()<startRCount+dist)
    {
      FLF=ReadFrontLF(direc);
      for(int i=0;i<8;i++)
      {
        Serial.print((int)FLF[i]);
      }
      Serial.print(",");
      if(FLF[0]==1 || FLF[1] == 1)
      {
        Lefto=1;
      }
      if(FLF[6]==1)
      {
        Righto=1;
      } 
      free(FLF);
      Serial.println(getEncRCount());
      delay(1);
    }
  }
  // Turn off motors
  shield.setM1Speed(0);
  shield.setM2Speed(0);
  shield.setBrakes(400,400);  
  delay(1000);
  shield.setBrakes(0,0);
  // Identify the junction from the gathered data about other branches
  //Serial.print("LF: Left/Right/Front = ");
  FLF=ReadFrontLF(direc);
  if(FrontLine(FLF)==1)
  {
    Fronto=1;
  }
  free(FLF);
  delay(1000);
  // Junction type return
  if(Lefto==1)
  {
    if(Righto==1)
    {
      JunctionType=2;
    }
    else
    {
      JunctionType=0;
    }
  }
  else
  {
    if(Righto==1)
    {
      JunctionType=1;
    }
    else
    {
      JunctionType=-10;
    }
  }
  // If we don't identify any other branches, then something has gone terribly wrong. Just stop the robot for debugging purposes.
  if(JunctionType==-10)
  {
    // Infinite loop of despair
    while(true)
    {
      Serial.println("SADNESS: BAD JUNCTION RECOGNITION");
      Serial2.println("SADNESS: BAD JUNCTION RECOGNITION");
      delay(1000);
    }
  }
  if(Fronto==1)
  {
    JunctionType+=3;
  }
  return(JunctionType);
}

// Converts a Line Follower's reading into an offset from a blob in millimeters
float RawLFtoOffset(bool* LF)
{
  int i=0;
  float sum=0;
  float total=0;
  for(i=0;i<7;i++)
  {
    if( LF[i] == 1 )
    {
      sum+=(float)i;
      total++;
    }
  }
  float centerp=sum/total;
  float center=7.0*centerp-24.5;
  return(center);
}

// Reverses the order of a boolean array.
void JuggleBoolArray(bool* Op)
{
  bool Temp[8];
  for(int Q=0;Q<8;Q++)
  {
    Temp[Q]=Op[7-Q];
  }
  for(int Q=0;Q<8;Q++)
  {
    Op[Q]=Temp[Q];
  }
}

// Reads the back line follower depending on whether Simon or Card is frontwards.
bool* ReadBackLF(bool direc)
{
  bool* Reso;
  if(!direc)
  {
    Reso=CarLF.measureBA();
  }
  else
  {
    Reso=SimLF.measureBA();
  }
  if(direc==0)
  {
    JuggleBoolArray(Reso);
  }
  return(Reso);
}

// Reads the front line follower depending on whether Simon or Card is frontwards.
bool* ReadFrontLF(bool direc)
{
  bool* Reso;
  if(direc)
  {
    Reso=CarLF.measureBA();
  }
  else
  {
    Reso=SimLF.measureBA();
  }
  if(direc==0)
  {
    JuggleBoolArray(Reso);
  }
  return(Reso);
}

// Reads the middle line follower
bool* ReadMiddleLF(bool direc)
{
  bool* Reso;
  Reso=MidLF.measureBA();
  if(direc==0)
  {
    JuggleBoolArray(Reso);
  }
  return(Reso);
}

// Starts the robot pure pivoting
void SetMotorsToRotate(int Speed)
{
  shield.setM1Speed(-Speed);
  shield.setM2Speed(Speed);
}

// Identifies when a junction is particularly "fat"
bool Anomalous(bool* Input, int Thresh)
{
  int t = 0;
  int comp = 0;
    
  for (t = 0; t<7; t++){
   comp = comp+Input[t];
  }

  //if(comp > 5 && (Input[0]==1 || Input[7]==1) ){
  if(comp > Thresh ){
     //printf("Anomalous. Detecting direction trend..."); //Detected Anomalous
     return(1);
  }
  /*
  if(Input[0]==1 || Input[6]==1)
  {
    return(1);
  }
  */
  return(0);
}

// Checks whether any line exists on a line follower signature
bool FrontLine(bool* TopInput)
{
  int t = 0;
  int checkup = 0;
    
  for (t = 0; t<7; t++){
   checkup = checkup+TopInput[t];
  }

  if(checkup > 1){ //Checks the top Path Sensor
     //printf("Upwards Path Exists\n");
     return(1);
  }
  return(0);
}