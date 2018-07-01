#ifndef INC_I2C_H
#define	INC_I2C_H

#include <xc.h>                        
#include "main.h"
#define I2C_CLOCK 100000                //Define frequencia do clock i2c
#define BITRATE ((_XTAL_FREQ/(4*I2C_CLOCK))-1) // define bit rate para setar SSPAD

void I2C_Ready();                       //check se o dispositivo esta pronto/ocupado ou n�o
void I2C_Init();                        // inicializa configura��o i2c
char I2C_Start(char);                   // Envia pulso START com endere�o de grava��o do dispositivo escravo
void I2C_Start_Wait(char);              //Envia o pulso START com o endere�o de grava��o do dispositivo escravo at� a confirma��o 
char I2C_Repeated_Start(char);          //Enviar pulso de IN�CIO REPETIDO com endere�o de leitura do dispositivo escravo
char I2C_Stop();                        //Envia pulso de STOP
char I2C_Write(unsigned char);          //Escreve endere�o/dado no escravo
void I2C_Ack();                         //Envia reconhecimento para o ecravo continuar a leitura
void I2C_Nack();                        // envia sinal de n�o reconhecimento para o escravo para parar a informa��o corrente 
char I2C_Read(char);                    // le os dados dodispositivo escravo
#endif	/* INC_I2C_H */