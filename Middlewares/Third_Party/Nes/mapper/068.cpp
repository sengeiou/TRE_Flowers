
/////////////////////////////////////////////////////////////////////
// Mapper 68
void NES_mapper68::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  regs[0] = 0;
  regs[1] = 0;
  regs[2] = 0;
  regs[3] = 0;
}

void NES_mapper68::MemoryWrite(uint32 addr, uint8 data)
{
  switch(addr & 0xF000)
  {
    case 0x8000:
      {
        set_PPU_bank0(data*2+0);
        set_PPU_bank1(data*2+1);
      }
      break;

    case 0x9000:
      {
        set_PPU_bank2(data*2+0);
        set_PPU_bank3(data*2+1);
      }
      break;

    case 0xA000:
      {
        set_PPU_bank4(data*2+0);
        set_PPU_bank5(data*2+1);
      }
      break;

    case 0xB000:
      {
        set_PPU_bank6(data*2+0);
        set_PPU_bank7(data*2+1);
      }
      break;

    case 0xC000:
      {
        regs[2] = data;
        SyncMirror();
      }
      break;

    case 0xD000:
      {
        regs[3] = data;
        SyncMirror();
      }
      break;

    case 0xE000:
      {
        regs[0] = (data & 0x10) >> 4;
        regs[1] = data & 0x03;
        SyncMirror();
      }
      break;

    case 0xF000:
      {
        set_CPU_bank4(data*2);
        set_CPU_bank5(data*2+1);
      }
      break;
  }
}

void NES_mapper68::SyncMirror()
{
  if(regs[0])
  {
    if (regs[1] == 0)
    {
      set_PPU_bank8(regs[2] + 0x80);   // + 0x20000
      set_PPU_bank9(regs[3] + 0x80);
      set_PPU_bank10(regs[2] + 0x80);
      set_PPU_bank11(regs[3] + 0x80);
    }
    else if (regs[1] == 1)
    {
      set_PPU_bank8(regs[2] + 0x80);
      set_PPU_bank9(regs[2] + 0x80);
      set_PPU_bank10(regs[3] + 0x80);
      set_PPU_bank11(regs[3] + 0x80);
    }
    else if (regs[1] == 2)
    {
      set_PPU_bank8(regs[2] + 0x80);
      set_PPU_bank9(regs[2] + 0x80);
      set_PPU_bank10(regs[2] + 0x80);
      set_PPU_bank11(regs[2] + 0x80);
    }
    else if (regs[1] == 3)
    {
      set_PPU_bank8(regs[3] + 0x80);
      set_PPU_bank9(regs[3] + 0x80);
      set_PPU_bank10(regs[3] + 0x80);
      set_PPU_bank11(regs[3] + 0x80);
    }
  }
  else
  {
    if (regs[1] == 0)
    {
      set_mirroring(NES_PPU::MIRROR_VERT);
    }
    else if (regs[1] == 1)
    {
      set_mirroring(NES_PPU::MIRROR_HORIZ);
    }
    else if (regs[1] == 2)
    {
      set_mirroring(0,0,0,0);
    }
    else if (regs[1] == 3)
    {
      set_mirroring(1,1,1,1);
    }
  }
}
/////////////////////////////////////////////////////////////////////

