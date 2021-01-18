#include <Arduino.h>
#include <OneWire.h>
#include <Wire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>

#define Turbidity_pin A0
#define PH_pin A1
#define ONE_WIRE_BUS 2 
SoftwareSerial SIM800L(11, 10);
float tmp;
float ph;
int turb_v;
String turb;
String alert;

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
    tmp=sensors.getTempCByIndex(0);
     // 0 refers to the first IC on the wire   
    // Serial.println(" degree celcius");
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
    ph = -5.70 * volt + PH_calibration_value;
    // Serial.print("PH Level ");
    // Serial.println(ph_act);
    
}

void print_Turbidity_Level()
{
    int sensorValue = analogRead(A0);
    int turbidity = map(sensorValue, 0,640, 100, 0);
    if (turbidity < 10) 
    {
        // Serial.print("Turbidity level: ");
        // Serial.println(turbidity);
        // Serial.println("The water is clear ");
        turb_v=turbidity;
        turb="clear water";
    }
    else if ((turbidity > 10) && (turbidity < 30)) 
    {
        Serial.print("Turbidity level: ");
        Serial.println(turbidity);
        Serial.println("The water is cloudy ");
        turb_v=turbidity;
        turb="cloudy water";
    }
    else if ((turbidity > 31) && (turbidity < 50)) 
    {
        // Serial.print("Turbidity level: ");
        // Serial.println(turbidity);
        // Serial.println("The water is very cloudy ");
        turb_v=turbidity;
        turb="very cloudy water";
    }
    else if (turbidity > 50) 
    {
    //     Serial.print("Turbidity level: ");
    //     Serial.println(turbidity);
    //     Serial.println("The water is dirty ");
     turb_v=turbidity;
     turb="water is really dirty";
    }
}


void setup(void) 
{ 
 // start serial port 
    Serial.begin(9600); 
    SIM800L.begin(9600);
// Serial.println("Dallas Temperature IC Control Library Demo"); 
 // Start up the library 
    sensors.begin(); 
    Serial.println("Initializing...");
    delay(1000);
    SIM800L.println("AT+CNMI=1,2,0,0,0"); 
    delay(1000);
//   pinMode(Hall_Effect_Sensor_pin, INPUT_PULLUP);
//     // pinMode(Reed_Switch_pin, INPUT_PULLUP);
//   attachInterrupt(Hall_Effect_Sensor_pin, pulse_counter, FALLING);
//   // bool check = Execute_Command("meter:1233141");
//   // Serial.println(check);
//   // check = Execute_Command("flow:100");
//   // Serial.println(check);
//   EEPROM.get(meter_reading_save_address, current_water_meter_reading);
//   EEPROM.get(flow_per_pulse_save_address, flow_per_pulse);

//   Serial.println(current_water_meter_reading);
//   Serial.println(flow_per_pulse);
} 
typedef struct SString
{
    String number;
    String text;
};

String Gateway_Number = "01518441361"; // Number to which all communication will occur
SString Message; // this stores the sender's number and the text
bool new_message = false; //checks if there is a new message

//SoftwareSerial SIM800L(11, 10); // new (Rx, Tx) of pro mini

void Delete_Message(String location)
{
    SIM800L.println("AT+CMGD="+ location + "\r");
}

// For sending message
void Send_Message(String message, String number)
{
    SIM800L.println("AT+CMGF=1");    
    delay(1000);
    SIM800L.println("AT+CMGS=\"+" + number + "\"\r"); 
    delay(1000);
    SIM800L.println(message);
    delay(100);
    SIM800L.println((char)26);
    delay(1000);
}

