
#include <Servo.h>
#define distantaMin 15
#define distantaMax 30
#define servMin 125 // cea mai mica panta
#define servMax 40 // cea mai mare panta
#define T 0.05 //perioada esantionare

Servo servoul; 
const int trigPin = 24;
const int echoPin = 22;
const int servoPin = 3; //pwm
const int ledPin = 26;

float durata;
float distanta,distanta_calc,distanta_calc_prev;

float eroare,referinta,u,u_con;

float KP,KI,KD,P,I,D,D_prev,distanta_prev;
unsigned long int loopTimer;


void Calc_dist()
{
  digitalWrite(trigPin,LOW);
  delayMicroseconds(20);
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);          //creating a pulse for sensing distance;
  durata = pulseIn(echoPin,HIGH);   //read the echoTime, &hence the distance;
  distanta_calc = 0.0347*durata*0.5;
  
}


  
void Calc_comanda()
{
  P=KP*eroare;
  
  I=KI*eroare*T+I;
  if (I<-40) I=-40;
  if (I>40) I =40; 
  D=KD*(-distanta+distanta_prev)/T;
  u=P+I+D;
  D=(D_prev+D)/2;
  D_prev=D;
}



void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(ledPin, OUTPUT); // Sets the trigPin as an Output

  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(9600); // Starts the serial communication
  pinMode(A0,INPUT);

  

  servoul.attach(3);  // attaches the servo on pin 9 to the servo object
  servoul.write(85);  
  delay(2000);  
  loopTimer = micros();
  //KD=2.4;
  //KI=0.8;
  //KP=5.2;
  KD=2.1;
  KI=3.8;
  KP=5.9;
  referinta=25;
}




void loop() {
  
  
  /////////////////////////////////////////

  
  referinta=map(analogRead(A0),0,1023,distantaMin,distantaMax);


  Calc_dist();
  
  distanta=0.7265*distanta + 0.1367*(distanta_calc+distanta_calc_prev);
  distanta_calc_prev=distanta_calc;

  eroare=-distanta+referinta;

  if (abs(eroare)>1 || abs(distanta - distanta_prev) >0.2){
    Calc_comanda();

    u+=85;
    if (u < 40) u_con=40;
    else if (u>130) u_con=130;
    else u_con=int(u);  
    //u_con=constrain(u+85,servMin,servMax);
      digitalWrite(ledPin,LOW);

  }
  else
    digitalWrite(ledPin,HIGH);

  servoul.write(u_con);

  distanta_prev=distanta;
  Serial.print("D:");
  Serial.print(distanta);
  Serial.print(" R:");
  Serial.print(referinta);
  Serial.print(" E:");
  Serial.print(eroare);
  Serial.print(" U:");
  Serial.print(u);
  Serial.print(" Uc:");
  Serial.print(u_con);
  Serial.print(" P:");
  Serial.print(P);
  Serial.print(" I:");
  Serial.print(I);
  Serial.print(" D:");
  Serial.println(D);




  while (micros() - loopTimer < T*1000000){};
  loopTimer = micros();
  
}
