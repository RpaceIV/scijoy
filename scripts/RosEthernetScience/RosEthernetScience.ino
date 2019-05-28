/*
 * rosserial Publisher Example
 * Prints "hello world!"
 * This intend to connect to an Arduino Ethernet Shield
 * and a rosserial socket server.
 * You can launch the rosserial socket server with
 * roslaunch rosserial_server socket.launch
 * The default port is 11411
 *
 */
#include <SPI.h>
#include <Ethernet.h>

// To use the TCP version of rosserial_arduino
#define ROSSERIAL_ARDUINO_TCP

#include <ros.h>
#include <std_msgs/String.h>

// Set the shield settings
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 123);

// Set the rosserial socket server IP address
IPAddress server(192,168,1,2);
// Set the rosserial socket server port
const uint16_t serverPort = 11411;

ros::NodeHandle nh;
// Make a chatter publisher
std_msgs::String str_msg;
ros::Publisher chatter("scienceController", &str_msg);

// Subscriber stuff here
void messageCb( const std_msgs::String& val){
  Serial.println(val.data);   // blink the led
}

ros::Subscriber<std_msgs::String> sub("scienceAck", &messageCb );

// Be polite and say hello
char data[19] = "";
//char hello[16] = "0,0,0,0,0,0,0,0";
uint16_t period = 1000;
uint32_t last_time = 0;
String numSendString = "";
String numReturnString = "";
String digiPins = "";

int currentBit;
char binaryArray[28];
char *currentButton;
char *currentNum;
char *currentPot;

//Number scheme starts from left to right
//Potentiometers x4
int potPin[] = {A9,A11,A13,A15}; 

//=======/Wood Plate/========//
//Flick Switchs x10 
int flickSwitch[] = {A0,A14,A4,A6,A8,4,6,8,11,13}; 

//Feedback LED's red x10
int flickLed[] = {A1,A3,A5,A7,A10,3,5,7,9,12};

//=======/Angled Plate/========//
//Square Buttons x4 = {topLeftSquare,topRightSquare,bottomLeftSquare,bottomRightSquare}
int squareSwitch[] = {32,49,47,45};

//Feedback square LED's white x4
int squareLed[] = {50,48,46,44}; 

//Triangle Buttons x5 
int triangleSwitch[] = {35,37,39,41,43};

//Feedback triangle LED's yellow x5
int triangleLed[] = {34,36,38,40,42};

//Window Switches = {left dir, right dir} 
int rockerLeft[] = {30,28};
int rockerRight[] = {33,31};

//Joystick = {Up,Down,Left,Right}
int joy[] = {A12,14,A2,0};
                       
//Special Button 
int bigButton = 17;

//Status Led's
int powerOn = 16;
int rosConnection =15;

//RGB led strip
// int rgbStrip = 50;


void setup()
{
  // Use serial to monitor the process
  Serial.begin(115200);

  // Connect the Ethernet
  Ethernet.begin(mac, ip);

  // Let some time for the Ethernet Shield to be initialized
  delay(1000);

  Serial.println("");
  Serial.println("IP address: ");
  Serial.println(Ethernet.localIP());

  // Set the connection to rosserial socket server
  nh.getHardware()->setConnection(server, serverPort);
  nh.initNode();

  for(int i=0;i<10;i++){
    pinMode(flickSwitch[i], INPUT_PULLUP);
    pinMode(flickLed[i], OUTPUT);
  }
  for(int i=0;i<4;i++){
    pinMode(squareSwitch[i], INPUT_PULLUP);
    pinMode(squareLed[i], OUTPUT);
  }

  for(int i=0;i<5;i++){
    pinMode(triangleSwitch[i], INPUT_PULLUP);
    pinMode(triangleLed[i], OUTPUT);
  }
  for(int i=0;i<2;i++){
    pinMode(rockerLeft[i], INPUT_PULLUP);
    pinMode(rockerRight[i], INPUT_PULLUP);
  }
  for(int i=0;i<4;i++){
    pinMode(joy[i], INPUT_PULLUP);
  }

  pinMode(bigButton, INPUT);
  //pinMode(rgbStrip, OUTPUT);
  pinMode(powerOn, OUTPUT);
  pinMode(rosConnection, OUTPUT);

  // Start to be polite
  nh.advertise(chatter);
  nh.subscribe(sub);
}

