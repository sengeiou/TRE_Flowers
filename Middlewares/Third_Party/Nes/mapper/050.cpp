
/////////////////////////////////////////////////////////////////////
// Mapper 50
void NES_mapper50::Reset()
{
  set_CPU_banks(15,8,9,0,11);
  set_PPU_banks(0,1,2,3,4,5,6,7);

  irq_enabled = 0;
}

void NES_mapper50::MemoryWriteLow(uint32 addr, uint8 data)
{
  if((addr & 0xE060) == 0x4020)
  {
    if(addr & 0x0100)
    {
      irq_enabled = data & 0x01;
    }
    else
    {
      set_CPU_bank6((data&0x08)|((data&0x01)<<2)|((data&0x06)>>1));
    }
  }
}

void NES_mapper50::HSync(uint32 scanline)
{
  if(irq_enabled)
  {
    if(scanline == 21)
    {
      parent_NES->cpu->DoIRQ();
    }
  }
}
/////////////////////////////////////////////////////////////////////

