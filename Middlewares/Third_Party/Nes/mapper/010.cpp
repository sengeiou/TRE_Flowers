#include "nes_mapper.h"

 
void MAP10_set_VROM_1000(void);
void MAP10_set_VROM_0000(void);
 
// Mapper 10
void MAP10_Reset()
{
	int i;
//  // set CPU bank pointers
//  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  for(i = 0; i < sizeof(MAPx->regs)/sizeof(MAPx->regs[0]); i++)
    MAPx->regs[i] = 0;

  MAPx->regs[2] = 4;

  MAPx->irq_enabled = 0xFE;
  MAPx->irq_counter = 0xFE;

  MAP10_set_VROM_0000();
  MAP10_set_VROM_1000();
}

void MAP10_PPU_Latch_FDFE(uint32 addr)
{
  if(addr & 0x1000)
  {
    MAPx->irq_counter = (addr & 0x0FF0) >> 4;
    MAP10_set_VROM_1000();
  }
  else
  {
    MAPx->irq_enabled = (addr & 0x0FF0) >> 4;
    MAP10_set_VROM_0000();
  }
}

void MAP10_set_VROM_0000(void)
{
  int bank_num = (MAPx->irq_enabled == 0xFD) ? MAPx->regs[1] : MAPx->regs[2];

  bank_num <<= 2;

  set_PPU_bank0(bank_num+0);
  set_PPU_bank1(bank_num+1);
  set_PPU_bank2(bank_num+2);
  set_PPU_bank3(bank_num+3);
}

void MAP10_set_VROM_1000(void)
{
  int bank_num = (MAPx->irq_counter == 0xFD) ? MAPx->regs[3] : MAPx->regs[4];

  bank_num <<= 2;

  set_PPU_bank4(bank_num+0);
  set_PPU_bank5(bank_num+1);
  set_PPU_bank6(bank_num+2);
  set_PPU_bank7(bank_num+3);
}

void MAP10_MemoryWrite(uint16 addr, uint8 data)
{
	uint8 bank_num;
  switch(addr & 0xF000)
  {
    case 0xA000:
      {
        MAPx->regs[0] = data;

        // 16K ROM bank at $8000
        bank_num = MAPx->regs[0];
        set_CPU_bank4(bank_num*2);
        set_CPU_bank5(bank_num*2+1);
      }
      break;

    case 0xB000:
      {
        // B000-BFFF: select 4k VROM for (0000) $FD latch
        MAPx->regs[1] = data;
        MAP10_set_VROM_0000();
      }
      break;

    case 0xC000:
      {
        // C000-CFFF: select 4k VROM for (0000) $FE latch
        MAPx->regs[2] = data;
        MAP10_set_VROM_0000();
      }
      break;

    case 0xD000:
      {
        // D000-DFFF: select 4k VROM for (1000) $FD latch
        MAPx->regs[3] = data;
        MAP10_set_VROM_1000();
      }
      break;

    case 0xE000:
      {
        // E000-EFFF: select 4k VROM for (1000) $FE latch
        MAPx->regs[4] = data;
        MAP10_set_VROM_1000();
      }
      break;

    case 0xF000:
      {
        MAPx->regs[5] = data;

        if(MAPx->regs[5] & 0x01)
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

void MAP10_Init()
{
	NES_Mapper->Reset = MAP10_Reset;
	NES_Mapper->Write = MAP10_MemoryWrite;  
}