String arrToInt(char s[],int arrSize){
    int value1 = 0;
    int value2 = 0;
    for (int i=0; i< strlen(s)/2; i++)  // for every character in the string  strlen(s) returns the length of a char array
    {
      value1 *= 2; // double the result so far
      if (s[i] == '1') value1++;  //add 1 if needed
    }
    
    for (int i=strlen(s)/2; i< strlen(s); i++)  // for every character in the string  strlen(s) returns the length of a char array
    {
      value2 *= 2; // double the result so far
      if (s[i] == '1') value2++;  //add 1 if needed
    }

    return String(value1)+"," + String(value2);
    
}


//int arrToInt(char arr[],int arrSize){
//  int k=0;
//  int sum=0;
//  while(k<arrSize--){
//    if(int(arr[arrSize]) == 1){
//      sum++;
//    }
//    sum = sum<<1;
//    k++;
//  }
//  if(int(arr[arrSize]) == 1){
//    sum++;
//  }
//  return sum;
//}

void intToArray(int arr[],int num,int arrSize){
  int i=arrSize--;
  int shift=0;
  while(i>=0){
    int bit = (num>>shift) & 1;
    if (bit == 1){
      arr[i] = 1;
    }else{
      arr[i] = 0;
    }
    i--;
    shift++;
  }
}

void readValArray(int arr[], int arrSize){
  for(int i=0;i<arrSize;i++){
    if(digitalRead(arr[i])==1){
      currentBit = 0;
    }else{
      currentBit = 1;
    }
//    Serial.println(currentBit);
//    *currentButton = (char)currentBit;
//    strcat(binaryArray,currentButton);
//    Serial.println(sizeof(binaryArray));
      digiPins.concat(currentBit);
  }
}

void updateLights(char binaryArr[],int ledArr[], int arrSize){
  int i;
  for(i = 0; i < (arrSize); i++){
          Serial.println(binaryArr[i]);
          Serial.println(ledArr[i]);
    if(binaryArr[i] == '1'){
      digitalWrite(ledArr[i], HIGH);
    }else{
      digitalWrite(ledArr[i], LOW);
    }
  }
}

void loop()
{
    digitalWrite(powerOn,HIGH);
    if (nh.connected())
    {
      digitalWrite(rosConnection,HIGH);


      //============/Writing to Serial for Python to send to ROS/============/
      //Digital
      readValArray(flickSwitch,10);
      readValArray(squareSwitch,4);
      readValArray(triangleSwitch,5);
      readValArray(rockerLeft,2);
      readValArray(rockerRight,2);
      readValArray(joy,4);

      digiPins.concat(digitalRead(bigButton));
      digiPins.toCharArray(binaryArray, 28);
      numSendString = arrToInt(binaryArray, 28);
//      for(int i=0;i<9; i++){
//        *currentNum = numSendString[i];
//        strcat(data,currentNum);
//      }
      Serial.println(binaryArray);
//      
//      
////      //Analog   
    for(int i=0;i<4;i++){
//        Serial.println(analogRead(potPin[i]));
        numSendString.concat(",");
        if(1020<analogRead(potPin[i])){
          numSendString.concat("A");    
        }else if(918<analogRead(potPin[i])){
          numSendString.concat("9");
        }else if(816<analogRead(potPin[i])){
          numSendString.concat("8");
        }else if(714<analogRead(potPin[i])){
          numSendString.concat("7");
        }else if(612<analogRead(potPin[i])){
          numSendString.concat("6");
        }else if(510<analogRead(potPin[i])){
          numSendString.concat("5");
        }else if(408<analogRead(potPin[i])){
          numSendString.concat("4");
        }else if(204<analogRead(potPin[i])){
          numSendString.concat("3");
        }else if(102<analogRead(potPin[i])){
          numSendString.concat("2");
        }else if(1<analogRead(potPin[i])){
          numSendString.concat("1");
        }else{
          numSendString.concat("0");
        }
      }
//      Serial.println(numSendString);
      
      numSendString.toCharArray(data, 19);
      str_msg.data = data;
      chatter.publish( &str_msg );
      digiPins = "";
    } else {
      digitalWrite(rosConnection,LOW);
      Serial.println("Not Connected");
    }
  nh.spinOnce();
  delay(10);

  //============/Reading Serial to see if ROS was working/============/

//  returnNum = som/ething[0];
  
//  intToArray(int arr[],returnNum,25);
//  updateLights(dummy,flickLed,10);
  

}