// For receiving message
SString Receive_Message()
{   
    
    //Serial.println("hakku");
    String textMessage = "";

    SIM800L.print("AT+CMGF=1\r");
    delay(500);
    if(SIM800L.available() > 0)
    {
        textMessage = SIM800L.readString();
        delay(500);
        //Serial.println("Eikhaneo ashsi");
        Serial.println(textMessage);
    }
    // Serial.println(textMessage);

    SString msg;
    String temp;
    String data = textMessage;
    int len = data.length(), cnt1 = 0, cnt2 = 0;

    bool next_rn = false;

    for(int i = 0; i < len;i++)
    {
      // Serial.println(data.substring(i,i+1));
        temp = data.substring(i,i+1);

        if(temp == "+" && i < len - 1)
        {
            Serial.println(data.substring(i+1,i+2));
            if(data.substring(i+1,i+2) == "8")
            {
                msg.number = data.substring(i+3,i+14);
                i += 14;
            }
            
        }

        else if(temp == "\"" && i < len -1)
        {
            if(data.substring(i + 1, i + 2) == "\r" && data.substring(i + 2, i + 3) == "\n")
            {
                cnt1 = i + 3;
                next_rn = true;
                i += 3;
            }
        
        }

        else if(temp == "\r" && i < len -1)
        {
            if(data.substring(i + 1, i + 2) == "\n" && next_rn)
            {
                cnt2 = i;
                Serial.print("Extracted text: ");
                Serial.println(data.substring(cnt1, cnt2));
                next_rn = false;
            }
        }
    }
    msg.text = data.substring(cnt1, cnt2);
    // Serial.println(msg.number.length());
    // Serial.println(msg.text.length());
    //Serial.println("I CAME HERE");
    if(msg.number.length() > 0) 
    {
        Serial.println(msg.number.length());
        if(msg.text.length() > 0)
        {
            new_message = true;
        }
        else
        {
            Serial.println("Message received but text missing!");
            new_message = false;
            Send_Message("error:blank_message", msg.number);
        }
    }
    else
    {
        Serial.println(msg.number.length());
        new_message = false;
    }
    
    return msg;
}





// boolean isValidNumber(String str)
// {
//    boolean isNum=false;
//    for(byte i=0;i<str.length();i++)
//    {
//        isNum = isDigit(str.charAt(i)) || str.charAt(i) == '+' || str.charAt(i) == '.' || str.charAt(i) == '-';
//        if(!isNum) return false;
//    }
//    return isNum;
// }
bool Execute_Command(String Command)
{
//   if(isValidNumber(Command))//Command looks like this meter:xxxxxx
//   {
//     String initial_water_meter_reading_str = Command;//.substring(6);
//     current_water_meter_reading = (unsigned long)initial_water_meter_reading_str.toInt();
//     EEPROM.put(meter_reading_save_address, current_water_meter_reading);
//     EEPROM.get(meter_reading_save_address, current_water_meter_reading);
//     Serial.println(current_water_meter_reading);
//     return true;
//   }
    if(Command.indexOf("temp") > -1)//Command looks like this flow:xxxxxx
    { 
        print_temperature();
        Serial.println("Temp coming");
        String msg = "temperature: " + String(tmp) + " C";
        Serial.println(msg);
        Send_Message(msg, Message.number);
        return true;
    }
    else if(Command.indexOf("ph") > -1) // Command looks like ***getwater***
    { 
        print_PH_Level();
    // send water meter reading
    // Send_Message("getwater:" + String(current_water_meter_reading), Gateway_Number);
        if (ph >8.6){
            alert= "Very high ammonia level";
        }
        else if (ph<4.5){
            alert="very acidic water";
        }
        else if (ph>6.5 && ph< 8.5){
            alert="water is fine";
        }
        else {
            alert= "Stressed ph level";
        }
        String msg = "ph: " + String(ph) + alert;
        Serial.println(msg);
        Send_Message(msg, Message.number);
        return true;
    }
    else if(Command.indexOf("turbidity") > -1) // Command looks like reset
    { 
        print_Turbidity_Level();
        String msg = "turbidity: " + String(turb_v) + "\n" + turb;
        Serial.println(msg);
        Send_Message(msg, Message.number);
        return true;
    }
    else if(Command.indexOf("All") > -1) // Command looks like reset
    {   print_PH_Level();
        print_temperature();
        print_Turbidity_Level();
        String msg = "temperature: " + String(tmp) + " C" + "\n" + "ph: " + String(ph) + "\n" + "turbidity: " + String(turb_v) + "\n" + turb ;
        Serial.println(msg);
        Send_Message(msg, Message.number);
        return true;
    }
    else
    {
        Send_Message("error:invalid_command", Message.number);
        return false;
    }
}
//String red_alert;
void loop(void)
{   print_PH_Level();
    if (ph>11 || ph<4.0){
       // red_alert="Fishes are dying."
        Send_Message("Fishes are dying", "01521334490");
    }
    Message = Receive_Message();

    Serial.print("Number: ");
    Serial.print(Message.number);
    Serial.print(" Text: ");
    Serial.println(Message.text);

  if(new_message)
  {
    Serial.print("Received new message!\n");
    Serial.println(Message.number);
    Serial.println(Message.text);
    bool response = Execute_Command(Message.text);
    if(response)
    Send_Message("successfull", Message.number);
  }
  else
  {
    Serial.println("No new message was received");
  }
  delay(1000);
//     print_temperature();
//     Serial.println("");
//     print_PH_Level();
//     Serial.println("");
//     print_Turbidity_Level();
//     Serial.println("");
 }