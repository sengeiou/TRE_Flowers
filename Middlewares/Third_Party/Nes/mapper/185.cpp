
/////////////////////////////////////////////////////////////////////
// Mapper 185
void MAP185_Reset()
{
//  // set CPU bank pointers
//  set_CPU_banks(0,1,2,3);

//  patch = 0;

//  if(parent_NES->crc32() == 0xb36457c7) // Spy Vs Spy
//  {
//    patch = 1;
//  }

//  for(uint32 i = 0; i < 0x400; i++)
//  {
//    dummy_chr_rom[i] = 0xFF;
//  }
}

void MAP185_MemoryWrite(uint16 addr, uint8 data)
{
 // if((!patch && (data & 0x03)) || (patch && data == 0x21))
  if((data & 0x03))
  {
    set_PPU_banks(0,1,2,3,4,5,6,7);
  }
//  else
//  {
//    parent_NES->ppu->PPU_VRAM_banks[0] = dummy_chr_rom;
//    parent_NES->ppu->PPU_VRAM_banks[1] = dummy_chr_rom;
//    parent_NES->ppu->PPU_VRAM_banks[2] = dummy_chr_rom;
//    parent_NES->ppu->PPU_VRAM_banks[3] = dummy_chr_rom;
//    parent_NES->ppu->PPU_VRAM_banks[4] = dummy_chr_rom;
//    parent_NES->ppu->PPU_VRAM_banks[5] = dummy_chr_rom;
//    parent_NES->ppu->PPU_VRAM_banks[6] = dummy_chr_rom;
//    parent_NES->ppu->PPU_VRAM_banks[7] = dummy_chr_rom;
//  }
}
void MAP185_Init()
{
	NES_Mapper->Reset = MAP185_Reset;
	NES_Mapper->Write=MAP185_MemoryWrite; 
}





