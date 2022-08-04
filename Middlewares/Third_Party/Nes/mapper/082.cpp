#ifdef _NES_MAPPER_CPP_

/////////////////////////////////////////////////////////////////////
// Mapper 82
void NES_mapper82::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  // set PPU bank pointers
  if(num_1k_VROM_banks)
  {
    set_PPU_banks(0,1,2,3,4,5,6,7);
  }

  // set Mirroring
  set_mirroring(NES_PPU::MIRROR_VERT);

  regs[0] = 0;
}

void NES_mapper82::MemoryWriteSaveRAM(uint32 addr, uint8 data)
{
  switch (addr)
  {
    case 0x7EF0:
      /* Switch 2k VROM at $0000 or $1000 */
      {
        if(regs[0])
        {
          set_PPU_bank4((data & 0xFE)+0);
          set_PPU_bank5((data & 0xFE)+1);
        }
        else
        {
          set_PPU_bank0((data & 0xFE)+0);
          set_PPU_bank1((data & 0xFE)+1);
        }
      }
      break;

    case 0x7EF1:
      {
        if(regs[0])
        {
          set_PPU_bank6((data & 0xFE)+0);
          set_PPU_bank7((data & 0xFE)+1);
        }
        else
        {
          set_PPU_bank2((data & 0xFE)+0);
          set_PPU_bank3((data & 0xFE)+1);
        }
      }
      break;

    case 0x7EF2:
      {
        if(!regs[0])
        {
          set_PPU_bank4(data);
        }
        else
        {
          set_PPU_bank0(data);
        }
      }
      break;

    case 0x7EF3:
      {
        if(!regs[0])
        {
          set_PPU_bank5(data);
        }
        else
        {
          set_PPU_bank1(data);
        }
      }
      break;

    case 0x7EF4:
      {
        if(!regs[0])
        {
          set_PPU_bank6(data);
        }
        else
        {
          set_PPU_bank2(data);
        }
      }
      break;

    case 0x7EF5:
      {
        if(!regs[0])
        {
          set_PPU_bank7(data);
        }
        else
        {
          set_PPU_bank3(data);
        }
      }
      break;

    case 0x7EF6:
      {
        regs[0] = data & 0x02;
        if(data & 0x01)
        {
          set_mirroring(NES_PPU::MIRROR_VERT);
        }
        else
        {
          set_mirroring(NES_PPU::MIRROR_HORIZ);
        }
      }
      break;

    case 0x7EFA:
      {
        set_CPU_bank4(data >> 2);
      }
      break;

    case 0x7EFB:
      {
        set_CPU_bank5(data >> 2);
      }
      break;

    case 0x7EFC:
      {
        set_CPU_bank6(data >> 2);
      }
      break;
  }
}
/////////////////////////////////////////////////////////////////////

#endif
