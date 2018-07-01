#include <pic18f4550.h>
#include "i2c.h"

void I2C_Init()
{
    TRISB0 = 1;                     // Seleciona os pinos Rb0 e RB1 como entrada para a comunica��o
	TRISB1 = 1;
	SSPSTAT = 80;                   //Dados de entrada amostrados no final do tempo de sa�da de dados, Taxa de varia��o desativada, outros bits s�o limpos
    SSPCON1 = 0x28;					// habiliata sda e sca (RB) E RB1 para com i2c e define clock = FOSC / (4 * (SSPADD+1))*/ 
	SSPCON2 = 0;
    SSPADD = BITRATE;               //Seta a frequancia do clock no registrador de endere�os
    SSPIE = 1;                      //Habilita interrup�ao MSSP 
    SSPIF = 0;
}

char I2C_Start(char slave_write_address)
{   
    SSPCON2bits.SEN = 1;           // envia condi��o de START
    while(SSPCON2bits.SEN);         //Aguarda o complemento da condi��o de inicio
    SSPIF=0;
    if(!SSPSTATbits.S)              //transmiss�o foi completa == 1, falhou == 0
    return 0;
    return (I2C_Write(slave_write_address)); // escreve o endere�o no escravo e inicia comunica��o
}

void I2C_Start_Wait(char slave_write_address)
{
  while(1)
  {   
    SSPCON2bits.SEN = 1;             // envia condi��o de START 
    while(SSPCON2bits.SEN);         // aguarda o complemento da opera��o de inicio 
    SSPIF = 0;
    if(!SSPSTATbits.S)              //transmiss�o foi completa == 1, falhou == 0
        continue;
    I2C_Write(slave_write_address); // escreve o endere�o no escravo e inicia comunica��o
    if(ACKSTAT)                     // Check se ocorreu reconhecimento
    {
        I2C_Stop();                 // se nao ocorreu inicia rotina de STOP e continua while
        continue;
    }    
    break;                          // se sim, para o loop
  }
}

char I2C_Repeated_Start(char slave_read_address)
{
    RSEN = 1;                        //Inicia a condi��o de in�cio repetido nos pinos SDA e SCL. Apagado automaticamente pelo hardware
    while(SSPCON2bits.RSEN);        //aguarda a conclus�o da opera��o
    SSPIF = 0;
    if(!SSPSTATbits.S)              //REtorna 0 se a condi��o falhou
    return 0;
    I2C_Write(slave_read_address);  // escreve o endere�o no escravo e inicia comunica��o
    if (ACKSTAT)                    // Retorna 2 se o reconhecimeto occoreu, sen�o 1
     return 1;
    else
     return 2;
}

char I2C_Write(unsigned char data)
{
      SSPBUF = data;                // escreve a data no SSPBUFF
      I2C_Ready();
      if (ACKSTAT)                  // retorna 2 se ocorreu o reconhecimento, 1 caso contrario
        return 1;
      else
        return 2;
}

void I2C_Ack()
{
    ACKDT = 0;  					///REconhecimento de dado 1:NACK,0:ACK
	ACKEN = 1;        				//Habilita ACH para enviar
    while(ACKEN);
}

void I2C_Nack()
{
    ACKDT = 1;          			//N�o reconhecimento 1:NACK,0:ACK 
	ACKEN = 1;              		//Habilita ACK para enviar	          
    while(ACKEN);
}
char I2C_Read(char flag)            //LE dados do Ecravo com Ack= 0 & Nack = 1
{
    char buffer;
    RCEN = 1;                       // Habilita recep��o
    while(!SSPSTATbits.BF);         // aguarda o flag de "buffer completo", que � recebido apos receber o byte completo
    buffer = SSPBUF;                //Copia SSPBpara o buffer
    if(flag==0)
        I2C_Ack();                  //Envia reconhecimento
    else
        I2C_Nack();                 // Envia  N�o reconhecimenro 
    I2C_Ready();
    return(buffer);
}

char I2C_Stop()
{
    PEN = 1;                        // inicia a condi��o de parada
    while(PEN);                     //aguarda a confirma��o da condi��o de parada 
    SSPIF = 0;
    if(!SSPSTATbits.P);             // Retorna 0 se a condi��o de parada falhou
    return 0;
}

void I2C_Ready()
{
    while(!SSPIF);                  // Aguarda a opera��o se completar
    SSPIF=0;                        // limpa o flag de interrup�ao
}
