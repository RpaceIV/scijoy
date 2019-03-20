
int setPoint = 55;
String readString;

//pot 1-3
char data[80];
char outstr[15];
char outstr2[15];

int buttonPin = 2;  
int potPin = A0; 

void setup()
{
  pinMode(buttonPin, INPUT);
  Serial.begin(115200);  // initialize serial communications at 9600 bps

}

void loop()
{
  // serial read section
  char *pot1 = dtostrf((analogRead(potPin)),0,0,outstr);
  char *button1 = dtostrf((digitalRead(buttonPin)),0,0,outstr2);
  
  
  strcpy(data,"S");
  strcat(data,pot1);
  strcat(data,",");
  strcat(data,button1);
//  strcat(data,",");
  strcat(data,"E");
 
  Serial.println(data);
  
  delay(20);
}
