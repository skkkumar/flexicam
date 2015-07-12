#include <Servo.h> 

#define NUM_SERVOS 4

//collection of servo objects
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo motors [NUM_SERVOS] = {servo1, servo2, servo3, servo4};  // create servo object to control a servo 
                   // a maximum of eight servo objects can be created 
//the pins to which the motors are attached
int pinValues [NUM_SERVOS] = {4, 5, 6, 7};
//initial configuration of the motors
int firstValues [NUM_SERVOS] = {40, 90, 90, 90};
//communication variables
boolean startMessage = true;
int finalValues [NUM_SERVOS];
int message = 0;
int messagekey = 0;
int messagepart = 1;


void setup() {
        Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
        
        for(int i = 0 ; i < 1; i++){   //NUM_SERVOS
            //attach the servos to the pin
            motors[i].attach(pinValues[i]);
            delay(50);
            //keep the default position
            //motors[i].write(firstValues[i]);
            moveMotor(motors[i], firstValues[i], false);
            //initialize the final values of the servo
            finalValues[i] = -1;
        }     
}

//movees motors either 1) by one step 2) continuously in a loop
//returns true when the work is done
//sero is the servo object reference
//finalpos is the final position to which the motor has to be moved
//single is true if one step has to be executed else false for one continuous loop
boolean moveMotor(Servo servo, int finalpos, boolean single){
  
    //check if final value is achieved
     int currentpos = servo.read();
     if (currentpos == finalpos)
           return true;
           
     Serial.print("Current position:"); 
     Serial.println(currentpos); 
     
     Serial.print("Final position:"); 
     Serial.println(finalpos); 
     
     int stepsize = 1;
     if(finalpos < currentpos)
          stepsize = -1;
     
     //now move the motor
     if(single){
          servo.write(currentpos + stepsize); 
          delay(50);   
          return false;
     }
     else{
          while(currentpos != finalpos){
              currentpos = currentpos + stepsize;
              Serial.print("Current position:"); 
              Serial.println(currentpos); 
              servo.write(currentpos);              // tell servo to go to position in variable 'pos' 
              delay(50);
          }
          return true;
     }
}

//moves the selected motors step by step
void moveMotors(){
    for(int i = 0 ; i < NUM_SERVOS; i++){
           if(finalValues[i]!= -1){
                 boolean workdone = moveMotor(motors[i], finalValues[i], true);
                 if (workdone)
                       finalValues[i] = -1;
           }
     }  
}

//read bytes from the serial port
//the format of the message is "motor_index value,"
//motor_index is 1 based
void readByte(){
      if (Serial.available() > 0) {
            // read the incoming byte:
            int incomingByte = Serial.read(); 
            Serial.print("----------------- ");
            Serial.println(incomingByte);
                
            //for message end
            if (incomingByte == ','){
              
                 Serial.print("message end: \n"); 
                 
                 if(messagepart %2 == 0){
                     //assign the value to the key
                     //check if the key is valid
                     if (messagekey > 0 && messagekey <= NUM_SERVOS){
                          finalValues[messagekey - 1] = message;
                           Serial.print("motor key:  ");
                           Serial.print( messagekey); 
                           Serial.print(",motor value:  ");
                           Serial.println( message);     
                     }
                 }
                 
                 //reinitialize the system
                 message = 0;
                  messagekey = 0;
                  messagepart = 1;                
            }
            //for messagekey and message value separator
            else if(incomingByte == ' '){ 
                   messagekey = message;
                   messagepart = messagepart + 1;
                   message = 0; 
           }
           //for other numeric message values
           else{     
                message = message * 10 + (incomingByte-48);
           }
      }          
}


void loop() {
     //readByte();     
     //moveMotors();  
}
