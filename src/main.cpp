#include <Arduino.h>
#include <OneWire.h>
#include <Wire.h>
#include <DallasTemperature.h>

#define Turbidity_pin A0
#define PH_pin A1
#define ONE_WIRE_BUS 2 

// Setup a oneWire instance to communicate with any OneWire devices  
OneWire oneWire(ONE_WIRE_BUS); 
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

float PH_calibration_value = 25.05;
int phval = 0; 
int buffer_arr[100],temp;
void print_temperature()
{
    sensors.requestTemperatures(); // Send the command to get temperature readings 
    Serial.print("Temperature is: "); 
    Serial.print(sensors.getTempCByIndex(0)); // 0 refers to the first IC on the wire   
    Serial.println(" degree celcius");
}

void print_PH_Level()
{
    for(int i=0;i<100;i++)  
        buffer_arr[i]=analogRead(A1);
    
    for(int i=0;i<9;i++)
    { 
        for(int j=i+1;j<10;j++)
        {
            if(buffer_arr[i]>buffer_arr[j])
            {
                temp=buffer_arr[i];
                buffer_arr[i]=buffer_arr[j];
                buffer_arr[j]=temp;
            }
        }
    }
    unsigned long int avgval=0;
    for(int i=2;i<98;i++)
        avgval+=buffer_arr[i];
    float volt=(float)avgval*5.0/1024/96;
    float ph_act = -5.70 * volt + PH_calibration_value;
    Serial.print("PH Level ");
    Serial.println(ph_act);
}

void print_Turbidity_Level()
{
    int sensorValue = analogRead(A0);
    int turbidity = map(sensorValue, 0,640, 100, 0);
    if (turbidity < 10) 
    {
        Serial.print("Turbidity level: ");
        Serial.println(turbidity);
        Serial.println("The water is clear ");
    }
    else if ((turbidity > 10) && (turbidity < 30)) 
    {
        Serial.print("Turbidity level: ");
        Serial.println(turbidity);
        Serial.println("The water is cloudy ");
    }
    else if ((turbidity > 31) && (turbidity < 50)) 
    {
        Serial.print("Turbidity level: ");
        Serial.println(turbidity);
        Serial.println("The water is very cloudy ");
    }
    else if (turbidity > 50) 
    {
        Serial.print("Turbidity level: ");
        Serial.println(turbidity);
        Serial.println("The water is dirty ");
    }
}

void setup(void) 
{ 
 // start serial port 
 Serial.begin(9600); 
// Serial.println("Dallas Temperature IC Control Library Demo"); 
 // Start up the library 
 sensors.begin(); 
} 

void loop(void)
{
    print_temperature();
    Serial.println("");
    print_PH_Level();
    Serial.println("");
    print_Turbidity_Level();
    Serial.println("");
}