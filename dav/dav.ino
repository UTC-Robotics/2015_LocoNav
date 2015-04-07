// Line Follower Provider's Library
#include <LineFollowerV2.h>
// Motor shield library
#include <DualVNH5019MotorShield.h>
// Generic Arduino library
#include <Arduino.h>

// Set speed for robot during line following
#define ROBOSPEED 160
#define SIMON 0
#define ETCHA 1
#define RUBIK 2
#define CARDS 3
#define ENDGM 4

// Initialize pins for the line followers
int SimLFPins[8]={20,22,24,26,28,30,32,34};
int MidLFPins[8]={21,23,25,27,29,31,33,35};
int CarLFPins[8]={36,38,40,42,44,46,48,50};

LineFollower MidLF(MidLFPins);
LineFollower SimLF(SimLFPins);
LineFollower CarLF(CarLFPins);
// Declare the motor shield's handler object
DualVNH5019MotorShield shield;

// Function prototypes
int FollowLine(bool, int);          // Follow a line until a junction/turn
void SetMotorsToRotate(int Speed);  // Starts the robot rotating
bool Anomalous(bool* , int);             // Judges whether a junction/turn is present on the line follower array
bool FrontLine(bool* );             // Judges whether a line exists on the line follower array
void Turn(bool, bool);              // Makes the robot rotate left/right until it sees a line and then rolls forward a bit off of the junction/turn
void JuggleBoolArray(bool*);        // Flipflops the boolean array 
void RollForwards(int);                // Rolls forward an arbitrary amount

// Unwritten dummy functions
void dockGame(int);
void playGame(int);
void undock(int);

// Setup all the inputs and outputs
void setup() {
  //CarLF.enable();
  // Turn on the motor shield and initialize the motor speeds to not move
  shield.init();
  shield.setM1Speed(0);
  shield.setM2Speed(0);
  // Setup the debugging Serial communication channel
  Serial.begin(9600);
  Serial2.begin(115200);
}

// Main Function = JOURNEY
void loop() {
  // Initialize the variables
  int endType=0; // The type of junction that was identified
  int GNum=0;    // The number of the current game
  bool direc=0;  // Whether the "front" or the "other front" is the functional front
  bool justCrossed=0; // In case of 4-way junction
  
  while(GNum<4)
  {
    // Get to game branch
    Serial.println("To: game branch");
    while(endType<2)
    {
      endType=FollowLine(0,-ROBOSPEED);
      Serial.println(endType);
      Serial2.println(endType);
      if(endType<2)
        Turn(direc,(bool)endType);
    }
    // Turn onto the game branch
    if(endType==4-GNum%2)
    {
      Serial.println("J: Rolling");
      RollForwards(2);
    }
    else
    {
      Serial.println("J: Turning");
      Turn(0,GNum%2);
    }
    endType=-10;
    // Roll up game branch onto game square
    GameBranch:
    Serial.println("To: game square");
    while(endType<2)
    {
      endType=FollowLine(0,-ROBOSPEED);
      Serial.println(endType);
      Serial2.println(endType);
      if(endType<2)
        Turn(0,(bool)endType);
    }
    endType=-10;
    // Game Square
    Serial.println("Playing games");
    dockGame(GNum);
    playGame(GNum);
    undock(GNum);
    //direc^=1;
    // Travel back down the game branch towards the main road
    Serial.println("To: main road");
    while(endType<2)
    {
      endType=FollowLine(0,-ROBOSPEED);
      Serial.println(endType);
      Serial2.println(endType);
      if(endType<2)
        Turn(0,(bool)endType);
    }
    // Manage the main road junction
    if(endType==5 && justCrossed==0)
    {
      justCrossed=1;
      GNum++;
      Serial.println("J: 4Way Rolling");
      RollForwards(2);
      goto GameBranch;
    }
    if(endType==4-GNum%2)
    {
      Serial.println("J: Rolling");
      RollForwards(2);
    }
    else
    {
      Serial.println("J: Turning");
      Turn(0,GNum%2);
    }
    endType=-10;
    GNum++;
    justCrossed=0;
    Serial.println("--------------------- NEXT GAME ---------------------");
  }
  // Travel down the victory stretch of the main road
  Serial.println("To: Finish line");
  while(endType<2)
  {
    endType=FollowLine(0,-ROBOSPEED);
    Serial.println(endType);
    Serial2.println(endType);
    if(endType<2)
      Turn(0,(bool)endType);
  }
  
  // Infinite loop to ensure the robot does nothing after performance is complete
  while(true)
  {
    Serial.println("Finis");
    Serial2.println("Finis");
    delay(1000);
  }
}

