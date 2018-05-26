
#include <pic18f4550.h>
#include <stdio.h>
#include <stdlib.h>
#include "I2C_Master_File.h"
#include "MPU6050_res_define.h"
#include "Configuration_header_file.h"
#include "mylcd.h"
#include <xc.h>
#include <string.h>

void main()

{
    inic_XLCD();										//inicializa o LCD
    char select = 0;									// controle de informação do display
	char str1[5];
    char str2[5];
    char txt[10];
	int Ax,Ay,Az,T,Gx,Gy,Gz;							// valores interiros q serão lidos dos registradores do mpu
	float Xa,Ya,Za,t,Xg,Yg,Zg;							// valores  float usados para converter e escrever os dados no dysplay
    I2C_Init();											//inicializa a comunicação i2c	
	MPU6050_Init();										// inicializa o mpu
    
	TRISC = 0b00000001;									// portc sera a chave de seleção de informação
	while(1)
	{       
         if (SELECAO == 0){
        if((select)== 3){
            select = 0;
        }
        else{
            (select)=(select)+1;
        }
    }   
		MPU_Start_Loc();								// Read Gyro values continuously and send to terminal over USART 
		Ax = (((int)I2C_Read(0)<<8) | (int)I2C_Read(0));
		Ay = (((int)I2C_Read(0)<<8) | (int)I2C_Read(0));
		Az = (((int)I2C_Read(0)<<8) | (int)I2C_Read(0));
		T = (((int)I2C_Read(0)<<8) | (int)I2C_Read(0));
		Gx = (((int)I2C_Read(0)<<8) | (int)I2C_Read(0));
		Gy = (((int)I2C_Read(0)<<8) | (int)I2C_Read(0));
		Gz = (((int)I2C_Read(0)<<8) | (int)I2C_Read(1));
		I2C_Stop();
        		
		Xa = (float)Ax/16384.0;							 //Divide raw value by sensitivity scale factor to get real values 
		Ya = (float)Ay/16384.0;
		Za = (float)Az/16384.0;
		Xg = (float)Gx/131.0;
		Yg = (float)Gy/131.0;
		Zg = (float)Gz/131.0;
		t = ((float)T/340.00)+36.53;					 //Convert temperature in �/c using formula 
    
        if(select == 0){
            strcpy(str1,"Ax:\0");
             strcpy(str2,"Ay:\0");
            escreve(str1,str2,Xa,Ya);
        }
        else if(select == 1){
            strcpy(str1,"Az:\0");
             strcpy(str2,"Gx:\0");
            escreve(str1,str2,Za,Xg);
        }
          else if(select == 2){
            strcpy(str1,"Gy:\0");
             strcpy(str2,"Gz:\0");
            escreve(str1,str2,Yg,Zg);
        }
          else if(select == 3){
            while (BusyXLCD());
            SetDDRamAddr(0x02);
            while (BusyXLCD());
            putrsXLCD("TEMPERATURA");
            traduz(t, &txt); 
            while (BusyXLCD());
            SetDDRamAddr(0x44);
            while (BusyXLCD());
            putrsXLCD(txt);
             Mydelay(500);
            limpa();
        }

	}
}
//____________________________FUNÇOES IMPLEMENTADAS______________________________________//
void MPU6050_Init(){ // inicialização do giroscopio
	Mydelay(150);		 
	I2C_Start_Wait(0xD0);	 //Start with device write address 
	I2C_Write(SMPLRT_DIV);	 //Write to sample rate register 
	I2C_Write(0x07);	 //1KHz sample rate 
	I2C_Stop();

	I2C_Start_Wait(0xD0);
	I2C_Write(PWR_MGMT_1);	 //Write to power management register 
	I2C_Write(0x01);	 //X axis gyroscope reference frequency 
	I2C_Stop();

	I2C_Start_Wait(0xD0);
	I2C_Write(CONFIG);	//Write to Configuration register 
	I2C_Write(0x00);	 //Fs = 8KHz 
	I2C_Stop();

	I2C_Start_Wait(0xD0);
	I2C_Write(GYRO_CONFIG);	 //Write to Gyro configuration register 
	I2C_Write(0x18);	 //Full scale range +/- 2000 degree/C 
	I2C_Stop();

	I2C_Start_Wait(0xD0);
	I2C_Write(INT_ENABLE);	 //Write to interrupt enable register 
	I2C_Write(0x01);
	I2C_Stop();

}

