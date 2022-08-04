#include "nes_mapper.h"
 

Mapper1Res *MAP1;


// Mapper 1
void MAP1_Reset()
{  
  MAP1->write_count = 0;
  MAP1->bits = 0x00;
  MAP1->regs[0] = 0x0C; // reflects initial ROM state
  MAP1->regs[1] = 0x00;
  MAP1->regs[2] = 0x00;
  MAP1->regs[3] = 0x00;
  {
    uint32 size_in_K = num_8k_ROM_banks * 8;
    if(size_in_K == 1024)
    {
      MAP1->MMC1_Size = MMC1_1024K;
    }
    else if(size_in_K == 512)
    {
      MAP1->MMC1_Size = MMC1_512K;
    }
    else
    {
      MAP1->MMC1_Size = MMC1_SMALL;
    }
  }
  MAP1->MMC1_256K_base = 0; // use first 256K
  MAP1->MMC1_swap = 0;

  if(MAP1->MMC1_Size == MMC1_SMALL)
  {
    // set two high pages to last two banks
    MAP1->MMC1_HI1 = num_8k_ROM_banks-2;
    MAP1->MMC1_HI2 = num_8k_ROM_banks-1;
  }
  else
  {
    // set two high pages to last two banks of current 256K region
    MAP1->MMC1_HI1 = (256/8)-2;
    MAP1->MMC1_HI2 = (256/8)-1;
  }

  // set CPU bank pointers
  MAP1->MMC1_bank1 = 0;
  MAP1->MMC1_bank2 = 1;
  MAP1->MMC1_bank3 = MAP1->MMC1_HI1;
  MAP1->MMC1_bank4 = MAP1->MMC1_HI2;
 
}

void MAP1_MMC1_set_CPU_banks()
{
  set_CPU_banks((MAP1->MMC1_256K_base << 5) + (MAP1->MMC1_bank1 & ((256/8)-1)),
                (MAP1->MMC1_256K_base << 5) + (MAP1->MMC1_bank2 & ((256/8)-1)),
                (MAP1->MMC1_256K_base << 5) + (MAP1->MMC1_bank3 & ((256/8)-1)),
                (MAP1->MMC1_256K_base << 5) + (MAP1->MMC1_bank4 & ((256/8)-1)));
}  

