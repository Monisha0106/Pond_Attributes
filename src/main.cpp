#include <Arduino.h>
#include <OneWire.h>
#include <Wire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>
#include<avr/pgmspace.h>
//#include "SPIFFS.h"
// #include <libxl.h>
//#include <sandcastle.vscode-open>
#define Turbidity_pin A0
#define PH_pin A1
#define ONE_WIRE_BUS 2 
SoftwareSerial SIM800L(11, 10);
float tmp;
float ph;
int turb_v;
float ammonia;
String turb;
String alert;
const PROGMEM float A[22][31]={{500.0,15.5,16.0,16.5,17.0,17.5,18.0,18.5,19.0,19.5,20.0,20.5,21.0,21.5,22.0,22.5,23.0,23.5,24.3,24.5,25.0,25.5,26.0,26.5,27.0,27.5,28.0,23.5,29.0,29.5,30.0},
                      {8.0,2.77,2.87,2.97,3.08,3.20,3.31,3.44,3.56,3.69,3.82,3.96,4.10,4.24,4.39,4.55,4.70,4.87,5.03,5.21,5.38,5.57,5.75,5.95,6.15,6.35,6.56,6.78,7.00,7.22,7.46},
                      {8.1,3.46,3.58,3.72,3.85,3.99,4.14,4.29,4.44,4.60,4.76,4.93,5.10,5.28,5.47,5.66,5.85,6.05,6.25,6.47,6.69,6.91,7.14,7.37,7.62,7.87,8.12,8.38,8.65,8.92,9.2l},
                      {8.2,4.31,4.47,4.63,4.80,4.98,5.15,5.34,5.53,5.72,5.92,6.13,6.34,6.56,6.79,7.02,7.25,7.50,7.75,8.01,8.27,8.54,8.82,9.11,9.40,9.70,10.0,10.3,10.7,11.0,11.3},
                      {8.3,5.37,5.56,5.76,5.97,6.18,6.40,6.63,6.86,7.10,7.34,7.60,7.86,8.12,8.39,8.68,8.96,9.25,9.56,9.88,10.2,10.5,10.9,11.2,11.5,11.9,12.3,12.7,13.0,13.4,13.8},
                      {8.4,5.67,5.91,7.15,7.40,7.66,7.93,8.20,8.49,8.77,9.07,9.38,9.69,10.0,10.3,10.7,11.0,11.4,11.7,12.1,12.5,12.9,13.3,13.7,14.1,14.5,15.0,15.4,15.9,16.4,16.8},
                      {8.6,10.2,10.5,10.9,11.2,11.6,12.0,12.4,12.8,13.2,13.7,14.1,14.5,15.0,15.5,15.9,16.4,16.9,17.4,17.9,18.5,19.0,19.6,20.1,20.7,21.3,21.8,22.4,23.0,23.7,24.3},
                      {8.7,12.5,12.9,13.3,13.8,14.2,14.7,15.1,15.6,16.1,16.6,17.1,17.6,18.2,18.7,19.3,19.8,20.4,21.0,21.5,22.2,22.8,23.4,24.1,24.7,25.4,26.0,26.7,27.4,28.1,28.8},
                      {8.8,15.2,15.7,16.2,16.7,17.2,17.8,18.3,18.9,19.5,20.0,20.6,21.2,21.8,22.5,23.1,23.7,24.4,25.l,25.7,26.4,27.1,27.8,28.5,29.2,30.0,30.7,31.4,32.2,32.9,33.7},
                      {8.9,18.4,19.0,19.6,20.2,20.8,21.4,22.0,22.7,23.3,24.0,24.7,25.3,26.0,26.7,27.4,28.2,28.9,29.6,30.4,31.1,31.9,32.7,33.4,34.2,35.0,35.8,36.6,37.4,38.2,39.0},
                      {9.0,22.1,22.8,23.5,24.1,24.8,25.5,26.2,27.0,27.7,28.4,29.2,29.9,30.7,31.5,32.3,33.0,33.8,34.5,35.5,36.3,37.1,37.9,38.7,39.6,40.4,41.2,42.1,42.9,43.8,44.6},
                      {9.1,26.4,27.1,27.8,28.6,29.4,30.1,30.9,31.7,32.5,33.3,34.2,35.0,35.8,36.6,37.5,38.3,39.2,40.0,40.9,41.7,42.5,43.5,44.3,45.2,46.1,46.9,47.8,48.5,49.5,50.4},
                      {9.2,31.1,31.9,32.7,33.5,34.4,35.2,36.1,36.9,37.3,38.6,39.5,40.4,41.2,42.1,43.0,43.9,44.8,45.7,46.5,47.4,48.3,49.2,50.1,50.9,51.3,52.7,53.5,54.4,55.2,56.1},
                      {9.3,36.2,37.1,38.0,38.8,39.7,40.6,41.5,42.4,43.3,44.2,45.1,46.0,46.9,47.8,48.7,49.6,50.5,51.4,52.3,53.2,54.0,54.9,55.8,56.6,57.5,58.3,59.2,60.0,60.8,61.6},
                      {9.4,41.7,42.6,43.5,44.4,45.4,46.3,47.2,48.1,49.0,49.9,50.9,51.8,52.7,53.6,54.5,55.4,56.2,57.1,58.0,58.8,59.7,60.5,61.4,62.2,63.0,63.8,64.6,65.4,66.2,66.9},
                      {9.5,47.3,48.3,49.2,50.2,51.1,52.0,52.9,53.9,54.8,55.7,56.6,57.5,58.3,59.2,60.1,60.9,61.8,62.5,63.5,64.3,65.1,65.9,66.7,67.4,68.2,68.9,69.7,70.4,71.1,71.8},
                      {9.6,53.1,54.0,55.0,55.9,56.8,57.7,58.6,59.5,60.4,61.3,62.1,63.0,63.8,64.6,65.5,66.3,67.1,67.8,68.5,69.4,70.1,70.8,71.6,72.3,73.0,73.6,74.3,75.0,75.6,76.2},
                      {9.7,58.8,59.7,60.6,61.5,62.3,63.2,64.1,64.9,65.7,66.6,67.4,68.2,68.9,69.7,70.5,71.2,71.9,72.7,73.4,74.0,74.7,75.4,76.0,76.6,77.3,77.9,78.5,79.0,79.6,80.1},
                      {9.8,64.2,65.1,65.9,66.8,67.6,68.4,69.2,70.0,70.7,71.6,72.2,72.9,73.7,74.3,75.0,75.7,76.3,77.0,77.6,78.2,78.8,79.4,80.0,80.5,81.1,81.6,82.1,82.6,83.1,83.6},
                      {9.9,69.3,70.1,70.9,71.7,72.4,73.1,73.9,74.5,75.3,75.9,76.6,77.2,77.9,78.4,79.1,79.7,80.3,80.8,81.4,81.9,82.4,82.9,83.4,83.9,84.3,84.8,85.2,85.7,86.1,86.5},
                      {10.0,74.0,74.7,75.4,76.1,76.8,77.4,78.1,78.7,79.3,79.9,80.5,81.0,81.5,82.1,82.6,83.2,83.5,84.1,84.5,85.1,85.5,85.9,86.3,86.8,87.1,87.5,87.9,88.3,88.6,89.0}};
