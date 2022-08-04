
/////////////////////////////////////////////////////////////////////
// Mapper 92
void NES_mapper92::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  // set PPU bank pointers
  if(num_1k_VROM_banks)
  {
    set_PPU_banks(0,1,2,3,4,5,6,7);
  }
}

// Created by rinao
void NES_mapper92::MemoryWrite(uint32 addr, uint8 data)
{
  data = (uint8)addr & 0xff;
  uint8 c_bank = (data & 0x0f) << 1;
  uint8 p_bank = data & 0x0f;

  if (addr >= 0x9000){ // Moero!! ProSoccer
    if ((data & 0xf0) == 0xd0){
      set_CPU_banks(0,1,c_bank,c_bank+1);
    } else if ((data & 0xf0) == 0xe0){
      set_PPU_banks(p_bank*8,p_bank*8+1,p_bank*8+2,p_bank*8+3,p_bank*8+4,p_bank*8+5,p_bank*8+6,p_bank*8+7);
    }
  } else { // Moero!! Proyakyuu '88 ketteiban
    if ((data & 0xf0) == 0xb0){
      set_CPU_banks(0,1,c_bank,c_bank+1);
    } else if ((data & 0xf0) == 0x70){
      set_PPU_banks(p_bank*8,p_bank*8+1,p_bank*8+2,p_bank*8+3,p_bank*8+4,p_bank*8+5,p_bank*8+6,p_bank*8+7);
    }
  }
}
/////////////////////////////////////////////////////////////////////

