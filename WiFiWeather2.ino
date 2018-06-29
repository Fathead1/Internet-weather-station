


#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <ArduinoJson.h>

const char* ssid     = "***********";      // SSID of local network
const char* password = "***********";   // Password on network
String APIKEY = "********************************"; // API from openweathermap.org
String CityID = "2638219"; //Selsey, GB

WiFiClient client;
char servername[]="api.openweathermap.org";  // remote server we will connect to
String result;

int  counter = 60;
int a = 2.237; //to convert m/s to mph
#define today     0 
int omode, cmode=today;   

String weatherDescription ="";
String weatherLocation = "";
String Country;
float Temperature;
float Humidity;
float Pressure;
String Windspeed ="";
String WindDirection ="";
String speed;
String deg;

LiquidCrystal_I2C lcd(0x27,20,4); 

const char*  cards[] = {"N  ", "NNE", "NE ","ENE","E  ","ESE","SE ","SSE","S  ","SSW","SW ","WSW","W  ","WNW","NW ","NNW"};

#define mps2mph 2.237


  // getCardinal: converts the headsing (in degrees) to a text-based cardinal heading ("ENE", for example)
String getCardinal( float h ) { 
  h += 11.25; if (h > 360.0) h = h-360.0;
  return cards[(int) (h / 22.5)];
}

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("CONNECTING TO");  
  delay (2000);
  lcd.setCursor(0,2);
  lcd.print("  Origin Broadband");
  delay (2000);
  Serial.println("Connecting");
  WiFi.begin(ssid, password);

  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  lcd.clear();
  lcd.setCursor(0,2);
  lcd.print("    Connected !");
  Serial.println("Connected");
  delay(2000);
  
}

void loop() {
    if(counter == 60) //Get new data every 10 minutes
    {
      counter = 0;
      displayGettingData();
      delay(1000);
      getWeatherData();
    }else
    {
      counter++;
      displayWeather(weatherLocation,weatherDescription);
      delay(5000);
      displayConditions(Temperature,Humidity,Pressure);
      delay(5000);
      displayConditions2(Windspeed,WindDirection);
      delay(5000);
    }

}

void getWeatherData() //client function to send/receive GET request data.
{
  if (client.connect(servername, 80)) {  //starts client connection, checks for connection
    client.println("GET /data/2.5/weather?id="+CityID+"&units=metric&APPID="+APIKEY);
    client.println("Host: api.openweathermap.org");
    
    client.println("Connection: close");
    client.println();
  } 
  else {
    Serial.println("connection failed"); //error message if no client connect
    Serial.println();
  }

  while(client.connected() && !client.available()) delay(1); //waits for data
  while (client.connected() || client.available()) { //connected or data available
    char c = client.read(); //gets byte from ethernet buffer
      result = result+c;
    }

  client.stop(); //stop client
  result.replace('[', ' ');
  result.replace(']', ' ');
  Serial.println(result);

char jsonArray [result.length()+1];
result.toCharArray(jsonArray,sizeof(jsonArray));
jsonArray[result.length() + 1] = '\0';

StaticJsonBuffer<1024> json_buf;
JsonObject &root = json_buf.parseObject(jsonArray);
if (!root.success())
{
  Serial.println("parseObject() failed");
}

String location = root["name"];
String country = root["sys"]["country"];
float temperature = root["main"]["temp"];
float humidity = root["main"]["humidity"];
String weather = root["weather"]["main"];
String description = root["weather"]["description"];
float pressure = root["main"]["pressure"];
String speed = root["wind"]["speed"];
String deg = root["wind"]["deg"];

weatherDescription = description;
weatherLocation = location;
Country = country;
Temperature = temperature;
Humidity = humidity;
Pressure = pressure;
Windspeed = speed;
WindDirection = deg;

}

void displayWeather(String location,String description)
{
  lcd.clear();
  lcd.setCursor(7,0);
  lcd.print(location);
  lcd.setCursor(5,2);
  lcd.print(description);
  
}

void displayConditions(float Temperature,float Humidity,float Pressure)
{
 lcd.clear();
 lcd.print("Temp: "); 
 lcd.print(Temperature,2);
 lcd.print((char)223);
 lcd.print("C");

 //Printing Humidity
 lcd.setCursor(0,2);
 lcd.print("Humidity: ");
 lcd.print(Humidity,0);
 lcd.print("%");
 
 //Printing Pressure
 lcd.setCursor(0,3);
 lcd.print("Pressure: ");
 lcd.print(Pressure,1);
 lcd.print("hPa");

 }

void displayConditions2(String speed, String deg)
{
  
 //Printing Windspeed
 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("Windspeed: ");
 lcd.print(speed);
 lcd.print("m/s");

 lcd.setCursor(0,2);
 lcd.print("Wind Dir: ");
 lcd.print(deg);
 lcd.print((char)223);
 lcd.setCursor(14,2);
 lcd.print(getCardinal(deg[cmode]));  
 
}

void displayGettingData()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("    Getting data");
  lcd.setCursor(0,1);
  lcd.print("        From");
  lcd.setCursor(0,3);
  lcd.print(" openweathermap.org");
  delay(2000);
}