//float arr[20][3];
// double arr[42][31]=  {{89.0,15.5,16.0,16.5,17.0,17.5,18.0,18.5,19.0,19.5,20.0,20.5,21.0,21.5,22.0,22.5,23.0,23.5,24.3,24.5,25.0,25.5,26.0,26.5,27.0,27.5,28.0,23.5,29.0,29.5,30.0},
//                       {6.0,0.0284,0.0295,0.0306,0.0318,.032,0.0343,0.0356,0.0369,0.0383,0.0397,0.0412,0.0427,0.0443,0.0459,0.0476,0.0493,0.051,0.0530,0.0549,0.0569,0.0589,0.0610,0.0632,0.0654,0.0678,0.0701,0.0726,0.0752,0.0778,0.0805},
//                       {6.1,0.0358,0.0372,0.0386,0.0401,0.0416,.043l,0.0448,0.0465,0.0482,0.0500,0.0518,0.0538,0.0557,0.0578,0.0599,0.0621,0.0644,0.0667,0.0691,0.0716,0.0742,0.0768,0.0796,0.0824,0.0853,0.0883,0.0914,0.0946,0.0979,0.101},
//                       {6.2,0.0451,0.0468,0.0486,0.0504,0.0523,0.0543,0.0564,0.0585,0.0607,0.0629,0.0653,0.0677,0.0702,0.0727,0.0754,0.0782,0.0810,0.0839,0.0870,0.0901,0.0933,0.0967,0.100,0.104,0.107,0.111,0.115,0.119,0.123,0.129},
//                       {6.4,0.0714,0.0741,0.0759,0.0799,0.0829,0.0860,0.0893,0.0925,0.0961,0.0997,0.103,0.107,0.111,0.115,0.119,0.124,0.128,0.133,0.138,0.143,0.148,0.153,0.159,0.164,0.170,0.176,0.182,0.189,0.195,0.202},
//                       {6.5,0.0898,0.0933,0.0968,0.101,0.104,0.108,0.112,0.117,0.121,0.125,0.130,0.135,0.140,0.145,0.150,0.156,0.162,0.167,0.173,0.180,0.186,0.193,0.200,0.207,0.214,0.221,0.229,0.237,0.246,0.254},
//                       {6.6,0.113,0.117,0.122,0.127,0.131,0.126,0.141,0.147,0.152,0.158,0.164,0.170,0.176,0.183,0.189,0.196,0.203,0.211,0.218,0.225,0.234,0.242,0.251,0.260,0.269,0.279,0.289,0.299,0.309,0.320},
//                       {6.7,0.142,0.148,0.153,0.159,0.165,0.172,0.178,0.185,0.192,0.199,0.206,0.214,0.222,0.230,0.238,0.247,0.256,0.265,0.275,0.284,0.295,0.305,0.316,0.327,0.339,0.351,0.363,0.376,0.389,0.402},
//                       {6.8,0.179,0.186,0.193,0.200,0.208,0.216,0.224,0.232,0.241,0.250,0.259,0.269,0.279,0.289,0.300,0.310,0.322,0.333,0.345,0.358,0.371,0.384,0.397,0.411,0.426,0.441,0.456,0.472,0.489,0.506},
//                       {6.9,0.225,0.234,0.243,0.252,0.262,0.272,0.282,0.292,0.303,0.315,0.326,0.338,.35l,0.364,0.377,0.390,0.405,0.419,0.434,0.450,0.466,0.483,0.500,0.517,0.536,0.554,0.574,0.594,0.615,0.636},
//                       {7.0,0.284,0.294,0.306,0.317,0.329,0.342,0.355,0.368,0.381,0.396,0.410,0.425,.44l,0.457,0.474,0.491,0.509,0.527,0.546,0.566,0.586,0.607,0.628,0.651,0.674,0.697,0.722,0.747,0.772,0.799},
//                       {7.1,0.357,0.370,0.384,0.399,0.414,0.430,0.446,0.463,0.480,0.498,0.516,0.535,0.555,0.575,0.596,0.617,0.640,0.663,0.687,0.711,0.737,0.763,0.790,0.818,0.846,0.876,0.907,0.938,0.970,1.00,},
//                       {7.2,0.449,0.466,0.483,0.502,0.521,0.540,0.561,0.582,0.603,0.625,0.649,0.673,0.697,0.723,0.749,0.776,0.804,0.833,0.863,0.894,0.926,0.958,0.992,1.03,1.06,1.10,1.14,1.18,1.22,1.25},
//                       {7.3,0.564,0.586,0.608,0.631,0.655,0.679,0.705,0.731,0.758,0.786,0.815,0.845,0.876,0.908,0.941,0.975,1.01,1.05,1.08,1.12,1.16,1.20,1.25,1.29,1.33,1.38,1.43,1.48,1.53,1.58},
//                       {7.4,0.709,0.736,0.764,0.793,0.823,0.854,0.886,0.919,0.953,0.988,1.02,1.06,1.10,1.14,1.18,1.22,1.27,1.31,1.36,1.41,1.46,1.51,1.56,1.62,1.67,1.73,1.79,1.85,1.92,1.98},
//                       {7.5,0.891,0.925,0.960,0.996,1.03,1.07,1.11,1.15,1.20,1.24,1.29,1.33,1.38,1.43,1.48,1.54,1.59,1.65,1.71,1.77,1.83,1.89,1.96,2.03,2.10,2.17,2.25,2.32,2.40,2.48},
                     // {7.6,1.12,1.16,1.21,1.25,1.30,1.35,1.40,1.45,1.50,1.56,1.61,1.67,1.73,1.80,1.86,1.93,2.00,2.07,2.14,2.22,2.29,2.37,2.46,2.54,2.63,2.72,2.81,2.91,3.01,3.11},
                     // {7.7,1.41,1.46,1.51,1.57,1.63,1.69,1.75,1.82,1.88,1.95,2.02,2.10,2.17,2.25,2.33,2.41,2.50,2.59,2.68,2.77,2.87,2.97,3.07,3.18,3.29,3.40,3.5l,3.63,2.75,3.88},
                   //   {7.8,1.76,1.83,1.90,1.97,2.04,2.12,2.20,2.28,2.36,2.44,2.53,2.63,2.72,2.82,2.92,3.02,3.13,3.24,3.35,3.47,3.59,3.71,3.84,3.97,4.10,4.24,4.38,4.53,4.68,4.84},
                     // {7.9,2.21,2.29,2.38,2.47,2.56,2.65,2.75,2.85,2.95,3.06,3.17,3.28,3.40,3.52,3.64,3.77,3.90,4.04,4.18,4.33,4.47,4.53,4.78,4.94,5.11,5.28,5.46,5.64,5.82,6.0l},
  

 
  

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
    
    // for(int i=0;i<9;i++)
    // { 
    //     for(int j=i+1;j<10;j++)
    //     {
    //         if(buffer_arr[i]>buffer_arr[j])
    //         {
    //             temp=buffer_arr[i];
    //             buffer_arr[i]=buffer_arr[j];
    //             buffer_arr[j]=temp;
    //         }
    //     }
    // }
    unsigned long int avgval=0;
    for(int i=2;i<98;i++)
        avgval+=buffer_arr[i];
    float volt=(float)avgval*5.0/1024/96;
    ph = -5.70 * volt + PH_calibration_value;
    // Serial.print("PH Level ");
    // Serial.println(ph_act);
    
}

