
/////////////////////////////////////////////////////////////////////
// Mapper 232
void NES_mapper232::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  regs[0] = 0x0C;
  regs[1] = 0x00;
}

void NES_mapper232::MemoryWrite(uint32 addr, uint8 data)
{
  if(addr == 0x9000)
  {
    regs[0] = (data & 0x18) >> 1;
  }
  else if(0xA000 <= addr && addr <= 0xFFFF)
  {
    regs[1] = data & 0x03;
  }

  set_CPU_bank4((regs[0] | regs[1]) * 2 + 0);
  set_CPU_bank5((regs[0] | regs[1]) * 2 + 1);
  set_CPU_bank6((regs[0] | 0x03) * 2 + 0);
  set_CPU_bank7((regs[0] | 0x03) * 2 + 1);
}
/////////////////////////////////////////////////////////////////////

