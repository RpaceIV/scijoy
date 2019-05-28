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

void messageCb(const std_msgs::String& val){
  Serial.println(val.data);
}

ros::Publisher chatter("/scijoy", &str_msg);
ros::Subscriber<std_msgs::String> sub("/scijoy", &messageCb);

char data[16] = "";
uint16_t period = 1000;
uint32_t last_time = 0;
String numSendString = "";
String numReturnString = "";

char dummy[10]="1111111111";

int currentBit;
char binaryArray[25];
char *currentButton;
char *currentNum;
char *currentPot;

//Number scheme starts from left to right
//Potentiometers x4
int potPin[] = {A9,A11,A13,A15}; 

//=======/Wood Plate/========//
//Flick Switchs x10 
int flickSwitch[] = {A0,A2,A4,A6,53,4,6,8,11,13}; 

//Feedback LED's red x10
int flickLed[] = {A1,A3,A5,A7,52,3,5,7,9,12};

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
int joy[] = {0,14,1,2};

//Special Button 
int bigButton = 17;

//Status Led's
int powerOn = 16;
int rosConnection =15;

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
  nh.subscribe(sub);
  
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

  pinMode(bigButton, INPUT_PULLUP);
  pinMode(rgbStrip, OUTPUT);
  pinMode(powerOn, OUTPUT);
  pinMode(rosConnection, OUTPUT);

}

int arrToInt(char arr[],int arrSize){
  int k=0;
  int sum=0;
  while(k<arrSize--){
    if(int(arr[arrSize]) == 1){
      sum++;
    }
    sum = sum<<1;
    k++;
  }
  if(int(arr[arrSize]) == 1){
    sum++;
  }
  return sum;
}

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
    
    *currentButton = (char)currentBit;
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
  pinMode(powerOn,HIGH);
  pinMode(rosConnection,HIGH);
  //============/Writing to Serial for Python to send to ROS/============/
  //Digital
  readValArray(flickSwitch,10);
  readValArray(squareSwitch,4);
  readValArray(triangleSwitch,5);
  readValArray(rockerLeft,2);
  readValArray(rockerRight,2);
  readValArray(joy,4);
  
  *currentButton = (char)digitalRead(bigButton);
  strcat(binaryArray,currentButton);
  
  numSendString = String(arrToInt(binaryArray,25));
  for(int i=0;i<9; i++){
    *currentNum = numSendString[i];
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
      pinMode(rosConnection,HIGH);
    Serial.println(data);
    str_msg.data = data;
    chatter.publish( &str_msg );
  } else {
    Serial.println("Not Connected");
  }
  nh.spinOnce();
  delay(100);

  //============/Reading Serial to see if ROS was working/============/

//  returnNum = som/ething[0];
  
//  intToArray(int arr[],returnNum,25);
//  updateLights(dummy,flickLed,10);
  

}
