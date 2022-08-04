#ifdef _NES_MAPPER_CPP_

/////////////////////////////////////////////////////////////////////
// Mapper 151
void NES_mapper151::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  // set VROM banks
  set_PPU_banks(0,1,2,3,4,5,6,7);
}

void NES_mapper151::MemoryWrite(uint32 addr, uint8 data)
{
  switch(addr & 0xF000)
  {
    case 0x8000:
      {
        set_CPU_bank4(data);
      }
      break;

    case 0xA000:
      {
        set_CPU_bank5(data);
      }
      break;

    case 0xC000:
      {
        set_CPU_bank6(data);
      }
      break;

    case 0xE000:
      {
        set_PPU_bank0(data*4+0);
        set_PPU_bank1(data*4+1);
        set_PPU_bank2(data*4+2);
        set_PPU_bank3(data*4+3);
      }
      break;

    case 0xF000:
      {
        set_PPU_bank4(data*4+0);
        set_PPU_bank5(data*4+1);
        set_PPU_bank6(data*4+2);
        set_PPU_bank7(data*4+3);
      }
      break;
  }
}
/////////////////////////////////////////////////////////////////////

#endif
