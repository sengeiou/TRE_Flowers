
/////////////////////////////////////////////////////////////////////
// Mapper 40 (smb2j)
void NES_mapper40::Reset()
{
  irq_enabled = 0;
  lines_to_irq = 0;

  // set CPU bank pointers
  set_CPU_banks(6,4,5,0,7);

  // set VROM banks
  if(num_1k_VROM_banks)
  {
    set_PPU_banks(0,1,2,3,4,5,6,7);
  }
}

void NES_mapper40::MemoryWrite(uint32 addr, uint8 data)
{
  switch(addr & 0xE000)
  {
    case 0x8000:
      irq_enabled = 0;
//      LOG("MAP40: [$8000] = " << HEX(data,2) << endl);
      break;

    case 0xA000:
      irq_enabled = 1;
      lines_to_irq = 37;

//      LOG("MAP40: [$A000] = " << HEX(data,2) << endl);
      break;

    case 0xC000:
//      LOG("MAP40: [$C000] = " << HEX(data,2) << endl);
//      LOG("MAP40: INVALID WRITE TO $C000" << endl);
      break;

    case 0xE000:
//      LOG("MAP40: [$E000] = " << HEX(data,2) << endl);

      set_CPU_bank6(data & 0x07);
      break;

  }
}

void NES_mapper40::HSync(uint32 scanline)
{
  if(irq_enabled)
  {
    if((--lines_to_irq) <= 0)
    {
      parent_NES->cpu->DoIRQ();
    }
  }
}
/////////////////////////////////////////////////////////////////////