float temp_rounder(float v)
{
    if(v-floor(v)<0.35)
        return floor(v);
    else if((v-floor(v))>0.35 || (v-floor(v))<0.75)
        return (floor(v)+0.5);
    else if(v-floor(v)>0.75)
        return (round(v));
    else 
        return round(v);
}
float print_Ammonia_Level()
{    
    //print_temperature();
    //print_PH_Level();
    temp = 19.83;
    ph = 9.9;
    double temper = temp_rounder(temp);
    double p_h = roundf(ph);
    Serial.println(temper);
    Serial.println(p_h);
    int ph_index=0;
    int temp_index=0;
     
    
    for (uint8_t i=0; i<22; i++)
    {   
        //Serial.println(i);
        float smt = pgm_read_float(&A[i][0]);
        //Serial.println(smt);
        if(smt == p_h)
        {
            Serial.println(i);
            ph_index = i;
            //break;
           
        } 
        //break;      
    }
    for (uint8_t i=0; i<31; i++)
    {   
        float smt = pgm_read_float(&A[0][i]);
        if(smt == temper)
            {
                temp_index = i;
               // break;
            }
    }
    ammonia =pgm_read_float(&A[ph_index][temp_index]);
    //Serial.println(ph_index);
    //Serial.println(temp_index);
    //float ammonia = arr[ph_index][temp_index];
    
    //Serial.print("Ammonia level is: ");
    //Serial.println(ammonia);
}
    // else if(ph<9.0 && ph>8.4)
    // {
    //     double A[6][31]={{89.0,15.5,16.0,16.5,17.0,17.5,18.0,18.5,19.0,19.5,20.0,20.5,21.0,21.5,22.0,22.5,23.0,23.5,24.3,24.5,25.0,25.5,26.0,26.5,27.0,27.5,28.0,23.5,29.0,29.5,30.0},
    //                   {8.5,8.25,8.54,8.84,9.14,9.46,9.78,10.1,10.5,10.8,11.2,11.5,11.9,12.3,12.7,13.1,13.5,13.9,14.4,14.8,15.3,15.7,16.2,16.7,17.2,17.7,18.2,18.7,19.2,19.8,20.3},
    //                   {8.6,10.2,10.5,10.9,11.2,11.6,12.0,12.4,12.8,13.2,13.7,14.1,14.5,15.0,15.5,15.9,16.4,16.9,17.4,17.9,18.5,19.0,19.6,20.1,20.7,21.3,21.8,22.4,23.0,23.7,24.3},
    //                   {8.7,12.5,12.9,13.3,13.8,14.2,14.7,15.1,15.6,16.1,16.6,17.1,17.6,18.2,18.7,19.3,19.8,20.4,21.0,21.5,22.2,22.8,23.4,24.1,24.7,25.4,26.0,26.7,27.4,28.1,28.8},
    //                   {8.8,15.2,15.7,16.2,16.7,17.2,17.8,18.3,18.9,19.5,20.0,20.6,21.2,21.8,22.5,23.1,23.7,24.4,25.l,25.7,26.4,27.1,27.8,28.5,29.2,30.0,30.7,31.4,32.2,32.9,33.7},
    //                   {8.9,18.4,19.0,19.6,20.2,20.8,21.4,22.0,22.7,23.3,24.0,24.7,25.3,26.0,26.7,27.4,28.2,28.9,29.6,30.4,31.1,31.9,32.7,33.4,34.2,35.0,35.8,36.6,37.4,38.2,39.0}};
    //     for (int i=0; i<6; i++)
    //     {
    //         if(A[i][0] == p_h)
    //         {
    //         ph_index = i;
    //         //break;
    //         Serial.println(A[i][0]);
    //         } 
    //     break;      
    //     }
    //     for (int i=0; i<31; i++)
    //     {
    //         if(A[0][i] == temper)
    //         {
    //             temp_index = i;
    //            // break;
    //         }
    //     }
    //  ammonia = A[ph_index][temp_index];
    // }
    // else if(ph<9.5 && ph>8.9)
    // {
    //      double A[6][31]= {{89.0,15.5,16.0,16.5,17.0,17.5,18.0,18.5,19.0,19.5,20.0,20.5,21.0,21.5,22.0,22.5,23.0,23.5,24.3,24.5,25.0,25.5,26.0,26.5,27.0,27.5,28.0,23.5,29.0,29.5,30.0},
    //                   {9.0,22.1,22.8,23.5,24.1,24.8,25.5,26.2,27.0,27.7,28.4,29.2,29.9,30.7,31.5,32.3,33.0,33.8,34.5,35.5,36.3,37.1,37.9,38.7,39.6,40.4,41.2,42.1,42.9,43.8,44.6},
    //                   {9.1,26.4,27.1,27.8,28.6,29.4,30.1,30.9,31.7,32.5,33.3,34.2,35.0,35.8,36.6,37.5,38.3,39.2,40.0,40.9,41.7,42.5,43.5,44.3,45.2,46.1,46.9,47.8,48.5,49.5,50.4},
    //                   {9.2,31.1,31.9,32.7,33.5,34.4,35.2,36.1,36.9,37.3,38.6,39.5,40.4,41.2,42.1,43.0,43.9,44.8,45.7,46.5,47.4,48.3,49.2,50.1,50.9,51.3,52.7,53.5,54.4,55.2,56.1},
    //                   {9.3,36.2,37.1,38.0,38.8,39.7,40.6,41.5,42.4,43.3,44.2,45.1,46.0,46.9,47.8,48.7,49.6,50.5,51.4,52.3,53.2,54.0,54.9,55.8,56.6,57.5,58.3,59.2,60.0,60.8,61.6},
    //                   {9.4,41.7,42.6,43.5,44.4,45.4,46.3,47.2,48.1,49.0,49.9,50.9,51.8,52.7,53.6,54.5,55.4,56.2,57.1,58.0,58.8,59.7,60.5,61.4,62.2,63.0,63.8,64.6,65.4,66.2,66.9}};
    //     for (int i=0; i<6; i++)
    //     {
    //         if(A[i][0] == p_h)
    //         {
    //             ph_index = i;
    //         //break;
    //             Serial.println(A[i][0]);
    //         } 
    //     break;      
    //     }
    //     for (int i=0; i<31; i++)
    //     {
    //         if(A[0][i] == temper)
    //         {
    //             temp_index = i;
    //            // break;
    //         }
    //     }
    //  ammonia = A[ph_index][temp_index];
    // }
    // else if(ph<10.1 && ph>9.4)
    // {
    //     double A[7][31]=   {{89.0,15.5,16.0,16.5,17.0,17.5,18.0,18.5,19.0,19.5,20.0,20.5,21.0,21.5,22.0,22.5,23.0,23.5,24.3,24.5,25.0,25.5,26.0,26.5,27.0,27.5,28.0,23.5,29.0,29.5,30.0},
    //                   {9.5,47.3,48.3,49.2,50.2,51.1,52.0,52.9,53.9,54.8,55.7,56.6,57.5,58.3,59.2,60.1,60.9,61.8,62.5,63.5,64.3,65.1,65.9,66.7,67.4,68.2,68.9,69.7,70.4,71.1,71.8},
    //                   {9.6,53.1,54.0,55.0,55.9,56.8,57.7,58.6,59.5,60.4,61.3,62.1,63.0,63.8,64.6,65.5,66.3,67.1,67.8,68.5,69.4,70.1,70.8,71.6,72.3,73.0,73.6,74.3,75.0,75.6,76.2},
    //                   {9.7,58.8,59.7,60.6,61.5,62.3,63.2,64.1,64.9,65.7,66.6,67.4,68.2,68.9,69.7,70.5,71.2,71.9,72.7,73.4,74.0,74.7,75.4,76.0,76.6,77.3,77.9,78.5,79.0,79.6,80.1},
    //                   {9.8,64.2,65.1,65.9,66.8,67.6,68.4,69.2,70.0,70.7,71.6,72.2,72.9,73.7,74.3,75.0,75.7,76.3,77.0,77.6,78.2,78.8,79.4,80.0,80.5,81.1,81.6,82.1,82.6,83.1,83.6},
    //                   {9.9,69.3,70.1,70.9,71.7,72.4,73.1,73.9,74.5,75.3,75.9,76.6,77.2,77.9,78.4,79.1,79.7,80.3,80.8,81.4,81.9,82.4,82.9,83.4,83.9,84.3,84.8,85.2,85.7,86.1,86.5},
    //                   {10.0,74.0,74.7,75.4,76.1,76.8,77.4,78.1,78.7,79.3,79.9,80.5,81.0,81.5,82.1,82.6,83.2,83.5,84.1,84.5,85.1,85.5,85.9,86.3,86.8,87.1,87.5,87.9,88.3,88.6,89.0}};
    //     for (int i=0; i<6; i++)
    //     {
    //         if(A[i][0] == p_h)
    //         {
    //         ph_index = i;
    //         //break;
    //         Serial.println(A[i][0]);
    //         } 
    //     break;      
    //     }
    //     for (int i=0; i<31; i++)
    //     {
    //         if(A[0][i] == temper)
    //         {
    //             temp_index = i;
    //            // break;
    //         }
    //     }
    //  ammonia = A[ph_index][temp_index];
    // }
    // else
    // {
    //     Serial.println("Ammonia level e morse");
    // }
    
    
  

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
//float arr[];
// float ammonia_value(int a, int b)
// {
//     float ammonia;
//     for(int i=0;i<42;i++)
//     {   if(arr[i][0]==print_temperature())
//         {
//             for(int j=0;j<31;j++)
//             {
//                 if(arr[0][j]==print_PH_Level())
//                 {
//                      ammonia= arr[i][j];
//                 }
//             }
//     }
//     return ammonia;
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
        String msg = "ph: " + String(ph) + "\n" + alert + "\n" + "Percent ammonia level:" + String(ammonia) + "%";
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
        String msg = "temperature: " + String(tmp) + " C" + "\n" + "ph: " + String(ph) + "\n" + "Percent ammonia level:" + String(ammonia) + "%" + "\n" + "turbidity: " + String(turb_v) + "\n" + turb ;
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
{   
    print_Ammonia_Level();
    print_PH_Level();
    print_Turbidity_Level();
    //print_temperature();
    //delay(1000);
    //print_PH_Level();
    if (ph>11 || ph<4.0){
       // red_alert="Fishes are dying."
        Send_Message("Fishes are dying", "01521334490");
    }
    if(ammonia>55.0)
    {
        Serial.print(ammonia);
        Serial.print("%");
        Serial.println("mach mortese sob");
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