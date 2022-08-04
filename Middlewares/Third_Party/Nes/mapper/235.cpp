
/////////////////////////////////////////////////////////////////////
// Mapper 235
void NES_mapper235::Reset()
{
  // set dummy data for open bus
  for(uint32 i = 0; i < 0x2000; i++)
  {
    dummy[i] = 0xFF;
  }

  // set CPU bank pointers
  set_CPU_banks(0,1,2,3);
}

void NES_mapper235::MemoryWrite(uint32 addr, uint8 data)
{
  uint8 prg_bank = ((addr & 0x0300) >> 3) | (addr & 0x001F);
  uint8 open_bus = 0;

  if (num_8k_ROM_banks == 64*2)
  {
    // 100-in-1
    switch (addr & 0x0300)
    {
      case 0x0000: break;
      case 0x0100: open_bus = 1; break;
      case 0x0200: open_bus = 1; break;
      case 0x0300: open_bus = 1; break;
    }
  }
  else if (num_8k_ROM_banks == 128*2)
  {
    // 150-in-1
    switch (addr & 0x0300)
    {
      case 0x0000: break;
      case 0x0100: open_bus = 1; break;
      case 0x0200: prg_bank = (prg_bank & 0x1F) | 0x20; break;
      case 0x0300: open_bus = 1; break;
    }
  }
  else if (num_8k_ROM_banks == 192*2)
  {
    // 210-in-1
    switch (addr & 0x0300)
    {
      case 0x0000: break;
      case 0x0100: open_bus = 1; break;
      case 0x0200: prg_bank = (prg_bank & 0x1F) | 0x20; break;
      case 0x0300: prg_bank = (prg_bank & 0x1F) | 0x40; break;
    }
  }
  else if (num_8k_ROM_banks == 256*2)
  {
    // 260-in-1
  }
  if(addr & 0x0800)
  {
    if(addr & 0x1000)
    {
      // upper 16KB Banks
      set_CPU_bank4(prg_bank*4+2);
      set_CPU_bank5(prg_bank*4+3);
      set_CPU_bank6(prg_bank*4+2);
      set_CPU_bank7(prg_bank*4+3);
    }
    else
    {
      // lower 16KB Banks
      set_CPU_bank4(prg_bank*4+0);
      set_CPU_bank5(prg_bank*4+1);
      set_CPU_bank6(prg_bank*4+0);
      set_CPU_bank7(prg_bank*4+1);
    }
  }
  else
  {
    // 32KB Banks
    set_CPU_bank4(prg_bank*4+0);
    set_CPU_bank5(prg_bank*4+1);
    set_CPU_bank6(prg_bank*4+2);
    set_CPU_bank7(prg_bank*4+3);
  }
  if(open_bus)
  {
    NES_6502::Context context;
    parent_NES->cpu->GetContext(&context);
    context.mem_page[4] = dummy;
    context.mem_page[5] = dummy;
    context.mem_page[6] = dummy;
    context.mem_page[7] = dummy;
    parent_NES->cpu->SetContext(&context);
  }
  if(addr & 0x0400)
  {
    set_mirroring(0,0,0,0);
  }
  else if(addr & 0x2000)
  {
    set_mirroring(NES_PPU::MIRROR_HORIZ);
  }
  else
  {
    set_mirroring(NES_PPU::MIRROR_VERT);
  }
}
/////////////////////////////////////////////////////////////////////

