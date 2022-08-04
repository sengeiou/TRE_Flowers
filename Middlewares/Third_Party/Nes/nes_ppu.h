#ifndef __NES_PPU_H
#define __NES_PPU_H 
#include "nes_main.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序移植自网友ye781205的NES模拟器工程
//ALIENTEK STM32F407开发板
//NES APU 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/7/1
//版本：V1.0  			  
////////////////////////////////////////////////////////////////////////////////// 	 


#undef   NULL
#define  NULL 0 

#ifndef  TRUE
#define  TRUE     1
#endif
#ifndef  FALSE
#define  FALSE    0
#endif



static const uint16_t NES_Palette[64]=
{
	0x73AE,0x20D1,0x0015,0x4013,0x880E,0xA802,0xA000,0x7840,
	0x4160,0x0220,0x0280,0x01E2,0x19EB,0x0000,0x0000,0x0000,
	0xBDF7,0x039D,0x21DD,0x801E,0xB817,0xE00B,0xD940,0xCA61,
	0x8B80,0x04A0,0x0540,0x0487,0x0411,0x0000,0x0000,0x0000,
	0xF79E,0x3DFF,0x5CBF,0xA45F,0xF3DF,0xFBB6,0xFBAC,0xFCC7,
	0xF5E7,0x8682,0x4EE9,0x5FD3,0x075B,0x0000,0x0000,0x0000,
	0xF79E,0xAF3F,0xC6BF,0xD65F,0xFE3F,0xFE3B,0xFDF6,0xFED5,
	0xFF34,0xE7F4,0xAF97,0xB7F9,0x9FFE,0x0000,0x0000,0x0000
};

extern  u8* VROM_banks;       //VROM开始地址 图案表*************************************
extern  u8* VROM_tiles;
extern  uint8* PPU_VRAM_banks[12];
extern  uint8* PPU_tile_banks[8];
extern uint8* PPU_patterntables;       //8192//VROM开始地址 图案表*************************************
extern uint8* spr_ram;   			//sprite ram

enum 
{ 
	NES_SCREEN_WIDTH  = 256,
	NES_SCREEN_HEIGHT = 240,
	SIDE_MARGIN = 8,
	NES_SCREEN_WIDTH_VIEWABLE  = NES_SCREEN_WIDTH,
	NES_BACKBUF_WIDTH = NES_SCREEN_WIDTH + (2*SIDE_MARGIN)
};
enum
{ 
	BG_WRITTEN_FLAG = 0x01, 
	SPR_WRITTEN_FLAG = 0x02 
}; 

typedef struct
{
	uint32  in_vblank;
	uint32  current_frame_line;
	uint16  bg_pattern_table_addr;
    uint16  spr_pattern_table_addr;
	uint16  ppu_addr_inc;
	uint16  loopy_v;  // vram address -- used for reading/writing through $2007
                    // see loopy-2005.txt
    uint16  loopy_t;  // temp vram address
    uint8   loopy_x;  // 3-bit subtile x-offset
	uint8   toggle_2005_2006;
    uint8   spr_ram_rw_ptr;  // sprite ram read/write pointer内存读/写指针
    uint8   read_2007_buffer; 
	uint8 LowRegs[0x08];
	uint8 bg_pal[0x10];  //extern BYTE BGPal[0x20];			//背景调色板
    uint8 spr_pal[0x10]; //extern BYTE SPPal[0x20];			//精灵调色板

	uint8 PPU_nametables[4*0x400];	//4096  PPU真正的的RAM
	// Rick
	uint8* PPU_VRAM_banks[12];
	uint8* PPU_tile_banks[8]; 
	uint8* PPU_tile_tables;  
	uint8 solid_buf[NES_BACKBUF_WIDTH];    // 当前像素的位标志线
	uint8 dummy_buffer[NES_BACKBUF_WIDTH]; // used to do sprite 0 hit detection when we aren't supposed to draw
	
	
} ppu_data;  


extern ppu_data *ppu;  
extern u8 *spr_ram;  
  


uint32 spr_enabled(void);
uint32 bg_enabled(void);
void set_name_table(uint8 bank, int bank_num);
void set_tile_bank(int i, uint8 *bank);
void compile(int count, uint8 *src, uint8 *dest);
void set_tile_banks(uint8 *bank0, uint8 *bank1, uint8 *bank2, uint8 *bank3,
uint8 *bank4, uint8 *bank5, uint8 *bank6, uint8 *bank7);
void PPU_reset(void);
void PPU_start_frame(void);
void set_mirroring(uint32 nt0, uint32 nt1, uint32 nt2, uint32 nt3);//设置垂直水平镜像
uint8 ReadLowRegs(uint32 addr);
void  WriteLowRegs(uint32 addr, uint8 data);
void scanline_draw(int LineNo);
void do_scanline_and_draw(uint8* buf);

// Rick
void start_frame(uint8 *buf, int ypitch);
void end_frame(uint8 *buf);
void do_scanline_and_dont_draw(int LineNo);
uint32 NMI_enabled(void);
void start_vblank(void);
void end_vblank(void);
void render_bg(uint8* buf);
void render_spr(uint8* buf);
void update_tile(int byteOffset, uint8 data);

void ppu_close(void);
#endif









