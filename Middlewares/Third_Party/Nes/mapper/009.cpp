#include "nes_mapper.h"



void MAP9_set_VROM_1000(void);
void MAP9_set_VROM_0000(void);
uint8 MAP9_regs[6];
uint8 MAP9_latch_0000;
uint8 MAP9_latch_1000;
  
/////////////////////////////////////////////////////////////////////
// Mapper 9
void MAP9_Reset()
{
	int i;
//  // set CPU bank pointers
//  set_CPU_banks(0,num_8k_ROM_banks-3,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  for( i = 0; i < sizeof(MAP9_regs)/sizeof(MAP9_regs[0]); i++)
    MAP9_regs[i] = 0;

  MAP9_regs[2] = 4;

  MAP9_latch_0000 = 0xFE;
  MAP9_latch_1000 = 0xFE;

  MAP9_set_VROM_0000();
  MAP9_set_VROM_1000();
}

void MAP9_PPU_Latch_FDFE(uint32 addr)
{
  if(addr & 0x1000)
  {
    MAP9_latch_1000 = (addr & 0x0FF0) >> 4;
    MAP9_set_VROM_1000();
  }
  else
  {
    MAP9_latch_0000 = (addr & 0x0FF0) >> 4;
    MAP9_set_VROM_0000();
  }
}

void MAP9_set_VROM_0000()
{
  int bank_num = (MAP9_latch_0000 == 0xFD) ? MAP9_regs[1] : MAP9_regs[2];

  bank_num <<= 2;

  set_PPU_bank0(bank_num+0);
  set_PPU_bank1(bank_num+1);
  set_PPU_bank2(bank_num+2);
  set_PPU_bank3(bank_num+3);
}

void MAP9_set_VROM_1000()
{
  int bank_num = (MAP9_latch_1000 == 0xFD) ? MAP9_regs[3] : MAP9_regs[4];

  bank_num <<= 2;

  set_PPU_bank4(bank_num+0);
  set_PPU_bank5(bank_num+1);
  set_PPU_bank6(bank_num+2);
  set_PPU_bank7(bank_num+3);
}

void MAP9_MemoryWrite(uint16 addr, uint8 data)
{
	uint8 bank_num ;
  switch(addr & 0xF000)
  {
    case 0xA000:
      {
        MAP9_regs[0] = data;

        // 8K ROM bank at $8000
        bank_num = MAP9_regs[0];
        set_CPU_bank4(bank_num);
      }
      break;

    case 0xB000:
      {
        // B000-BFFF: select 4k VROM for (0000) $FD latch
        MAP9_regs[1] = data;
        MAP9_set_VROM_0000();
      }
      break;

    case 0xC000:
      {
        // C000-CFFF: select 4k VROM for (0000) $FE latch
        MAP9_regs[2] = data;
        MAP9_set_VROM_0000();
      }
      break;

    case 0xD000:
      {
        // D000-DFFF: select 4k VROM for (1000) $FD latch
        MAP9_regs[3] = data;
        MAP9_set_VROM_1000();
      }
      break;

    case 0xE000:
      {
        // E000-EFFF: select 4k VROM for (1000) $FE latch
        MAP9_regs[4] = data;
        MAP9_set_VROM_1000();
      }
      break;

    case 0xF000:
      {
        MAP9_regs[5] = data;

        if(MAP9_regs[5] & 0x01)
        {
             set_mirroring(0,0,1,1);//Ë®Æ½¾µÏñInfoNES_Mirroring( 0 );   //0011
             //set_mirroring(NES_PPU::MIRROR_HORIZ);
        }
        else
        {
             set_mirroring(0,1,0,1);//´¹Ö±¾µÏñInfoNES_Mirroring( 1 );   //0101
             //set_mirroring(NES_PPU::MIRROR_VERT);
        }
      }
      break;
  }
} 
void MAP9_SNSS_fixup()
{
  MAP9_set_VROM_0000();
  MAP9_set_VROM_1000();
}
void MAP9_Init()
{
	NES_Mapper->Reset = MAP9_Reset;
	NES_Mapper->Write = MAP9_MemoryWrite;  
}
























