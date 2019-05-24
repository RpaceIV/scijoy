#include <SPI.h>
#include <Ethernet.h>

// To use the TCP version of rosserial_arduino
#define ROSSERIAL_ARDUINO_TCP

#include <ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Int8.h>

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

ros::Publisher chatter("/scijoy", &str_msg);
//ros::Subscriber<str_msg> sub("/scijoy", &str_msg);

char data[16] = "";
uint16_t period = 1000;
uint32_t last_time = 0;
String numString = "";
char dummy[10]="1111111111";

char binaryArray[25];
char *currentButton;
char *currentNum;
char *currentPot;

//Number scheme starts from left to right
//Potentiometers x4
int potPin[] = {A0,A1,A2,A3}; 

//=======/Wood Plate/========//
//Flick Switchs x10 
int flickSwitch[] = {51,2,3,4,5,6,7,8,9,52}; 

//Feedback LED's red x10
int flickLed[] = {11,12,13,14,15,16,17,18,19,20};

//=======/Angled Plate/========//
//Square Buttons x4 = {topLeftSquare,topRightSquare,bottomLeftSquare,bottomRightSquare}
int squareSwitch[] = {21,22,23,24};

//Feedback square LED's white x4
int squareLed[] = {25,26,27,28}; 

//Triangle Buttons x5 
int triangleSwitch[] = {29,30,31,32,33};

//Feedback triangle LED's yellow x5
int triangleLed[] = {34,35,36,37,38};

//Special Button 
int bigButton = 39;

//Joystick = {Up,Down,Left,Right}
int joy[] = {40,41,42,43};

//Status Led's
int powerOn = 44;
int rosConnection =45;

//Window Switches = {left dir, right dir} 
int windowLeft[] = {46,47};
int windowRight[] = {48,49};

//RGB led strip
int rgbStrip = 50;


void setup()
{  
  Serial.begin(115200);
  Serial2.begin(115200);
  
  Ethernet.begin(mac, ip);

  // Let some time for the Ethernet Shield to be initialized
  delay(1000);

  Serial.println("");
  Serial.println("Ethernet connected");
  Serial.println("IP address: ");
  Serial.println(Ethernet.localIP());

  // Set the connection to rosserial socket server
  nh.getHardware()->setConnection(server, serverPort);
  nh.initNode();

  // Another way to get IP
  Serial.print("IP = ");
  Serial.println(nh.getHardware()->getLocalIP());
  
  nh.advertise(chatter);
//  nh.subscribe(sub);
  
  for(int i=0;i<10;i++){
    pinMode(flickSwitch[i], INPUT);
    pinMode(flickLed[i], OUTPUT);
  }
  for(int i=0;i<4;i++){
    pinMode(squareSwitch[i], INPUT);
    pinMode(squareLed[i], OUTPUT);
  }
  for(int i=0;i<5;i++){
    pinMode(triangleSwitch[i], INPUT);
    pinMode(triangleLed[i], OUTPUT);
  }
  for(int i=0;i<2;i++){
    pinMode(windowLeft[i], INPUT);
    pinMode(windowRight[i], INPUT);
  }
  for(int i=0;i<4;i++){
    pinMode(joy[i], INPUT);
  }

  pinMode(bigButton, INPUT);
  pinMode(rgbStrip, OUTPUT);
  pinMode(powerOn, OUTPUT);
  pinMode(rosConnection, OUTPUT);

}

int arrToInt(char arr[],int arrSize){
  arrSize--;
  int sum=0;
  while(arrSize>0){
    if(int(arr[arrSize]) == 1){
      sum++;
    }
    sum = sum<<1;
    arrSize--;
  }
  if(int(arr[arrSize]) == 1){
    sum++;
  }
  return sum;
}

void intToArray(int arr[],int num,int arrSize){
  int i=0;
  while(i<arrSize){
    int bit = (num>>i) & 1;
    if (bit == 1){
      arr[i] = 1;
    }else{
      arr[i] = 0;
    }
    i++;
  }
}

void readValArray(int arr[], int arrSize){
  for(int i=0;i<arrSize;i++){
    *currentButton = (char)digitalRead(arr[i]);
    strcat(binaryArray,currentButton);
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
  Serial.println("hello");
  //============/Writing to Serial for Python to send to ROS/============/
  //Digital
  readValArray(flickSwitch,10);
  readValArray(squareSwitch,4);
  readValArray(triangleSwitch,5);
  readValArray(windowLeft,2);
  readValArray(windowRight,2);
  readValArray(joy,2);
  
  *currentButton = (char)digitalRead(bigButton);
  strcat(binaryArray,currentButton);
  
  numString = String(arrToInt(binaryArray,25));
  for(int i=0;i<9; i++){
    *currentNum = numString[i];
    strcat(data,currentNum);
  }
//  Serial.println(data);
  
  
  //Analog   
  for(int i=0;i<4;i++){
    strcat(data,',');
    if(1020<analogRead(potPin[i])){    
      strcat(data,'A');
    }else if(918<analogRead(potPin[i])){
      strcat(data,'9');
    }else if(816<analogRead(potPin[i])){
      strcat(data,'8');
    }else if(714<analogRead(potPin[i])){
      strcat(data,'7');
    }else if(612<analogRead(potPin[i])){
      strcat(data,'6');
    }else if(510<analogRead(potPin[i])){
      strcat(data,'5');    
    }else if(408<analogRead(potPin[i])){
      strcat(data,'4');
    }else if(204<analogRead(potPin[i])){
      strcat(data,'3');
    }else if(102<analogRead(potPin[i])){
      strcat(data,'2');
    }else if(1<analogRead(potPin[i])){
      strcat(data,'1');
    }else{
      strcat(data,'0');
    }
  }
  Serial.println(data);
  
  if (nh.connected()){
    Serial.println("Connected");
    str_msg.data = data;
    chatter.publish( &str_msg );
  } else {
    Serial.println("Not Connected");
  }
  nh.spinOnce();
  delay(100);

  //============/Reading Serial to see if ROS was working/============/
  updateLights(dummy,flickLed,10);
  

}
