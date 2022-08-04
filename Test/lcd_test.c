#include "lcd_test.h"
#include "lcd_2inch.h"
#include "lcd.h"

#define delay_ms(ms) HAL_Delay(ms)

u16 ColorTab[5]={RED,GREEN,BLUE,YELLOW,BRED};//定义颜色数组

void Test_Triangle(void)
{
	LCD_Clear(WHITE);
	u8 i=0;
	LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE);
	for(i=0;i<5;i++)
	{
		POINT_COLOR=ColorTab[i];
		Draw_Triangel(lcddev.width/2-40+(i*15),lcddev.height/2-12+(i*11),lcddev.width/2-25-1+(i*15),lcddev.height/2-12-26-1+(i*11),lcddev.width/2-10-1+(i*15),lcddev.height/2-12+(i*11));
	}
	delay_ms(1500);	
	LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE); 
	for(i=0;i<5;i++)
	{
		POINT_COLOR=ColorTab[i];
		Fill_Triangel(lcddev.width/2-40+(i*15),lcddev.height/2-12+(i*11),lcddev.width/2-25-1+(i*15),lcddev.height/2-12-26-1+(i*11),lcddev.width/2-10-1+(i*15),lcddev.height/2-12+(i*11));
	}
	delay_ms(1500);
}

void Test_Color(void)
{
	LCD_Clear(WHITE);
	LCD_Fill(0,0,lcddev.width,lcddev.height,WHITE);
	Show_Str(20,30,BLUE,YELLOW,"BL Test",16,1);delay_ms(800);
	LCD_Fill(0,0,lcddev.width,lcddev.height,RED);
	Show_Str(20,30,BLUE,YELLOW,"RED ",16,1);delay_ms(800);
	LCD_Fill(0,0,lcddev.width,lcddev.height,GREEN);
	Show_Str(20,30,BLUE,YELLOW,"GREEN ",16,1);delay_ms(800);
	LCD_Fill(0,0,lcddev.width,lcddev.height,BLUE);
	Show_Str(20,30,RED,YELLOW,"BLUE ",16,1);delay_ms(800);
}

void Test_FillRec(void)
{
	u8 i=0;
	LCD_Clear(WHITE);
	LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE);
	for (i=0; i<5; i++) 
	{
		POINT_COLOR=ColorTab[i];
		LCD_DrawRectangle(lcddev.width/2-40+(i*16),lcddev.height/2-40+(i*13),lcddev.width/2-40+(i*16)+30,lcddev.height/2-40+(i*13)+30); 
	}
	delay_ms(1500);	
	LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE); 
	for (i=0; i<5; i++) 
	{
		POINT_COLOR=ColorTab[i];
		LCD_DrawFillRectangle(lcddev.width/2-40+(i*16),lcddev.height/2-40+(i*13),lcddev.width/2-40+(i*16)+30,lcddev.height/2-40+(i*13)+30); 
	}
	delay_ms(1500);
}

void Test_Circle(void)
{
	u8 i=0;
	LCD_Clear(WHITE);
	LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE);
	for (i=0; i<5; i++)  
		gui_circle(lcddev.width/2-40+(i*15),lcddev.height/2-25+(i*13),ColorTab[i],15,0);
	delay_ms(1500);	
	LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE); 
	for (i=0; i<5; i++) 
	  	gui_circle(lcddev.width/2-40+(i*15),lcddev.height/2-25+(i*13),ColorTab[i],15,1);
	delay_ms(1500);
}

void English_Font_test(void)
{
	LCD_Clear(WHITE);
	Show_Str(10,22,BLUE,YELLOW,"6X12:abcdefgh01234567",12,0);
	Show_Str(10,34,BLUE,YELLOW,"6X12:ABCDEFGH01234567",12,1);
	Show_Str(10,47,BLUE,YELLOW,"6X12:~!@#$%^&*()_+{}:",12,0);
	Show_Str(10,60,BLUE,YELLOW,"8X16:abcde01234",16,0);
	Show_Str(10,76,BLUE,YELLOW,"8X16:ABCDE01234",16,1);
	Show_Str(10,92,BLUE,YELLOW,"8X16:~!@#$%^&*()",16,0); 
	delay_ms(1200);
}

