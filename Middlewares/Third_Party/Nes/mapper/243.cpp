#ifdef _NES_MAPPER_CPP_

/////////////////////////////////////////////////////////////////////
// Mapper 243
void NES_mapper243::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(0,1,2,3);

  // set PPU bank pointers
  if(num_1k_VROM_banks > 32)
  {
    set_PPU_banks(24,25,26,27,28,29,30,31);
  }
  set_mirroring(NES_PPU::MIRROR_HORIZ);

   regs[0] = 0;
   regs[1] = 0;
   regs[2] = 3;
   regs[3] = 0;
}

void NES_mapper243::MemoryWriteLow(uint32 addr, uint8 data)
{
  if((addr & 0x4101) == 0x4100)
  {
    regs[0] = data;
  }
  else if((addr & 0x4101) == 0x4101)
  {
    switch(regs[0] & 0x07)
    {
      case 0:
        {
          regs[1] = 0;
          regs[2] = 3;
        }
        break;

      case 4:
        {
          regs[2] = (regs[2] & 0x06) |  (data & 0x01);
        }
        break;

      case 5:
        {
          regs[1] = data & 0x01;
        }
        break;

      case 6:
        {
          regs[2] = (regs[2] & 0x01) | ((data & 0x03) << 1);
        }
        break;

      case 7:
        {
          regs[3] = data & 1;
        }
        break;
    }
    
    set_CPU_banks(regs[1]*4+0, regs[1]*4+1, regs[1]*4+2, regs[1]*4+3);
    set_PPU_banks(regs[2]*8+0, regs[2]*8+1, regs[2]*8+2, regs[2]*8+3,
                  regs[2]*8+4, regs[2]*8+5, regs[2]*8+6, regs[2]*8+7);
    if(regs[3])
    {
      set_mirroring(NES_PPU::MIRROR_VERT);
    }
    else
    {
      set_mirroring(NES_PPU::MIRROR_HORIZ);
    }
  }
}

void NES_mapper243::MemoryWriteSaveRAM(uint32 addr, uint8 data)
{
  if((addr & 0x4101) == 0x4100)
  {
    regs[0] = data;
  }
  else if((addr & 0x4101) == 0x4101)
  {
    switch(regs[0] & 0x07)
    {
      case 0:
        {
          regs[1] = 0;
          regs[2] = 3;
        }
        break;

      case 4:
        {
          regs[2] = (regs[2] & 0x06) |  (data & 0x01);
        }
        break;

      case 5:
        {
          regs[1] = data & 0x01;
        }
        break;

      case 6:
        {
          regs[2] = (regs[2] & 0x01) | ((data & 0x03) << 1);
        }
        break;

      case 7:
        {
          regs[3] = data & 1;
        }
        break;
    }
    
    set_CPU_banks(regs[1]*4+0, regs[1]*4+1, regs[1]*4+2, regs[1]*4+3);
    set_PPU_banks(regs[2]*8+0, regs[2]*8+1, regs[2]*8+2, regs[2]*8+3,
                  regs[2]*8+4, regs[2]*8+5, regs[2]*8+6, regs[2]*8+7);
    if(regs[3])
    {
      set_mirroring(NES_PPU::MIRROR_VERT);
    }
    else
    {
      set_mirroring(NES_PPU::MIRROR_HORIZ);
    }
  }
}
/////////////////////////////////////////////////////////////////////

#endif
