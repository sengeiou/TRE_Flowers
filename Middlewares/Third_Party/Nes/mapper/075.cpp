#include "nes_mapper.h"

u8 map75_regs[2];

// Mapper 75
void MAP75_Reset()
{
  // set CPU bank pointers
//  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  // set PPU bank pointers
  if(num_1k_VROM_banks)
  {
    set_PPU_banks(0,1,2,3,4,5,6,7);
  }

  map75_regs[0] = 0;
  map75_regs[1] = 1;
}

void MAP75_MemoryWrite(uint16 addr, uint8 data)
{
  switch(addr & 0xF000)
  {
    case 0x8000:
      {
        set_CPU_bank4(data);
      }
      break;

    case 0x9000:
      {
        if(data & 0x01)
        {
             set_mirroring(0,0,1,1);//Ë®Æ½¾µÏñInfoNES_Mirroring( 0 );   //0011
             //set_mirroring(NES_PPU::MIRROR_HORIZ);
        }
        else
        {
             set_mirroring(0,1,0,1);//´¹Ö±¾µÏñInfoNES_Mirroring( 1 );   //0101
             //set_mirroring(NES_PPU::MIRROR_VERT);
        }
        map75_regs[0] = (map75_regs[0] & 0x0F) | ((data & 0x02) << 3);
        set_PPU_bank0(map75_regs[0]*4+0);
        set_PPU_bank1(map75_regs[0]*4+1);
        set_PPU_bank2(map75_regs[0]*4+2);
        set_PPU_bank3(map75_regs[0]*4+3);
        map75_regs[1] = (map75_regs[1] & 0x0F) | ((data & 0x04) << 2);
        set_PPU_bank4(map75_regs[1]*4+0);
        set_PPU_bank5(map75_regs[1]*4+1);
        set_PPU_bank6(map75_regs[1]*4+2);
        set_PPU_bank7(map75_regs[1]*4+3);
      }
      break;

    case 0xA000:
      {
        set_CPU_bank5(data);
      }
      break;

    case 0xC000:
      {
        set_CPU_bank6(data);
      }
      break;

    case 0xE000:
      {
        map75_regs[0] = (map75_regs[0] & 0x10) | (data & 0x0F);
        set_PPU_bank0(map75_regs[0]*4+0);
        set_PPU_bank1(map75_regs[0]*4+1);
        set_PPU_bank2(map75_regs[0]*4+2);
        set_PPU_bank3(map75_regs[0]*4+3);
      }
      break;

    case 0xF000:
      {
        map75_regs[1] = (map75_regs[1] & 0x10) | (data & 0x0F);
        set_PPU_bank4(map75_regs[1]*4+0);
        set_PPU_bank5(map75_regs[1]*4+1);
        set_PPU_bank6(map75_regs[1]*4+2);
        set_PPU_bank7(map75_regs[1]*4+3);
      }
      break;
  }
}
void MAP75_Init()
{
	NES_Mapper->Reset = MAP75_Reset;
	NES_Mapper->Write = MAP75_MemoryWrite; 
}



