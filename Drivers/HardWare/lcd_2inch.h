#ifndef _LCD_2INCH_H
#define _LCD_2INCH_H

#include "sys.h"
#include "gpio.h"
#include "math.h"
#include "spi.h"

typedef struct  
{										    
	u16 width;			//LCD 宽度
	u16 height;			//LCD 高度
	u16 id;				//LCD ID
	u8  dir;			//横屏还是竖屏控制：0，竖屏；1，横屏。	
	u16	 wramcmd;		//开始写gram指令
	u16  setxcmd;		//设置x坐标指令
	u16  setycmd;		//设置y坐标指令	
}_lcd_dev;

//参数结构体
extern _lcd_dev lcddev;

//定义液晶屏顺时针旋转方向 	0-0度旋转，1-90度旋转，2-180度旋转，3-270度旋转
#define USE_HORIZONTAL  	 1 

#define LCD_W 240
#define LCD_H 320

extern u16  POINT_COLOR;//默认红色    
extern u16  BACK_COLOR; //背景颜色.默认为白色

////////////////////////////////////////////////////////////////////
//-----------------LCD端口定义---------------- 

#define LED  15        //背光控制引脚//BLK
#define CS   4       //片选引脚//CS
#define RS   5       //寄存器/数据选择引脚//DC
#define RST  13       //复位引脚//RES

//QDtech全系列模块采用了三极管控制背光亮灭，用户也可以接PWM调节背光亮度
#define LCD_LED_MAX 10
#define LCD_LED_MIN 0

#define LCD_LED_PWM_ENABLE HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_2)
#define LCD_LED_PWM_DISABLE HAL_TIM_PWM_Stop(&htim12, TIM_CHANNEL_2)

#define	LCD_LED PBout(LED) //LCD背光    		 
#define LCD_CS  PCout(CS)
#define LCD_RS  PCout(RS)
#define LCD_RST PBout(RST)
//如果使用官方库函数定义下列底层，速度将会下降到14帧每秒，建议采用我司推荐方法
//以下IO定义直接操作寄存器，快速IO操作，刷屏速率可以达到28帧每秒！ 

#define	LCD_CS_SET  LCD_CS=1 //GPIO_TYPE->BSRRL=1<<LCD_CS    //片选端口
#define	LCD_RS_SET	LCD_RS=1 //GPIO_TYPE->BSRRL=1<<LCD_RS    //数据/命令 
#define	LCD_RST_SET	LCD_RST=1 //GPIO_TYPE->BSRRL=1<<LCD_RST    //复位	

 							    
#define	LCD_CS_CLR  LCD_CS=0 //GPIO_TYPE->BSRRH=1<<LCD_CS     //片选端口 
#define	LCD_RS_CLR	LCD_RS=0 //GPIO_TYPE->BSRRH=1<<LCD_RS     //数据/命令 
#define	LCD_RST_CLR	LCD_RST=0 //GPIO_TYPE->BSRRH=1<<LCD_RST    //复位	

#define LCD_WRITE_BYTE(pdata, len) HAL_SPI_Transmit(lcd_spi, pdata, len, 1000);
#define LCD_WRITE_PACK(pdata, len) HAL_SPI_Transmit_DMA(lcd_spi, pdata, len);

//画笔颜色
#define WHITE       0xFFFF
#define BLACK      	0x0000	  
#define BLUE       	0x001F  
#define BRED        0XF81F
#define GRED 			 	0XFFE0
#define GBLUE			 	0X07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define BROWN 			0XBC40 //棕色
#define BRRED 			0XFC07 //棕红色
#define GRAY  			0X8430 //灰色
#define GRAY0       0xEF7D 
#define GRAY1       0x8410      	//灰色1      00000 000000 00000
#define GRAY2       0x4208 
//GUI颜色

#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN     	0X841F //浅绿色
#define LIGHTGRAY     0XEF5B //浅灰色(PANNEL)
#define LGRAY 			 		0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE      	0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE          0X2B12 //浅棕蓝色(选择条目的反色)
	    															  
void LCD_Init(SPI_HandleTypeDef *spi);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_SetBlk(uint8_t blk);
void LCD_Clear(u16 Color);	 
void LCD_SetCursor(u16 Xpos, u16 Ypos);
void LCD_DrawPoint(u16 x,u16 y);//画点
u16  LCD_ReadPoint(u16 x,u16 y); //读点
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);		   
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd);
void LCD_DisplayWindows(u16 Xstart, u16 Ystart, u16 Xend, u16 Yend, u16 *Image);

u16 LCD_RD_DATA(void);//读取LCD数据									    
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue);
void LCD_WR_DATA(u8 data);
u16 LCD_ReadReg(u8 LCD_Reg);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(u16 RGB_Code);
u16 LCD_ReadRAM(void);		   
u16 LCD_BGR2RGB(u16 c);
void LCD_SetParam(void);
void Lcd_WriteData_16Bit(u16 Data);
void LCD_direction(u8 direction );

#endif