void MAP1_MemoryWrite(uint16 addr, uint8 data)
{  
  uint32 reg_num;

  // if write is to a different reg, reset
  if((addr & 0x6000) != (MAP1->last_write_addr & 0x6000))
  {
    MAP1->write_count = 0;
    MAP1->bits = 0x00;
  }
  MAP1->last_write_addr = addr;

  // if bit 7 set, reset and return
  if(data & 0x80)
  {
    MAP1->write_count = 0;
    MAP1->bits = 0x00;
    return;
  }

  if(data & 0x01) MAP1->bits |= (1 << MAP1->write_count);
  MAP1->write_count++;
  if(MAP1->write_count < 5) return;

  reg_num = (addr & 0x7FFF) >> 13;
  MAP1->regs[reg_num] = MAP1->bits;

  MAP1->write_count = 0;
  MAP1->bits = 0x00;

//  LOG("MAP1 REG" << reg_num << ": " << HEX(MAP1->regs[reg_num],2) << endl);

  switch(reg_num)
  {
    case 0:
      {
//        LOG("REG0: " << HEX(reg[0],2) << endl);

        // set mirroring
        if(MAP1->regs[0] & 0x02)
        {
          if(MAP1->regs[0] & 0x01)
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
        else
        {
          // one-screen mirroring
          if(MAP1->regs[0] & 0x01)
          {
            set_mirroring(1,1,1,1);
          }
          else
          {
            set_mirroring(0,0,0,0);
          }
        }
      }
      break;

    case 1:
      {
        uint8 bank_num = MAP1->regs[1];

//        LOG("REG1: " << HEX(reg[1],2) << endl);

        if(MAP1->MMC1_Size == MMC1_1024K)
        {
          if(MAP1->regs[0] & 0x10)
          {
            if(MAP1->MMC1_swap)
            {
              MAP1->MMC1_256K_base = (MAP1->regs[1] & 0x10) >> 4;
              if(MAP1->regs[0] & 0x08)
              {
                MAP1->MMC1_256K_base |= ((MAP1->regs[2] & 0x10) >> 3);
              }
              MAP1_MMC1_set_CPU_banks();
              MAP1->MMC1_swap = 0;
            }
            else
            {
              MAP1->MMC1_swap = 1;
            }
          }
          else
          {
            // use 1st or 4th 256K banks
            MAP1->MMC1_256K_base = (MAP1->regs[1] & 0x10) ? 3 : 0;
            MAP1_MMC1_set_CPU_banks();
          }
        }
        else if((MAP1->MMC1_Size == MMC1_512K) && (!num_1k_VROM_banks))
        {
          MAP1->MMC1_256K_base = (MAP1->regs[1] & 0x10) >> 4;
          MAP1_MMC1_set_CPU_banks();
        }
        else if(num_1k_VROM_banks)
        {
          // set VROM bank at $0000
          if(MAP1->regs[0] & 0x10)
          {
            // swap 4K
            bank_num <<= 2;
            set_PPU_bank0(bank_num+0);
            set_PPU_bank1(bank_num+1);
            set_PPU_bank2(bank_num+2);
            set_PPU_bank3(bank_num+3);
          }
          else
          {
            // swap 8K
            bank_num <<= 2;
            set_PPU_banks(bank_num+0,bank_num+1,bank_num+2,bank_num+3,
                          bank_num+4,bank_num+5,bank_num+6,bank_num+7);
          }
        }
        else
        {
          if(MAP1->regs[0] & 0x10)
          {
            bank_num <<= 2;
            set_VRAM_bank(0, bank_num+0);
            set_VRAM_bank(1, bank_num+1);
            set_VRAM_bank(2, bank_num+2);
            set_VRAM_bank(3, bank_num+3);
          }
        }
      }
      break;

    case 2:
      {
        uint8 bank_num = MAP1->regs[2];

//        LOG("REG2: " << HEX(reg[2],2) << endl);

        if((MAP1->MMC1_Size == MMC1_1024K) && (MAP1->regs[0] & 0x08))
        {
          if(MAP1->MMC1_swap)
          {
            MAP1->MMC1_256K_base =  (MAP1->regs[1] & 0x10) >> 4;
            MAP1->MMC1_256K_base |= ((MAP1->regs[2] & 0x10) >> 3);
            MAP1_MMC1_set_CPU_banks();
            MAP1->MMC1_swap = 0;
          }
          else
          {
            MAP1->MMC1_swap = 1;
          }
        }

        if(!num_1k_VROM_banks)
        {
          if(MAP1->regs[0] & 0x10)
          {
            bank_num <<= 2;
            set_VRAM_bank(4, bank_num+0);
            set_VRAM_bank(5, bank_num+1);
            set_VRAM_bank(6, bank_num+2);
            set_VRAM_bank(7, bank_num+3);
            break;
          }
        }

        // set 4K VROM bank at $1000
        if(MAP1->regs[0] & 0x10)
        {
          // swap 4K
          bank_num <<= 2;
          set_PPU_bank4(bank_num+0);
          set_PPU_bank5(bank_num+1);
          set_PPU_bank6(bank_num+2);
          set_PPU_bank7(bank_num+3);
        }
      }
      break;

    case 3:
      {
        uint8 bank_num = MAP1->regs[3];

//        LOG("REG3: " << HEX(reg[3],2) << endl);

        // set ROM bank
        if(MAP1->regs[0] & 0x08)
        {
          // 16K of ROM
          bank_num <<= 1;

          if(MAP1->regs[0] & 0x04)
          {
            // 16K of ROM at $8000
            MAP1->MMC1_bank1 = bank_num;
            MAP1->MMC1_bank2 = bank_num+1;
            MAP1->MMC1_bank3 = MAP1->MMC1_HI1;
            MAP1->MMC1_bank4 = MAP1->MMC1_HI2;
          }
          else
          {
            // 16K of ROM at $C000
            if(MAP1->MMC1_Size == MMC1_SMALL)
            {
              MAP1->MMC1_bank1 = 0;
              MAP1->MMC1_bank2 = 1;
              MAP1->MMC1_bank3 = bank_num;
              MAP1->MMC1_bank4 = bank_num+1;
            }
          }
        }
        else
        {
          // 32K of ROM at $8000
          bank_num <<= 1; // thanx rinao

          MAP1->MMC1_bank1 = bank_num;
          MAP1->MMC1_bank2 = bank_num+1;
          if(MAP1->MMC1_Size == MMC1_SMALL)
          {
            MAP1->MMC1_bank3 = bank_num+2;
            MAP1->MMC1_bank4 = bank_num+3;
          }
        }

        MAP1_MMC1_set_CPU_banks();
      }
      break;
  }
}
void MAP1_Init()
{
	NES_Mapper->Reset=MAP1_Reset;
	NES_Mapper->Write=MAP1_MemoryWrite;
}


























