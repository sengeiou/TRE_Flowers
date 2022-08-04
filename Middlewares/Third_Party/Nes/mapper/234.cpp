
/////////////////////////////////////////////////////////////////////
// Mapper 234
void NES_mapper234::Reset()
{
  regs[0] = regs[1] = regs[2] = 0;
  Sync();
}

void NES_mapper234::MemoryReadSaveRAM(uint32 addr)
{
  NES_6502::Context context;
  parent_NES->cpu->GetContext(&context);
  uint8 data = context.mem_page[addr >> 13][addr & 0x1fff];

  switch(addr & 0xFFF8)
  {
    case 0xFF80:
    case 0xFF88:
    case 0xFF90:
    case 0xFF98:
      {
        if(!regs[0])
        {
          regs[0] = data;
          Sync();
        }
      }
      break;

    case 0xFFC0:
    case 0xFFC8:
    case 0xFFD0:
    case 0xFFD8:
      {
        if(!regs[2])
        {
          regs[2] = data;
          Sync();
        }
      }
      break;

    case 0xFFE8:
    case 0xFFF0:
      {
        regs[1] = data;
      }
      break;
  }
}

void NES_mapper234::MemoryWrite(uint32 addr, uint8 data)
{
  switch(addr & 0xFFF8)
  {
    case 0xFF80:
    case 0xFF88:
    case 0xFF90:
    case 0xFF98:
      {
        if(!regs[0])
        {
          regs[0] = data;
          Sync();
        }
      }
      break;

    case 0xFFC0:
    case 0xFFC8:
    case 0xFFD0:
    case 0xFFD8:
      {
        if(!regs[2])
        {
          regs[2] = data;
          Sync();
        }
      }
      break;

    case 0xFFE8:
    case 0xFFF0:
      {
        regs[1] = data;
      }
      break;
  }
}

void NES_mapper234::Sync()
{
  if(regs[0] & 0x80)
  {
    set_mirroring(NES_PPU::MIRROR_HORIZ);
  }
  else
  {
    set_mirroring(NES_PPU::MIRROR_VERT);
  }
  if (regs[0] & 0x40)
  {
    set_CPU_bank4(((regs[0] & 0x0E)|(regs[1] & 0x01))*4+0);
    set_CPU_bank5(((regs[0] & 0x0E)|(regs[1] & 0x01))*4+1);
    set_CPU_bank6(((regs[0] & 0x0E)|(regs[1] & 0x01))*4+2);
    set_CPU_bank7(((regs[0] & 0x0E)|(regs[1] & 0x01))*4+3);
    set_PPU_bank0((((regs[0] & 0x0E)<<2)|((regs[1] & 0x70)>>4))*8+0);
    set_PPU_bank1((((regs[0] & 0x0E)<<2)|((regs[1] & 0x70)>>4))*8+1);
    set_PPU_bank2((((regs[0] & 0x0E)<<2)|((regs[1] & 0x70)>>4))*8+2);
    set_PPU_bank3((((regs[0] & 0x0E)<<2)|((regs[1] & 0x70)>>4))*8+3);
    set_PPU_bank4((((regs[0] & 0x0E)<<2)|((regs[1] & 0x70)>>4))*8+4);
    set_PPU_bank5((((regs[0] & 0x0E)<<2)|((regs[1] & 0x70)>>4))*8+5);
    set_PPU_bank6((((regs[0] & 0x0E)<<2)|((regs[1] & 0x70)>>4))*8+6);
    set_PPU_bank7((((regs[0] & 0x0E)<<2)|((regs[1] & 0x70)>>4))*8+7);
  }
  else
  {
    set_CPU_bank4((regs[0] & 0x0F)*4+0);
    set_CPU_bank5((regs[0] & 0x0F)*4+1);
    set_CPU_bank6((regs[0] & 0x0F)*4+2);
    set_CPU_bank7((regs[0] & 0x0F)*4+3);
    set_PPU_bank0((((regs[0] & 0x0F)<<2)|((regs[1] & 0x30)>>4))*8+0);
    set_PPU_bank1((((regs[0] & 0x0F)<<2)|((regs[1] & 0x30)>>4))*8+1);
    set_PPU_bank2((((regs[0] & 0x0F)<<2)|((regs[1] & 0x30)>>4))*8+2);
    set_PPU_bank3((((regs[0] & 0x0F)<<2)|((regs[1] & 0x30)>>4))*8+3);
    set_PPU_bank4((((regs[0] & 0x0F)<<2)|((regs[1] & 0x30)>>4))*8+4);
    set_PPU_bank5((((regs[0] & 0x0F)<<2)|((regs[1] & 0x30)>>4))*8+5);
    set_PPU_bank6((((regs[0] & 0x0F)<<2)|((regs[1] & 0x30)>>4))*8+6);
    set_PPU_bank7((((regs[0] & 0x0F)<<2)|((regs[1] & 0x30)>>4))*8+7);
  }
}
/////////////////////////////////////////////////////////////////////

