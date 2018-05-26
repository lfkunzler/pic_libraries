

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef I2C_Master_H
#define	I2C_Master_H

#include <xc.h>                        
#define F_OSC 20000000                  // Define frequencia do micro
#define I2C_CLOCK 100000                //Define frequencia do clock i2c
#define BITRATE ((F_OSC/(4*I2C_CLOCK))-1) // define bit rate para setar SSPAD

void I2C_Ready();                       //check se o dispositivo esta pronto/ocupado ou não
void I2C_Init();                        // inicializa configuração i2c
char I2C_Start(char);                   // Envia pulso START com endereço de gravação do dispositivo escravo
void I2C_Start_Wait(char);              //Envia o pulso START com o endereço de gravação do dispositivo escravo até a confirmação 
char I2C_Repeated_Start(char);          //Enviar pulso de INÍCIO REPETIDO com endereço de leitura do dispositivo escravo
char I2C_Stop();                        //Envia pulso de STOP
char I2C_Write(unsigned char);          //Escreve endereço/dado no escravo
void I2C_Ack();                         //Envia reconhecimento para o ecravo continuar a leitura
void I2C_Nack();                        // envia sinal de não reconhecimento para o escravo para parar a informação corrente 
char I2C_Read(char);                    // le os dados dodispositivo escravo
#endif	/* I2C_Master_H */

