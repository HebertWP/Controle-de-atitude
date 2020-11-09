#include <Arduino.h>
#define AEN  5 
#define A1 6
#define A2 7

void setup(){
    pinMode(AEN, OUTPUT);
    pinMode(A1, OUTPUT);
    pinMode(A2, OUTPUT);
}

void loop(){
    analogWrite(AEN, 0);
    digitalWrite(A1,LOW);
    digitalWrite(A2,HIGH);
    for(int i=0; i <256;i++){
        analogWrite(AEN, i);
        delay(100);
    }
    digitalWrite(A1,LOW);
    digitalWrite(A2,LOW);
    delay(3000);
    
    analogWrite(AEN, 0);
    digitalWrite(A1,HIGH);
    digitalWrite(A2,LOW);
    for(int i=0; i <256;i++){
        analogWrite(AEN, i);
        delay(100);
    }
    analogWrite(AEN, 0);
    digitalWrite(A1,LOW);
    digitalWrite(A2,LOW);
    delay(3000);
    
}