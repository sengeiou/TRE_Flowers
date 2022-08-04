#ifdef _NES_MAPPER_CPP_

/////////////////////////////////////////////////////////////////////
// Mapper 246
void NES_mapper246::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
}

void NES_mapper246::MemoryWriteSaveRAM(uint32 addr, uint8 data)
{
  switch(addr)
  {
    case 0x6000:
      {
        set_CPU_bank4(data);
      }
      break;

    case 0x6001:
      {
        set_CPU_bank5(data);
      }
      break;

    case 0x6002:
      {
        set_CPU_bank6(data);
      }
      break;

    case 0x6003:
      {
        set_CPU_bank7(data);
      }
      break;

    case 0x6004:
      {
        set_PPU_bank0(data*2+0);
        set_PPU_bank1(data*2+1);
      }
      break;

    case 0x6005:
      {
        set_PPU_bank2(data*2+0);
        set_PPU_bank3(data*2+1);
      }
      break;

    case 0x6006:
      {
        set_PPU_bank4(data*2+0);
        set_PPU_bank5(data*2+1);
      }
      break;

    case 0x6007:
      {
        set_PPU_bank6(data*2+0);
        set_PPU_bank7(data*2+1);
      }
      break;
  }
}
/////////////////////////////////////////////////////////////////////

#endif