void MPU_Start_Loc(){
	I2C_Start_Wait(0xD0);								 //I2C start with device write address 
	I2C_Write(ACCEL_XOUT_H);							 //Write start location address from where to read 
	I2C_Repeated_Start(0xD1);							// I2C start with device read address 
}
void Mydelay(int tempo){
   int i;
   for (i=0;i<tempo;i++)
   {
      __delay_ms(1);
   }
}
void inic_XLCD(void) { // Inicializa o LCD
    OpenXLCD(FOUR_BIT & LINES_5X7); // configura para 4-bit, 5x7, multiplas linhas
    while (BusyXLCD());
    WriteCmdXLCD(0x06); // move o cursor para direita depois de escrever 
    while (BusyXLCD());
    WriteCmdXLCD(0x0C); // display ligado com cursor deligado
    while (BusyXLCD());
}
void limpa(void) { // Limpa a tela do display
    while (BusyXLCD());
    WriteCmdXLCD(0x01); // comando para limpar a tela do display
    Mydelay(80); // Eh necessario dar um tempo depois de limpar
}
void traduz(float num, char *txt){
	float aux,pfloat;
   	int i,j,pint;
   	char stringh[10];
	j=1;
   	if (num < 0){  // se numero negativo coloca um caracter negativo
      aux = (num * -1);
      txt[0]= '-';
   	} 
   	else{ 			//senão coloca caracter positivo
       aux = num;
       txt[0]='+';  
   	}
	pint=(int) aux;          // parte inteira do numero
	pfloat = aux - (float) pint; // parte decimal do numero
	if(pint==0){       			// se parte inteira for 0, coloca '0' na segunda posição
		txt[1]='0';
	  j++; 
	}
	else{
		 i=0;
		while(pint > 0){	
			stringh[i] = (pint % 10) + 48;
			pint = pint / 10;
			i++;
		}
   		 i--;
    	for(; i>=0; i--,j++){
			txt[j] = stringh[i];
		  
    	}
     }
	txt[j]='.';				// coloca o ponto da parte float
	j++;	// ja posiciona na proxima casa vazia
 
	if (pfloat == 0 ){
		txt[j]= '0';
	   j++;
	}
	else
	   {
		i=0;
		while (pfloat < 1){
		   pfloat = pfloat * 10;
		   if(pfloat > 1){
		      stringh[i] = (((int)pfloat) % 10)+48;
		      i++;
		   }
		   else{
		      stringh[i]= '0';
		      i++;
		      
		   }
		}
		   	   
		   
		i--;
		for(int aux=0; aux<=i; j++,aux++){
			txt[j] = stringh[aux];
		  }
	       }
	txt[j]='\0';
    
}
void escreve (char str1,char str2,float N1, float N2){
        char txt[10];
        while (BusyXLCD());
    	SetDDRamAddr(0x00);
        while (BusyXLCD());
     	putsXLCD(str1);
		traduz(N1, &txt); 
        while (BusyXLCD());
    	SetDDRamAddr(0x03);
        while (BusyXLCD());
     	putsXLCD(txt);  
        while (BusyXLCD());
    	SetDDRamAddr(0x40);
        while (BusyXLCD());
     	putsXLCD(str2);
		traduz(N2, &txt); 
        while (BusyXLCD());
    	SetDDRamAddr(0x43);
        while (BusyXLCD());
     	putsXLCD(txt);
        Mydelay(500);
        limpa();
}
//////////////// Funcoes de tempo para a xlcd.h ///////////

void DelayFor18TCY(void) {
    __delay_us((8 * 18.0 * 4000000.0) / _XTAL_FREQ);
    // Seria necessario 18, mas na pratica e necessario aumentar o valor
    // Quanto maior a frequencia, maior o valor
}
void DelayPORXLCD(void) {
    __delay_ms(15);
}
void DelayXLCD(void) {
    __delay_ms(5);
}
//____________________________FIM DAS FUNÇOES IMPLEMENTADAS_________________________________//

