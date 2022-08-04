
/////////////////////////////////////////////////////////////////////
// Mapper 237
void NES_mapper237::Reset()
{
  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
  set_PPU_banks(0,1,2,3,4,5,6,7);
}

void NES_mapper237::MemoryWrite(uint32 addr, uint8 data)
{
  if(addr & 0x4000)
  {
    if((addr & 0x0030) == 0x00)
    {
      set_CPU_bank4((addr&0x07)*2+0);
      set_CPU_bank5((addr&0x07)*2+1);
      set_CPU_bank6(num_8k_ROM_banks-2);
      set_CPU_bank7(num_8k_ROM_banks-1);
    }
    else if((addr & 0x0030) == 0x10)
    {
      for(uint32 i = 0; i < 0x2000; i++)
      {
        wram[i+0x0000] = ROM_banks[((addr&0x07)*2+0)*0x2000+(i&0x1ff0)+0x0d]; // 0x0b?
        wram[i+0x2000] = ROM_banks[((addr&0x07)*2+1)*0x2000+(i&0x1ff0)+0x0d]; // 0x0b?
        wram[i+0x4000] = ROM_banks[(num_8k_ROM_banks-2)*0x2000+(i&0x1ff0)+0x0d]; // 0x0b?
        wram[i+0x6000] = ROM_banks[(num_8k_ROM_banks-1)*0x2000+(i&0x1ff0)+0x0d]; // 0x0b?
      }
      NES_6502::Context context;
      parent_NES->cpu->GetContext(&context);
      context.mem_page[4] = wram + 0x0000;
      context.mem_page[5] = wram + 0x2000;
      context.mem_page[6] = wram + 0x4000;
      context.mem_page[7] = wram + 0x6000;
      parent_NES->cpu->SetContext(&context);
    }
    else if((addr & 0x0030) == 0x20)
    {
      set_CPU_bank4((addr&0x06)*2+0);
      set_CPU_bank5((addr&0x06)*2+1);
      set_CPU_bank6((addr&0x06)*2+2);
      set_CPU_bank7((addr&0x06)*2+3);
    }
    else if((addr & 0x0030) == 0x30)
    {
      set_CPU_bank4((addr&0x07)*2+0);
      set_CPU_bank5((addr&0x07)*2+1);
      set_CPU_bank6((addr&0x07)*2+0);
      set_CPU_bank7((addr&0x07)*2+1);
    }
  }
  else
  {
    if(addr & 0x0020)
    {
      set_mirroring(NES_PPU::MIRROR_VERT);
    }
    else
    {
      set_mirroring(NES_PPU::MIRROR_HORIZ);
    }
    set_PPU_bank0((addr&0x07)*8+0);
    set_PPU_bank1((addr&0x07)*8+1);
    set_PPU_bank2((addr&0x07)*8+2);
    set_PPU_bank3((addr&0x07)*8+3);
    set_PPU_bank4((addr&0x07)*8+4);
    set_PPU_bank5((addr&0x07)*8+5);
    set_PPU_bank6((addr&0x07)*8+6);
    set_PPU_bank7((addr&0x07)*8+7);
  }
}
/////////////////////////////////////////////////////////////////////

