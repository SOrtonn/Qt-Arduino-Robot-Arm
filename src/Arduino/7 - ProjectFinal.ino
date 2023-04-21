#include <Braccio.h> //Libraries to handle servo control & Braccios custom functions
#include <Servo.h>  //servo library is used in the braccio edition

Servo base;     //Declare each axis for movement 
Servo shoulder; //also gives each servo a name
Servo elbow;
Servo wrist_ver;
Servo wrist_rot;
Servo gripper;
int basicBase, basicShoulder, basicElbow, basicWristVer, basicWristRot, basicGrip;

#define S0 2   //colour sensor definitions
#define S1 13
#define S2 4      
#define S3 7
#define sensorOut 8
#define LED A0 //LED's connected to pin A0

int redfrequency = 0;
int greenfrequency = 0;
int bluefrequency = 0;

void setup() {
//setup func

  pinMode(S0, OUTPUT); //pin setups for colour sensor
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);

  Serial.begin(9600); //9600 baude rate MUST MATCH OTHER PROGRAM FOR SUCCESFUL COMMUNICATION
  Braccio.begin();//loads the robot arm
  basicPosition();//puts arm into default poisition
}

void loop() {
  // this function permanently loops so is the control function (main func)
  if(Serial.available())
  {//if there is data available to read

    char inputCommand = Serial.read(); //reads one character (therefore all commands are done using individual chars)
    int inputDegrees = Serial.parseInt(); //reads data from the first nuber

    //Serial.print("\ndeg recieved: ");
    //Serial.print(inputDegrees);

    //Serial.print("\ncommand recieved: ");
    //Serial.print(inputCommand);

    armInstruction(inputCommand, inputDegrees); //passes read data to the instruction func
  }    
}
void basicPosition()
{
  
  Braccio.ServoMovement(20, 90, 0, 180, 180, 0, 70); //Default arm position
  basicBase = 90;
  basicShoulder = 0;
  basicElbow = 180;
  basicWristVer = 180;
  basicWristRot = 0;
  basicGrip = 180;
  return;
}
void moveToGreen()
{ //hard coded location for green circle
  
  Braccio.ServoMovement(20, 165, 90, 130, 170, 180, 70); //lean back
  Braccio.ServoMovement(20, 40, 45, 140, 180, 180, 70); //rotate
  delay(100);
  Braccio.ServoMovement(20, 40, 110, 160, 155, 60, 70); //place
  Braccio.ServoMovement(20, 50, 110, 160, 155, 60, 10);
  return;
}
void moveToBlue()
{//hard coded location for blue circle
  
  Braccio.ServoMovement(20, 165, 90, 130, 170, 180, 70); //lean back
  Braccio.ServoMovement(20, 80, 45, 140, 180, 180, 70); //rotate
  delay(100);
  Braccio.ServoMovement(20, 80, 110, 160, 145, 60, 70); //place
  Braccio.ServoMovement(20, 90, 110, 160, 145, 60, 10);
  return;
}
void moveToRed()
{//hard coded location for red circle
  
  Braccio.ServoMovement(20, 165, 90, 130, 170, 180, 70); //lean back
  Braccio.ServoMovement(20, 120, 45, 140, 180, 180, 70); //rotate
  delay(100);
  Braccio.ServoMovement(20, 120, 105, 170, 140, 60, 70); // place
  Braccio.ServoMovement(20, 130, 105, 170, 140, 60, 10);
  return;
}
void armInstruction(char inputCommand, int inputDegrees)
{
 /*USE FOR DEBUGGING DATA RECIEVED FROM GUI
  Serial.print("\ndeg recieved IN Func: ");
  Serial.print(inputDegrees);

  Serial.print("\ncommand recieved IN Func: ");
  Serial.print(inputCommand);
*/
  switch(inputCommand)
  {//Large switch statement to determine what to do from each recieved instruction
    case 'g'://move gripper
      Braccio.ServoMovement(20, basicBase, basicShoulder, basicElbow, basicWristVer, basicWristRot, inputDegrees);
      basicGrip = inputDegrees;
      break;
    case 'r'://rotate wrist
      Braccio.ServoMovement(20, basicBase, basicShoulder, basicElbow, basicWristVer, inputDegrees, basicGrip);
      basicWristRot = inputDegrees;
      break;
    case 'v'://translate wrist
      Braccio.ServoMovement(20, basicBase, basicShoulder, basicElbow, inputDegrees, basicWristRot, basicGrip);
      basicWristVer = inputDegrees;
      break;
    case 'e'://translate elbow
      Braccio.ServoMovement(20, basicBase, basicShoulder, inputDegrees, basicWristVer, basicWristRot, basicGrip);
      basicElbow = inputDegrees;
      break;
    case 's'://translate shoulder
      Braccio.ServoMovement(20, basicBase, inputDegrees, basicElbow, basicWristVer, basicWristRot, basicGrip);
      basicShoulder = inputDegrees;
      break;
    case 'b'://translate base
      Braccio.ServoMovement(20, inputDegrees, basicShoulder, basicElbow, basicWristVer, basicWristRot, basicGrip);
      basicBase = inputDegrees;
      break;
    case 'x'://'x' used to cancel movements/default input move arm to basic position. CAN't use a default block as serial comms can send false data
      basicPosition();
      break;
    case 'l'://automatic colour sorting
      colourSort();
      break;
    case 'k'://automatic follow sequence
      sequence();
      basicPosition();
      break;
    default:
      //No code here as serial.read can send bogus instructions that would result in this being called ;)
      break;
  }
   return;
}
void colourSort()
{//function to control the automatic colour sorting mode
  char stopCommand = 'o';
  while(stopCommand != 'x')//loops until data sends stop command 'x'
  {
    delay(300);//delay to give sensor time to read
    digitalWrite(LED,HIGH);//turn colour sensor LEDS on
    basicPosition();//move arm to default pos
    char colour = findColour();//calls the colour sensor sensing function and returns r/g/b
    pickNplace(colour);//calls the pickup and move function passing colour as an argument
    stopCommand = Serial.read();//check users input
  }
  Serial.flush();//flush the serial line for any stuck messages
  digitalWrite(LED,LOW);//turn leds off
  return;
}
void pickItem()
{//function to pickup item from colour sensor
    Braccio.ServoMovement(20, 165, 130, 100, 180, 180, 0);
    Braccio.ServoMovement(20, 165, 130, 140, 180, 180, 0); // move to location
    Braccio.ServoMovement(20, 165, 130, 140, 180, 180, 70); //grab item
    Braccio.ServoMovement(20, 165, 100, 140, 180, 180, 70); //lift item up
    delay(100);
}
void pickNplace(char colour)
{//function to control arm movements in automatic mode
  
  if(colour == 'x')
  {//no colour = do nothing

  }
  else
  {
    Serial.print(colour); //output colour detected
    Serial.flush(); //force data thru
    delay(100);

    pickItem(); //pickup from colour sensor
    
    switch(colour)
    {//determine where the block needs to be dropped off
      case 'r':
        moveToRed();
        delay(100); //delays to give arm time to complete current task
        break;
      case 'g':
        moveToGreen();
        delay(100);
        break;
      case 'b':
        moveToBlue();
        delay(100);
        break;
    }
  }
  
 // Braccio.ServoMovement(20, 0, 115, 145, 170, 70, 70); // location to move too
 // delay(100);
 // Braccio.ServoMovement(20, 0, 115, 145, 170, 70, 10);
  basicPosition(); //go back to default position
  Serial.print("x"); //go back to no colour being read
  return;
}
void sequence()
{//fixed sequence automatic mode 
  char colour = 'x';
  char stopCommand = 'o';
  while(stopCommand != 'x')
  {
    digitalWrite(LED,HIGH);
    basicPosition();

    colour = findColour();
    
    if(colour == 'r')
    {//when first colour is found, assume other colour will be there after and complete sequence (to increase efficiency)
      Serial.print(colour);
      pickItem();
      moveToRed();
      delay(100);
      Serial.print("g");
      pickItem();
      moveToGreen();
      delay(100);
      Serial.print("b");
      pickItem();
      moveToBlue();
    }
    Serial.print("x");
    stopCommand = Serial.read();
  }
  digitalWrite(LED,LOW);//turn leds off
  return;

}
char findColour()
{//use colour sensor to determine colour
  //char colour[3];
  char colour;
  int i = 0;
  
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  // Reading the output frequency
  redfrequency = pulseIn(sensorOut, LOW);

  while(i < 3)
  {
    delay(100);
    // Setting Green filtered photodiodes to be read
    digitalWrite(S2,HIGH);
    digitalWrite(S3,HIGH);
    // Reading the output frequency
    greenfrequency = pulseIn(sensorOut, LOW);
    // Printing the value on the serial monitor
 
    delay(100);
    // Setting Blue filtered photodiodes to be read
    digitalWrite(S2,LOW);
    digitalWrite(S3,HIGH);
    // Reading the output frequency
    bluefrequency = pulseIn(sensorOut, LOW);
    // Printing the value on the serial line
    if (redfrequency < 60 && redfrequency < greenfrequency && redfrequency < bluefrequency && greenfrequency > 100 && bluefrequency < 120)
    {//sets the colour varaible if the returned values match a colour requirement
      colour = 'r'; 
     // Serial.print("\n");
      //Serial.print(colour);
      //Serial.print(",");
    //  Serial.flush();
    }
    else if (greenfrequency > 70 && greenfrequency < 85 && bluefrequency < 60)
    {
      colour = 'b';
      //Serial.print("\n");
      //Serial.print(colour);
      //Serial.print(",");
      //Serial.flush();
    } 
    else if (greenfrequency > 80 && greenfrequency < 110 && bluefrequency > 60 && redfrequency > 85)
    {
      colour = 'g';
      //Serial.print("\n");
      //Serial.print(colour);
      //Serial.print(",");
      //Serial.flush();
    }
    else
    {
      colour = 'x';
      //Serial.print("\n");
      //Serial.print(colour);
      //Serial.print(",");
      //Serial.flush();
    }
   
   //Serial.print("\n");
   //Serial.print(redfrequency);
   //Serial.print("\n");
   //Serial.print(greenfrequency);
   //Serial.print("\n");
   //Serial.print(bluefrequency);
   i = i + 1;
  }
  return colour;

}