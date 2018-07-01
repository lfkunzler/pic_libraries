#include <pic18f4550.h>
#include "i2c.h"

void I2C_Init()
{
    TRISB0 = 1;                     // Seleciona os pinos Rb0 e RB1 como entrada para a comunicação
	TRISB1 = 1;
	SSPSTAT = 80;                   //Dados de entrada amostrados no final do tempo de saída de dados, Taxa de variação desativada, outros bits são limpos
    SSPCON1 = 0x28;					// habiliata sda e sca (RB) E RB1 para com i2c e define clock = FOSC / (4 * (SSPADD+1))*/ 
	SSPCON2 = 0;
    SSPADD = BITRATE;               //Seta a frequancia do clock no registrador de endereços
    SSPIE = 1;                      //Habilita interrupçao MSSP 
    SSPIF = 0;
}

char I2C_Start(char slave_write_address)
{   
    SSPCON2bits.SEN = 1;           // envia condição de START
    while(SSPCON2bits.SEN);         //Aguarda o complemento da condição de inicio
    SSPIF=0;
    if(!SSPSTATbits.S)              //transmissão foi completa == 1, falhou == 0
    return 0;
    return (I2C_Write(slave_write_address)); // escreve o endereço no escravo e inicia comunicação
}

void I2C_Start_Wait(char slave_write_address)
{
  while(1)
  {   
    SSPCON2bits.SEN = 1;             // envia condição de START 
    while(SSPCON2bits.SEN);         // aguarda o complemento da operação de inicio 
    SSPIF = 0;
    if(!SSPSTATbits.S)              //transmissão foi completa == 1, falhou == 0
        continue;
    I2C_Write(slave_write_address); // escreve o endereço no escravo e inicia comunicação
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
    RSEN = 1;                        //Inicia a condição de início repetido nos pinos SDA e SCL. Apagado automaticamente pelo hardware
    while(SSPCON2bits.RSEN);        //aguarda a conclusão da operação
    SSPIF = 0;
    if(!SSPSTATbits.S)              //REtorna 0 se a condição falhou
    return 0;
    I2C_Write(slave_read_address);  // escreve o endereço no escravo e inicia comunicação
    if (ACKSTAT)                    // Retorna 2 se o reconhecimeto occoreu, senão 1
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
    ACKDT = 1;          			//Não reconhecimento 1:NACK,0:ACK 
	ACKEN = 1;              		//Habilita ACK para enviar	          
    while(ACKEN);
}
char I2C_Read(char flag)            //LE dados do Ecravo com Ack= 0 & Nack = 1
{
    char buffer;
    RCEN = 1;                       // Habilita recepção
    while(!SSPSTATbits.BF);         // aguarda o flag de "buffer completo", que é recebido apos receber o byte completo
    buffer = SSPBUF;                //Copia SSPBpara o buffer
    if(flag==0)
        I2C_Ack();                  //Envia reconhecimento
    else
        I2C_Nack();                 // Envia  Não reconhecimenro 
    I2C_Ready();
    return(buffer);
}

char I2C_Stop()
{
    PEN = 1;                        // inicia a condição de parada
    while(PEN);                     //aguarda a confirmação da condição de parada 
    SSPIF = 0;
    if(!SSPSTATbits.P);             // Retorna 0 se a condição de parada falhou
    return 0;
}

void I2C_Ready()
{
    while(!SSPIF);                  // Aguarda a operação se completar
    SSPIF=0;                        // limpa o flag de interrupçao
}