// Dummy function for docking games : TO BE FILLED IN BY INTEGRATION
void dockGame(int Game)
{
  int Amount=0;
  // do something like a switch case specifying the amount the bot should roll forwards
  switch(Game)
  {
    case SIMON:
    Amount=2;
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
  RollForwards(Amount);
  return;
}

// Dummy function for enabling game-playing slave microcontrollers : TO BE FILLED IN BY INTEGRATION
void playGame(int Game)
{
  // do a switch case to send the enable signal to the slave microcontrollers to play the games
  int pin=0;
  int TimeCap=0;
  // do something like a switch case specifying the amount the bot should roll forwards
  switch(Game)
  {
    case SIMON:
    pin=22;
    TimeCap=15;
    break;
    case ETCHA:
    pin=23;
    TimeCap=10;
    break;
    case RUBIK:
    pin=21;
    TimeCap=5;
    break;
    case CARDS:
    pin=15;
    TimeCap=5;
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
void undock(int Game)
{
  // reverse the actions of "dockGame()" function. Just back out of the square until the back line follower is in contact with the road.
  
  // for this demonstration, RoPLiN will just turn 180 (since we don't have a back LF, so RoPLiN isn't reversible).
  Turn(0,0);
  return;
}

// Function to roll robot forward an arbitrary amount
void RollForwards(int Amount)
{
  // Turn motors on to move forwards
  shield.setM1Speed(-ROBOSPEED);
  shield.setM2Speed(-ROBOSPEED);
  // Wait a bit to give the robot time to roll
  delay(200*Amount);
  // Turn off motors
  shield.setM1Speed(0);
  shield.setM2Speed(0);  
  return;
}

// Executes a turn onto a new branch
void Turn(bool direc, bool Direction)
{
  // Declare variables
  int motorTarget=0;
  int curSpeed=0;
  //  Decide whether clockwise or counterclockwise is right/left 
  if((Direction^direc)==0)
  {
    curSpeed=-ROBOSPEED;
    motorTarget=-ROBOSPEED;
  }
  else
  {
    curSpeed=ROBOSPEED;
    motorTarget=ROBOSPEED;
  }
  // Keep on turning until current line is no longer seen
  bool* FLF;
  FLF=ReadFrontLF(0);
  while(FrontLine(FLF)==1)
  {
    // Read line follower
    free(FLF);
    FLF=ReadFrontLF(0);
    // Slowly ramp up turning speed to avoid jerkiness
    if(curSpeed>motorTarget)
      curSpeed--;
    if(curSpeed<motorTarget)
      curSpeed++;
    SetMotorsToRotate(curSpeed);
    // Delay a bit
    delay(10);
  }
  // Keep on turning until the new line appears
  while(FrontLine(FLF)==0)
  {
    // Read front line follower
    free(FLF);
    FLF=ReadFrontLF(0);
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
  free(FLF);
  
  // Use the PD Controller to center robot over the new line
 
  // Declare variables
  int Interest=200;
  int Time=0;
  float LastAngErr=9999; // Initialize to unrealistic value to recognize the first loop
  float AngErr=0;
  FLF=ReadFrontLF(0);
  // Measure the front LF's offset
  float FrOff=RawLFtoOffset(FLF);
  // Calculate the angular error (radians) and print
  AngErr=atan((FrOff-0)/125.0) * (180/3.141592); // Degrees
  while(Interest>0)
  {
    // Free the LF's Boolean arrays
    free(FLF);
    // Read the Line Followers
    FLF=ReadFrontLF(0);
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
  return;
}

int FollowLine(bool direc, int motorBase = -150)
{
  // Line follower arrays
  bool* MLF;
  bool* FLF;
  // Motor speed
  // Time index
  int Time=0;
  int JunCountD=-1;
  int JunctionType=0;
  // Debugging feature variables
  int Interest=0;
  // State variables
  float LastAngErr=9999; // Initialize to unrealistic value to recognize the first loop
  // Robot forwards motion motor speed
  int motorBaseN=motorBase*1.0; // "motor base now" = Base motor speed at the current time
  int mBNincer=0;               // "motor base now incrementer" = delay variable that accelerates robot at rate of 0.025V per 5ms
  // Junction recognition flags
  bool Lefto  = 0;
  bool Righto = 0;
  bool JunctionSighted=0;
  bool Fronto = 0;
  // Poll the LFs
  MLF=ReadMiddleLF(0);
  FLF=ReadFrontLF(0);
  // Keep on moving until the center LF has seen an anomaly and the middle LF is over an anomaly
  while(!(JunctionSighted==1 && (Anomalous(MLF,2)==1 || JunCountD==0)))
  //while(true)
  {
    // Slowly ramp up speed to goal speed (to avoid jerkiness)
    if(motorBaseN>motorBase)
    {
      mBNincer++;
      if(mBNincer>=5)
      {
        motorBaseN--;
        mBNincer=0;
      }
    }
    // Free the LF's Boolean arrays
    free(MLF);
    free(FLF);
    // Read the Line Followers
    MLF=ReadMiddleLF(0);
    FLF=ReadFrontLF(0);
    // Print the raw line follower reads for debugging

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
 
    // Measure the lateral error
    float LatErr=RawLFtoOffset(MLF);
    // Measure the front LF's offset
    float FrOff=RawLFtoOffset(FLF);
    // Calculate the angular error (radians) and print
    float AngErr=0;
    //AngErr=atan((FrOff-LatErr)/125.0) * (180/3.141592); // Degrees
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
    // Set the rotation speed
    int deltaMotor=0;
    // Manage the junction count down timer
    if(JunCountD>0)
    {
      JunCountD--;
    }
    // If the front line follower sees a junction take a snapshot and roll over it.
    if(Anomalous(FLF,3))
    {
      deltaMotor=0;
      // Cut speed
      if(JunctionSighted==0)
      {
        //motorBaseN=motorBase*1.0;
        JunCountD=100;
      // Snapshot: Recognize whether this anomaly has left or right branches
      if(FLF[0]==1 || FLF[1] == 1)
      {
        Lefto=1;
      }
      if(FLF[7]==1 || FLF[6] == 1)
      {
        Righto=1;
      } 
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
      // If the front LF sees no line, don't bother following it!(DUH!) Just keep rolling straight.
      if(FrontLine(FLF) == 0)
      {
        deltaMotor=0;
      }
      else // If no junctions are sighted and a line still exits, then do a normal line following PD Controller
      {
        // PD Control
        deltaMotor = (int)( 15*AngErr - 15*DAng );
      }
    }
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
    shield.setM1Speed(motorBaseN-deltaMotor);
    shield.setM2Speed(motorBaseN+deltaMotor);
    // Delay a bit
    delay(1);
    Time += 1;
  }
  // Brake roughly over the junction
  shield.setM1Speed(0);
  shield.setM2Speed(0);
  shield.setBrakes(400,400);
  // Identify the junction from the gathered data about other branches
  //Serial.print("LF: Left/Right/Front = ");
  if(FrontLine(FLF)==1)
  {
    Fronto=1;
  }
  /*
  Serial.print(Lefto);
  Serial.print("/");
  Serial.print(Righto);
  Serial.print("/");
  Serial.println(Fronto);
  */
  delay(1000);
  // Clear the speeds
  shield.setBrakes(0,0);
  // Clear the dynamically allocated line follower arrays
  free(MLF);
  free(FLF);
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
  for(i=0;i<8;i++)
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
  if(direc==1)
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
  if(direc==1)
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
  if(direc==1)
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
    
  for (t = 0; t<8; t++){
   comp = comp+Input[t];
  }

  //if(comp > 5 && (Input[0]==1 || Input[7]==1) ){
  if(comp > Thresh ){
     //printf("Anomalous. Detecting direction trend..."); //Detected Anomalous
     return(1);
  }
  if(Input[0]==1 || Input[7]==1)
  {
    return(1);
  }
  return(0);
}

// Checks whether any line exists on a line follower signature
bool FrontLine(bool* TopInput)
{
  int t = 0;
  int checkup = 0;
    
  for (t = 0; t<8; t++){
   checkup = checkup+TopInput[t];
  }

  if(checkup > 1){ //Checks the top Path Sensor
     //printf("Upwards Path Exists\n");
     return(1);
  }
  return(0);
}
