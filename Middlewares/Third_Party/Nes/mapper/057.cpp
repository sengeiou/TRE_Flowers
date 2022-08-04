#ifdef _NES_MAPPER_CPP_

/////////////////////////////////////////////////////////////////////
// Mapper 57
void NES_mapper57::Reset()
{
  set_CPU_banks(0,1,0,1);
  set_PPU_banks(0,1,2,3,4,5,6,7);

  regs[0] = 0;
}

void NES_mapper57::MemoryWrite(uint32 addr, uint8 data)
{
  switch(addr)
  {
    case 0x8000:
    case 0x8001:
    case 0x8002:
    case 0x8003:
      {
        if(data & 0x40)
        {
          set_PPU_bank0(8 * ((data & 0x03) + ((regs[0] & 0x10) >> 1) + (regs[0] & 0x07)) + 0);
          set_PPU_bank1(8 * ((data & 0x03) + ((regs[0] & 0x10) >> 1) + (regs[0] & 0x07)) + 1);
          set_PPU_bank2(8 * ((data & 0x03) + ((regs[0] & 0x10) >> 1) + (regs[0] & 0x07)) + 2);
          set_PPU_bank3(8 * ((data & 0x03) + ((regs[0] & 0x10) >> 1) + (regs[0] & 0x07)) + 3);
          set_PPU_bank4(8 * ((data & 0x03) + ((regs[0] & 0x10) >> 1) + (regs[0] & 0x07)) + 4);
          set_PPU_bank5(8 * ((data & 0x03) + ((regs[0] & 0x10) >> 1) + (regs[0] & 0x07)) + 5);
          set_PPU_bank6(8 * ((data & 0x03) + ((regs[0] & 0x10) >> 1) + (regs[0] & 0x07)) + 6);
          set_PPU_bank7(8 * ((data & 0x03) + ((regs[0] & 0x10) >> 1) + (regs[0] & 0x07)) + 7);
        }
      }
      break;

    case 0x8800:
      {
        regs[0] = data;

        if(data & 0x80)
        {
          set_CPU_bank4(4 * ((data & 0x40) >> 6) + 8 + 0);
          set_CPU_bank5(4 * ((data & 0x40) >> 6) + 8 + 1);
          set_CPU_bank6(4 * ((data & 0x40) >> 6) + 8 + 2);
          set_CPU_bank7(4 * ((data & 0x40) >> 6) + 8 + 3);
        }
        else
        {
          set_CPU_bank4(2 * ((data & 0x60) >> 5) + 0);
          set_CPU_bank5(2 * ((data & 0x60) >> 5) + 1);
          set_CPU_bank6(2 * ((data & 0x60) >> 5) + 0);
          set_CPU_bank7(2 * ((data & 0x60) >> 5) + 1);
        }

        set_PPU_bank0(8 * ((data & 0x07) + ((data & 0x10) >> 1)) + 0);
        set_PPU_bank1(8 * ((data & 0x07) + ((data & 0x10) >> 1)) + 1);
        set_PPU_bank2(8 * ((data & 0x07) + ((data & 0x10) >> 1)) + 2);
        set_PPU_bank3(8 * ((data & 0x07) + ((data & 0x10) >> 1)) + 3);
        set_PPU_bank4(8 * ((data & 0x07) + ((data & 0x10) >> 1)) + 4);
        set_PPU_bank5(8 * ((data & 0x07) + ((data & 0x10) >> 1)) + 5);
        set_PPU_bank6(8 * ((data & 0x07) + ((data & 0x10) >> 1)) + 6);
        set_PPU_bank7(8 * ((data & 0x07) + ((data & 0x10) >> 1)) + 7);

        if(data & 0x08)
        {
          set_mirroring(NES_PPU::MIRROR_HORIZ);
        }
        else
        {
          set_mirroring(NES_PPU::MIRROR_VERT);
        }
      }
      break;
  }
}
/////////////////////////////////////////////////////////////////////

#endif
