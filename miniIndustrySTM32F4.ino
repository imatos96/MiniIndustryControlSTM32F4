  #include<Servo.h>
  #include "HX711.h"

  #define DOUT  12
  #define CLK  13
  
  HX711 scale(DOUT, CLK);
  
  //#define trigPin A0
  //#define echoPin A1

  //definiranje svih servo motora - ukupno 7
  Servo base;
  Servo elbow;
  Servo shoulder;
  Servo grip;
  Servo ejector;
  Servo conveyorBelt;
  Servo funnel;

  //definiranje pinova za serijsku komukaciju s Raspberry-jem
  HardwareSerial Serial1(PA10,PA9);
  String incoming="";
  String conditions="";
  bool stringComplete=false;
  int counter=0;

  // kutevi robotske ruke
  int angle = 10;
  int acceptedPositionAngle=175;
  int declinedPositionAngle=35;
  int basePosition=95;
  int ejectorAngle=10;
  
  int valBase = 95;
  int valShoulder = 90;
  int valElbow = 90;
  int valPinch = 130;

  
  //long duration;
  //int distanceCm;
  bool objectTaken=false;
  
  //vaga
 
  float calibration_factor =-465;
  float scaleUnits;
  int scaleScale;
  bool  trebaVaga;
  
  bool  startExecute=false;
  bool startConveyorBelt=false;


  //zastavice za sprjecavanje prolaska kroz pocetni dio koda nakon primanja podatka s kamere
  bool ejectorFlag=true;
  bool conveyorFlag=true;
  bool funnelFlag=true;
 
  int pickedObject=0;
  int temp=0;
  int pickAccepted=0;
  
  void setup() {
    Serial.begin(9600);
    Serial1.begin(9600);
    


   // pinMode(trigPin, OUTPUT);
    //pinMode(echoPin, INPUT);

    base.attach(3);
    shoulder.attach(6);
    elbow.attach(5);
    grip.attach(9);
    ejector.attach(10);
    funnel.attach(7);
    
    base.write(valBase);
    shoulder.write(valShoulder);
    elbow.write(valElbow);
    grip.write(valPinch);
    ejector.write(10);
    funnel.write(75);
    delay(100);

   
    scale.set_scale(calibration_factor);
    scale.tare(); 
    long zero_factor = scale.read_average();
  }
  
  void loop()
  {
    scaleUnits=scale.get_units();//dohvati podatke s vage -- prosjek 50 ocitavanja
    //Serial.print("masa: ");
    //Serial.println(scaleUnits);
    
     if (Serial1.available())
     {
      incoming = Serial1.readString();
    
      Serial.println("imam string");;
      Serial.println(incoming);
    //  Serial.println(inputString);
   //   Serial.println("poslano");
      for(int i=0;incoming[i]!='\0';i++)
      {
          counter++;
      }
      Serial.println(counter);
      if(counter==4&&incoming[0]=='E')
      {
        conditions=incoming;
        Serial.println(conditions);
        counter=0;
      }
      Serial.println(conditions[0]);

        if (incoming[0]=='E' && conditions[0]=='E')
        {
         // Serial.println(conditions[0]);
          scaleScale=0;
          trebaVaga=false;
          startExecute=true;
         }
        if(incoming[0]=='Q' || pickedObject==15) 
        {
          Serial.println("ja cu da zaustavim");
          scaleScale=0;
          trebaVaga=false;
          startExecute=false;
          pickedObject=0;
          conveyorBelt.detach();
        }
        
        
      
        if(conditions[3]=='1' || conditions[3]=='2' || conditions[3]=='3')
        {
          trebaVaga=true;
          Serial.println("yolo");
        }
        else if(conditions[3]='N')
        {
          trebaVaga=false;
        }
        if(counter!=4 && incoming!=conditions) 
        {
          //counterAnali++;
        /*  Serial.println(pickedObject+1);
          Serial.print(". oblik: ");
          
          if (inputString[0]=='1')  Serial.print("crvena, ");
          else if (inputString[0]=='2')  Serial.print("zelena, ");
          else if (inputString[0]=='3')  Serial.print("plava, ");
          else if (inputString[0]=='4')  Serial.print("žuta, ");
          else if (inputString[0]=='5')  Serial.print("nepoznato, ");
          if (inputString[1]=='1')  Serial.print("piramida, ");
          else if (inputString[1]=='2')  Serial.print("kugla, ");
          else if (inputString[1]=='3')  Serial.print("kocka, ");
          else if (inputString[1]=='4')  Serial.print("valjak, ");
          else if (inputString[1]=='5')  Serial.print("nepoznato, ");*/
         
          Serial.println(conditions[2]);
          Serial.println(incoming[1]);
          
          if(conditions[1]=='N' && conditions[2]=='N') //likovi su 1, 2 su boje
          {
             pickAccepted=1;
             Serial.println("ne treba boja ni lik");
             
          }
          else if (conditions[1]!='N' && conditions[2]=='N')//ako treba lik
          {
              if(incoming[0] = conditions[1])
            {
              pickAccepted=1;
            }
            else 
            {
              pickAccepted=2;
              Serial.println("mjesto 1");
            }
          }
           else if (conditions[1]=='N' && conditions[2]!='N') //ako treba boja
          {
              if(incoming[1] == conditions[2])
            {
              pickAccepted=1;
            }
            else 
            {
              pickAccepted=2;
              Serial.println("mjesto 2");
              
            }
          }
          else if (conditions[1]!='N' && conditions[2]!='N') //ako tebaju i lik i boja
          {
            Serial.println("usao sam u glupu petlju");
            if(incoming[1] == conditions[2] && incoming[0] == conditions[1])
            {
              pickAccepted=1;
            }
            else 
            {
              pickAccepted=2;
              Serial.println("mjesto 3");
            }
        }
        //startConveyorBelt=true;
        counter=0;
       } 
       incoming = "";
       stringComplete = false;  
      // Serial.println(startExecute);
      }


    //  Serial.println(startExecute);
      //proces
      if(startExecute==true)
      {
       //Serial.println("usao u execute");
      
       int i=0;
       if(((pickedObject==0) || (pickedObject==8 && pickAccepted==0)) && funnelFlag==true)//tresenje -- ne smije incoming jer se prazni odmah
        {
          Serial.println("tresem");
          while(i<5){
          slowMotion(45,85,funnel,1, 5);
          //Serial.println(i);
          i++;
          }
          funnelFlag=false;
        }
        Serial.println("pickedObject");
        Serial.println(pickedObject);
        Serial.println("temp");
        Serial.println(temp);
       // Serial.println(ejectorFlag);
       Serial.println("pickAccepted");
        Serial.println(pickAccepted);
        
       
        if(((pickedObject>temp) || (pickedObject==0 && temp==0 )) && pickedObject<16 && ejectorFlag==true ) //ako smo na pocetku ili ako je veci
        {
          Serial.println("bacam objekt");
          if(pickedObject%2==0 || pickedObject==0)
          {
            slowMotion(10,90,ejector,1, 15); //ulijevo
          }
          else if(pickedObject%2!=0)
          {
            slowMotion(10,90,ejector,0, 15);//udesno
          }
          int bytesSent = Serial1.write('1'); 
          //
          ejectorFlag=false;
          delay(2000);
        }
        //tresenje ako je picked Object 0 ili 8 znaci da je i ovaj mrdnuo objekt 8 puta -- RIJESENO
       //switch treba kao interrupt na vagu zaustaviti i kada dodje A ili D pokrenuti 
        
         //kad lijevo kad desno
        temp=pickedObject;


      if(conveyorFlag==true)
       {
         // Serial.println("radi traka");
          conveyorBelt.attach(4);
         // while((scale.get_units(10),3)<10)
          
            conveyorBelt.write(50);
         //Serial.println("scaleUnits");
        // Serial.println(scaleUnits);
         if((scaleUnits)>10.00) 
         {
          conveyorBelt.detach();
          startConveyorBelt=false;
          Serial.println("traka stala");
          conveyorFlag=false;
          if(pickAccepted==0)
          {
            pickAccepted=2;
            Serial.println("mjesto 4");
          }
         }

         
       }
           
            
            //serIn=Serial.read(); -- ovo mozda ne treba jer je vec u if-petlji
          if(pickAccepted!=0 && conveyorFlag==false)
          {
            Serial.println("trebam pokupiti nekog");
            //scale.tare();
            //Serial.print(scale.get_units(), 3);
           
            //scaleUnits=(scale.get_units(50),3);//dohvati podatke s vage -- prosjek 50 ocitavanja
            //Serial.print("masa: ");
           // Serial.print(scaleUnits);
            if(trebaVaga) 
            {
              if(10<scaleUnits<20) // ide redom i mjeri unutar kojeg ranga je izmjereni objekt
              {
                if(conditions[3]=='1')
                {
                  scaleScale=1; //prvi rang
                }
              }
              else if(20<scaleUnits<27)
              {
                if(conditions[3]=='2')
                {
                  scaleScale=2;//drugi rang
                }
              }
              else if(27<scaleUnits<40)
              {
                if(conditions[3]=='3')
                {
                  scaleScale=3;//treći rang
                }
              }
            }
            else scaleScale=0;
            if(pickAccepted==1) //salje ako je prihvaceno i ako treba samo vaga
            {
              
              if(trebaVaga==true && scaleScale!=0)
              {
                pick();
                acceptedPosition();
                home(basePosition, acceptedPositionAngle-5, 0);
                pickedObject++;
                pickAccepted=0;
               }
               else if(trebaVaga== true && scaleScale==0)
               {
                pick();
                declinedPosition();
                home(declinedPositionAngle, basePosition+5,1);
                pickedObject++;
                pickAccepted=0;
               }
               
               else if (trebaVaga==false)
               {
                pick();
                acceptedPosition();
                home(basePosition, acceptedPositionAngle-5, 0);
                pickedObject++;
                pickAccepted=0;
               }
            }
            else if(pickAccepted==2)
            {
              pick();
              declinedPosition();
              home(declinedPositionAngle, basePosition+5,1);
              pickedObject++;
              pickAccepted=0;
              
            }
            pickAccepted=0;
            ejectorFlag=true;
            if(pickedObject==8) funnelFlag=true;
            conveyorFlag=true;
          }
      
   }
  }
  
 /* void serialEvent() {
    if (Serial1.available()) {
        incoming = Serial1.readString();
        stringComplete = true;
        Serial.println("usaao");
        Serial.println(incoming);
       // Serial1.flush();
        
      }
    }
  */
   
  
   void pick()
  {
    //Serial.print(scale.get_units(), 3);
    scaleUnits=scale.get_units();
    while(scaleUnits>10)
    {
    slowMotion(120,185, grip, 1, 15);
    delay(200);
    slowMotion(50,100, shoulder, 0, 15);
    delay(200);
    slowMotion(90,100, elbow, 1, 15);
    delay(200);
    slowMotion(120,185, grip, 0, 15);
    delay(200);
    slowMotion(90,100, elbow, 0, 15);
    delay(200);
    slowMotion(50,100, shoulder, 1, 15);
    scaleUnits=scale.get_units();
    delay(1000);
    }
    
   }
  
  void home(int minAngle,int maxAngle, bool directionAngle)
  {
    slowMotion(minAngle,maxAngle, base, directionAngle, 15);
    delay(200);
    elbow.write(95);
    delay(200);
    shoulder.write(90);
    delay(200);
    grip.write(120);
    delay(500);
  }
  void acceptedPosition()
  {
   // delay(2000);
    slowMotion(90,acceptedPositionAngle, base, 1, 15);
    delay(200);
    slowMotion(50,100, shoulder, 0, 15);
    delay(200);
    slowMotion(90,115, elbow, 1, 15);
    delay(200);
    slowMotion(120,185, grip, 1, 15);
    delay(200);
    slowMotion(90,115, elbow, 0, 15);
    delay(200);
    slowMotion(50,100, shoulder, 1, 15);
    delay(200);
    grip.write(120);
    
    //acceptedPositionAngle-=3;
  }
  void declinedPosition()
   {
   // delay(2000);
    slowMotion(declinedPositionAngle,90, base, 0, 15);
    delay(200);
    slowMotion(50,100, shoulder, 0, 15);
    delay(200);
    slowMotion(90,115, elbow, 1, 15);
    delay(200);
    slowMotion(120,185, grip, 1, 15);
    delay(200);
    slowMotion(90,115, elbow, 0, 15);
    delay(200);
    slowMotion(50,100, shoulder, 1, 15);
    delay(200);
    grip.write(120);
    
   // declinedPositionAngle+=3;
   }
  
 void slowMotion(int min, int max, Servo servo, bool direction, int timeDelay)
  {
    if(direction==1)
    {
       for(angle = min; angle < max; angle++)  
      {                                  
        servo.write(angle);               
        delay(timeDelay);                   
      }   
    }
    else if (direction==0)
    {
       for(angle = max; angle > min; angle--)    
      {                                
        servo.write(angle);           
        delay(timeDelay);       
      } 
    }
  }
 
  

 
  // ne treba -- provjerava se vaga u pick-u
  /*int ultrasonic()
  {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distanceCm= duration*0.034/2;
    return distanceCm;
  }*/ 
 
