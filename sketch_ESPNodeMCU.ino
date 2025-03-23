#include <ESP8266WiFi.h>

const char* ssid = "TPLink";
const char* password = "";

int Pin5 = 5; //D1 gpio5
int Pin6 = 4; //D2 gpio4
int SensorMotionPin = 12; //D6 gpio12
int SensorilluminationPin = 17; // A0 gpio17
int SensorMotVal = 0; //отримання 0 або 1 з датчика руху
int SensorilluminationValue = 0;

//Змінні для таймера
byte SEC = 0;
byte MIN = 0;
byte HOUR = 0; 
byte g = 0;
byte h = 0;
byte c = 0;
boolean timer = false;
boolean pauseTime = false;
boolean sensorMotion = false;
boolean illuminationSensor = false;
unsigned long prMillis = 0;
String valueAction = "";
String valueLedM = "";
String valueLedI = "";

int value = 0;
String request = "";
String response = "";

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);      

  // prepare GPIOs
  pinMode(Pin5, OUTPUT);
  digitalWrite(Pin5, HIGH);
  
  pinMode(Pin6, OUTPUT);
  digitalWrite(Pin6, HIGH);

  pinMode(SensorMotionPin, INPUT);
 
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {

  // Check if a client has connected
  WiFiClient client = server.available();
  if (timer == false && sensorMotion == false && illuminationSensor ==  false) {
    if (!client) {
      return;
    }   
    // Wait until the client sends some data
    Serial.println("new client");
    while(!client.available()){
      delay(1);
    } 
  } 

  // Read the first line of the request
  if (client.available()) {
    request = client.readStringUntil('\r');
    Serial.println(request);
  }
  client.flush();
  delay(250);
// Match the request
//============ON-OFF PINS========== 
  if (request.indexOf("pin=5")!= -1) {
        digitalWrite(Pin5, !digitalRead(Pin5));
        value = digitalRead(Pin5);
        response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n D5 is now ";
        Serial.println("PIN5 DONE!");
      } 
      
  if (request.indexOf("pin=6")!= -1) {
        digitalWrite(Pin6, !digitalRead(Pin6));
        value = digitalRead(Pin6);
        response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n D6 is now ";
      }
   
//============SM==========    
   if (request.indexOf("smot=1")!= -1) {
      sensorMotion = true; 
      valueLedM = request.substring(18, 20);
   }
   
   if (request.indexOf("smot=0")!= -1) {
      sensorMotion = false;
   }

    if (sensorMotion == true && illuminationSensor == false) {
       SensorMotVal = digitalRead(SensorMotionPin);   
       if (SensorMotVal == 1 && valueLedM == "D5") {
          digitalWrite(Pin5, LOW); // HIGH
        } 
       if (SensorMotVal == 0 && valueLedM == "D5") {
          digitalWrite(Pin5, HIGH); //LOW
        }  
       if (SensorMotVal == 1 && valueLedM == "D6") {
          digitalWrite(Pin6, LOW); //HIGH
        }
       if (SensorMotVal == 0 && valueLedM == "D6") {
          digitalWrite(Pin6, HIGH); //LOW
        } 
       if (SensorMotVal == 1 && valueLedM == "DD") {
          digitalWrite(Pin5, LOW); //HIGH
          digitalWrite(Pin6, LOW); //HIGH
        }
       if (SensorMotVal == 0 && valueLedM == "DD") {
          digitalWrite(Pin5, HIGH); //LOW
          digitalWrite(Pin6, HIGH); //LOW
        }        
    }
    
//============ILS==========  
  if (request.indexOf("ils=1")!= -1) {
      illuminationSensor = true;
      valueLedI = request.substring(17, 19);
   }
   
   if (request.indexOf("ils=0")!= -1) {
      illuminationSensor = false;
   }

   if (illuminationSensor == true && sensorMotion == false) {
      SensorilluminationValue = analogRead(SensorilluminationPin);
      Serial.println(SensorilluminationValue);
      if (SensorilluminationValue >= 900) {
       if (valueLedI == "D5") {
          digitalWrite(Pin5, LOW); 
        }  
       if (valueLedI == "D6") {
          digitalWrite(Pin6, LOW); 
        } 
       if (valueLedI == "DD") {
          digitalWrite(Pin5, LOW);
          digitalWrite(Pin6, LOW); 
        }
      } 
      
      if (SensorilluminationValue <= 750) {
        if (valueLedI == "D5") {
          digitalWrite(Pin5, HIGH); 
        }  
       if (valueLedI == "D6") {
          digitalWrite(Pin6, HIGH); 
        } 
       if (valueLedI == "DD") {
          digitalWrite(Pin5, HIGH);
          digitalWrite(Pin6, HIGH); 
        }        
      }
    }

   //============SM+ILS==========  
   if (sensorMotion == true && illuminationSensor == true) {
       SensorMotVal = digitalRead(SensorMotionPin);  
       SensorilluminationValue = analogRead(SensorilluminationPin);
       if (SensorilluminationValue >= 900) { 
         if (SensorMotVal == 1 && valueLedM == "D5") {
            digitalWrite(Pin5, LOW); // HIGH
          } 
         if (SensorMotVal == 0 && valueLedM == "D5") {
            digitalWrite(Pin5, HIGH); //LOW
          }  
         if (SensorMotVal == 1 && valueLedM == "D6") {
            digitalWrite(Pin6, LOW); //HIGH
          }
         if (SensorMotVal == 0 && valueLedM == "D6") {
            digitalWrite(Pin6, HIGH); //LOW
          } 
         if (SensorMotVal == 1 && valueLedM == "DD") {
            digitalWrite(Pin5, LOW); //HIGH
            digitalWrite(Pin6, LOW); //HIGH
          }
         if (SensorMotVal == 0 && valueLedM == "DD") {
            digitalWrite(Pin5, HIGH); //LOW
            digitalWrite(Pin6, HIGH); //LOW
          } 
       }       
    }  
 //============Timer========== 
  //Start the timer
  if (request.indexOf("/tim=st")!= -1) {
    if(pauseTime == true) {
        timer = true;
        pauseTime = false;
      } 
      
    if(pauseTime == false) {
        g = request.substring(18, 20).toInt();
        h = request.substring(21, 23).toInt();
        c = request.substring(24, 26).toInt();
            
        valueAction = request.substring(26, 31); 
        Serial.println(valueAction);  
        timer = true;
        Serial.println("Timer started");
      }
      request = "";
  }

  //Stop the timer
  if (request.indexOf("/tim=sp")!= -1) {
      SEC = 0;
      MIN = 0;
      HOUR = 0;
      g = 0;
      h = 0;
      c = 0;
      request = "";
      Serial.println("Timer stopped");
      timer = false;
  }

  //Pause the timer
  if (request.indexOf("/tim=ps")!= -1) {
      timer = false;
      pauseTime = true;
      request = "";
      Serial.println("Timer paused");
  }
  
  if (timer == true) {
      if ((HOUR == g) && (MIN == h) && (SEC == c)) {

        //D5 and D6 now is auto
          if (valueAction == "/DD/A") {
              digitalWrite(Pin5, !digitalRead(Pin5));
              digitalWrite(Pin6, !digitalRead(Pin6));
              Serial.println("/DD/A DONE!");
           }
           
        //D5 and D6 now is ON
           if (valueAction == "/DD/N") {
              digitalWrite(Pin5, LOW); //HIGH
              digitalWrite(Pin6, LOW); //HIGH
              g = 0;
              h = 0;
              c = 0;
              
              request = "";
              value = digitalRead(Pin5);
              timer = false;
              Serial.println("/DD/N DONE!");
           }

         //D5 and D6 now is OFF 
           if (valueAction == "/DD/F") {
              digitalWrite(Pin5, HIGH);
              digitalWrite(Pin6, HIGH);
              g = 0;
              h = 0;
              c = 0;
              
              request = "";
              value = digitalRead(Pin5);
              timer = false;
              Serial.println("/DD/F DONE!");
           }

           //D5 now is auto 
           if (valueAction == "/D5/A") {
              digitalWrite(Pin5, !digitalRead(Pin5));
              value = digitalRead(Pin5);
              Serial.println("/D5/A DONE!");
           }

           //D5 now is ON 
           if (valueAction == "/D5/N") {
              digitalWrite(Pin5, LOW);
              g = 0;
              h = 0;
              c = 0;
              
              request = "";
              value = digitalRead(Pin5);
              timer = false;
              Serial.println("/D5/N DONE!");
           }

           //D5 now is OFF 
           if (valueAction == "/D5/F") {
              digitalWrite(Pin5, HIGH);
              g = 0;
              h = 0;
              c = 0;

              request = "";
              value = digitalRead(Pin5);
              timer = false;
              Serial.println("/D5/F DONE!");
           }

           //D6 now is auto 
           if (valueAction == "/D6/A") {
              digitalWrite(Pin6, !digitalRead(Pin6));
              value = digitalRead(Pin6);
              Serial.println("/D6/A DONE!");
           }

           //D6 now is ON 
           if (valueAction == "/D6/N") {
              digitalWrite(Pin6, LOW);
              g = 0;
              h = 0;
              c = 0;
              
              request = ""; 
              value = digitalRead(Pin6);
              timer = false;
              Serial.println("/D6/N DONE!");
           }

           //D6 now is OFF 
           if (valueAction == "/D6/F") {
              digitalWrite(Pin6, HIGH);
              g = 0;
              h = 0;
              c = 0;

              request = ""; 
              value = digitalRead(Pin6);
              timer = false;
              Serial.println("/D6/F DONE!");
           }
          SEC = 0;
          MIN = 0;
          HOUR = 0; 
          Serial.println("Time out");
        } else {
            if (millis() - prMillis > 848) {
                prMillis = millis();
                 
                SEC++;
                if (SEC>59) {
                  SEC = 0;
                  MIN++;
                }
            
                if (MIN > 59) {
                  MIN = 0;
                  HOUR++;
                }
            
                if (HOUR > 23) {
                  HOUR = 0;
                } 
    
             }
            if (HOUR < 10)  Serial.print("0");
               Serial.print(HOUR);
               Serial.print(":");
            if (MIN < 10)  Serial.print("0");
               Serial.print(MIN);
               Serial.print(":");
            if (SEC < 10)  Serial.print("0");
               Serial.println(SEC);
               delay (1000); 
          }
    }
    
  client.flush();

//===================RESPONSE=======================
  if (timer == false && sensorMotion == false && illuminationSensor ==  false) {
    //Prepare the response
    response += (value)?"low":"high";
    Serial.println(response);
    // Send the response to the client
    client.print(response);
    delay(1);
    Serial.println("Client disonnected");
    //The client will actually be disconnected 
    //when the function returns and 'client' object is detroyed
  }
}

