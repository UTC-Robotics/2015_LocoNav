#include <LineFollowerV2.h>
#include <DualVNH5019MotorShield.h>
#include <Arduino.h>
int SimLFPins[8]={36,38,40,42,44,46,48,50};
int MidLFPins[8]={37,39,41,43,45,47,49,51};
int CarLFPins[8]={36,38,40,42,44,46,48,50};
LineFollower MidLF(MidLFPins, 52);
LineFollower SimLF(SimLFPins, 53);
LineFollower CarLF(SimLFPins, 53);

int FollowLine(bool, int);
void SetMotorsToRotate(int Speed);
bool Anomalous(bool* );
bool FrontLine(bool* );
int TypeDistinguish(bool* Input);

DualVNH5019MotorShield shield;

void setup() {
  MidLF.enable();
  SimLF.enable();
  shield.init();
  shield.setM1Speed(0);
  shield.setM2Speed(0);

  Serial.begin(9600);
}

void loop() {
  bool* MLF;
  bool* FLF;
  while(true)
  {
    // Read the line followers
    MLF=ReadMiddleLF(0);
    FLF=ReadFrontLF(0);
    //
    if(Anomalous(MLF)==1)
    {
      int JunctionType=0;
      JunctionType=TypeDistinguish(MLF);
      if(JunctionType==-1)
        Serial.println("Error:: Is anomalous, but also just a line");
      else
      {
        if(FrontLine(FLF)==1)
        {
          JunctionType+=3;
        }
        Serial.println(JunctionType);
      }
    }
  }
  
  //FollowLine(0,-150);
}

int FollowLine(bool direc, int motorBase = -150)
{
    // Line follower arrays
  bool* MLF;
  bool* FLF;
  // Motor speed
  // Time index
  int Time=0;
  // Debugging feature variables
  int Interest=0;
  // State variables
  float LastAngErr=9999; // Initialize to unrealistic value to recognize the first loop
  
  while(true)
  {
    // Read the Line Followers
    MLF=ReadMiddleLF(0);
    FLF=ReadFrontLF(0);
    
    // Print the raw line follower reads for debugging
    /*
    for(int i=0;i<8;i++)
    {
      Serial.print((int)MLF[i]);
    }
    Serial.print("  ");
    for(int i=0;i<8;i++)
    {
      Serial.print((int)FLF[i]);
    }
    Serial.print("::");
    */
    
    // Measure the lateral error
    float LatErr=RawLFtoOffset(MLF);
    // Measure the front LF's offset
    float FrOff=RawLFtoOffset(FLF);
    // Calculate the angular error (radians) and print
    float AngErr=0;
    AngErr=atan((FrOff-LatErr)/125.0) * (180/3.141592); // Degrees
    //AngErr=atan((FrOff-LatErr)/125.0);                // Radians
        // Calculate the angular error difference...
    float DAng=0;
    if(LastAngErr != 9999) // ...unless this is the first iteration of the FollowLine loop
    {
      DAng=LastAngErr-AngErr;
    }
    // Save the angle for later difference calculations 
    LastAngErr=AngErr; 
    // PD Controller from Angle only
    //int deltaMotor = (int)( -50*AngErr + 20*DAng );
    // PD Controller on angle and offset
    int deltaMotor = (int)( 20*(AngErr+0.6*LatErr) + 20*DAng );
    /*
    if(deltaMotor>10)
      deltaMotor+=100;

    if(deltaMotor<10)
      deltaMotor-=100;
    */
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
      Serial.print(LatErr);
      // Print a return carriage
      Serial.println("");
    }
    // Set the motor speeds
    shield.setM1Speed(motorBase-deltaMotor);
    shield.setM2Speed(motorBase+deltaMotor);
    // Free the LF's Boolean arrays
    free(MLF);
    free(FLF);
    // Delay a bit
    delay(1);
    Time += 1;
  }
}

// Converts a Line Follower's reading into an offset from a blob in millimeters
float RawLFtoOffset(bool* LF)
{
  int i=0;
  int sum=0;
  float total=0;
  for(i=0;i<8;i++)
  {
    if( LF[i] == 1 )
    {
      sum+=i;
      total++;
    }
  }
  float centerp=sum/((float)total);
  float center=((7.0*centerp)-24.5);
  return(center);
}

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
  return(Reso);
}


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
  return(Reso);
}

bool* ReadMiddleLF(bool direc)
{
  bool* Reso;
  Reso=MidLF.measureBA();
  return(Reso);
}

void SetMotorsToRotate(int Speed)
{
  shield.setM1Speed(-Speed);
  shield.setM2Speed(Speed);
}

/*
int JakeDirections(bool* Input, bool* TopInput)
{
  int t = 0;
  int comp = 0;
  int checkup = 0;
    
  for (t = 0; t<8; t++){
   comp = comp+Input[t];
   checkup = checkup+TopInput[t];
  }

  if(checkup > 2){ //Checks the top Path Sensor
     printf("Upwards Path Exists\n");
  }

  if(comp > 4){ //Check for an Anomaly
     printf("Anomalous. Detecting direction trend..."); //Detected Anomalous
     if(Input[7] == 1){ //Checking the Right side
        if(Input[0] == 1){ //If line on the Right, Checking the Left side
           printf("T Connection"); //Both lines
        }else{
           printf("RIGHT Line detected"); //Right Heavy
        }
     }else{
        if(Input[0] == 1){ //Checking for Left Line
           printf("LEFT Line detected"); //Left Heavy
        }
     }
  }else{
     printf("Straight Line"); //Neither line
  }
}
*/

bool Anomalous(bool* Input)
{
  int t = 0;
  int comp = 0;
    
  for (t = 0; t<8; t++){
   comp = comp+Input[t];
  }

  if(comp > 4){
     //printf("Anomalous. Detecting direction trend..."); //Detected Anomalous
     return(1);
  }
  return(0);
}

bool FrontLine(bool* TopInput)
{
  int t = 0;
  int checkup = 0;
    
  for (t = 0; t<8; t++){
   checkup = checkup+TopInput[t];
  }

  if(checkup > 2){ //Checks the top Path Sensor
     //printf("Upwards Path Exists\n");
     return(1);
  }
  return(0);
}

int TypeDistinguish(bool* Input)
{
  // Assess the middle LF input
  if(Input[7] == 1){ //Checking the Right side
    if(Input[0] == 1){ //If line on the Right, Checking the Left side
      //printf("T Connection"); //Both lines
      return(2);
    }else{
      //printf("RIGHT Line detected"); //Right Heavy
      return(1);
    }
  }else{
    if(Input[0] == 1){ //Checking for Left Line
      //printf("LEFT Line detected"); //Left Heavy
      return(0);
    }
  }
  return(-1);
}
