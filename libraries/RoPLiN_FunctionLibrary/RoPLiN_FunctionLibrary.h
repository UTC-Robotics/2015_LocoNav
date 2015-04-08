#ifndef RoPLin_FunctionLibrary_h
#define RoPLin_FunctionLibrary_h

// Function prototypes
int FollowLine(bool, int);                // Follow a line until a junction/turn
void Parallelize(bool, int);              // Parallel Parking-ish
void BackParallelPark(bool, int, int);    // Follow a line backwards for a bit to line up
void SetMotorsToRotate(int Speed);        // Starts the robot rotating
bool Anomalous(bool* , int);              // Judges whether a junction/turn is present on the line follower array
											// & identifies when a junction is particularly "fat"
bool FrontLine(bool* );                   // Judges whether a line exists on the line follower array
void Turn(bool, bool);                    // Makes the robot rotate left/right until it sees a line and then rolls 
											// forward a bit off of the junction/turn
void JuggleBoolArray(bool*);              // Flipflops the boolean array 
void RollForwards(bool, int);             // Rolls forward an arbitrary amount
void LiftMotor(float);                    // Move the lift motor up or down

// Unwritten dummy functions
void dockGame(bool, int);
void playGame(int);
void undock(bool, int);

#endif 