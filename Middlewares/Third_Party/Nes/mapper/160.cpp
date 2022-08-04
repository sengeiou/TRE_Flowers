
/////////////////////////////////////////////////////////////////////
// Mapper 160
void NES_mapper160::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  irq_enabled = 0;
  irq_counter = 0;
  irq_latch = 0;
  refresh_type = 0;
}

void NES_mapper160::MemoryWrite(uint32 addr, uint8 data)
{
  switch(addr)
  {
    case 0x8000:
      {
        set_CPU_bank4(data);
      }
      break;

    case 0x8001:
      {
        set_CPU_bank5(data);
      }
      break;

    case 0x8002:
      {
        set_CPU_bank6(data);
      }
      break;

    case 0x9000:
      {
        if(data == 0x2B)
        {
          refresh_type = 1; // title
        }
        else if(data == 0xA8)
        {
          refresh_type = 2; // pass word
        }
        else if(data == 0x1F)
        {
          refresh_type = 3; // game over
        }
        else if(data == 0x7C)
        {
          refresh_type = 4; // continue
        }
        else if(data == 0x18)
        {
          refresh_type = 5; // roulette
        }
        else if(data == 0x60)
        {
          refresh_type = 6; // congratulation
        }
        else
        {
          refresh_type = 0;
        }
        set_PPU_bank0(data);
      }
      break;

    case 0x9001:
      {
        set_PPU_bank1(data);
      }
      break;

    case 0x9002:
      {
        if(refresh_type == 2 && data != 0xE8)
        {
          refresh_type = 0; // not pass word
        }
        set_PPU_bank2(data);
      }
      break;

    case 0x9003:
      {
        set_PPU_bank3(data);
      }
      break;

    case 0x9004:
      {
        set_PPU_bank4(data);
      }
      break;

    case 0x9005:
      {
        set_PPU_bank5(data);
      }
      break;

    case 0x9006:
      {
        set_PPU_bank6(data);
      }
      break;

    case 0x9007:
      {
        set_PPU_bank7(data);
      }
      break;

    case 0xC000:
      {
        irq_counter = irq_latch;
        irq_enabled = irq_latch;
      }
      break;

    case 0xC001:
      {
        irq_latch = data;
      }
      break;

    case 0xC002:
      {
        irq_enabled = 0;
      }
      break;

    case 0xC003:
      {
        irq_counter = data;
      }
      break;
  }
}

void NES_mapper160::HSync(uint32 scanline)
{
  if(scanline == 0 || scanline == 239)
  {
    if(refresh_type == 1)
    {
      set_PPU_banks(0x58,0x59,0x5A,0x5B,0x58,0x59,0x5A,0x5B);
    }
    else if(refresh_type == 2)
    {
      set_PPU_banks(0x78,0x79,0x7A,0x7B,0x78,0x79,0x7A,0x7B);
    }
    else if(refresh_type == 3)
    {
      set_PPU_banks(0x7C,0x7D,0x7E,0x7F,0x7C,0x7D,0x7E,0x7F);
    }
    else if(refresh_type == 5)
    {
      set_PPU_banks(0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77);
    }
    else if(refresh_type == 6)
    {
      set_PPU_banks(0x5C,0x5D,0x5E,0x5F,0x7C,0x7D,0x7E,0x7F);
    }
  }
  if(scanline == 64)
  {
    if(refresh_type == 4)
    {
      // face of lamp's spirit
      if(parent_NES->ppu->PPU_VRAM_banks[8][32*10+16] == 0x0A)
      {
        set_PPU_bank0(0x68);
        set_PPU_bank1(0x69);
        set_PPU_bank2(0x6A);
        set_PPU_bank3(0x6B);
      }else{
        set_PPU_bank0(0x6C);
        set_PPU_bank1(0x6D);
        set_PPU_bank2(0x6E);
        set_PPU_bank3(0x6F);
      }
    }
  }
  if(scanline == 128)
  {
    if(refresh_type == 4)
    {
      set_PPU_bank0(0x68);
      set_PPU_bank1(0x69);
      set_PPU_bank2(0x6A);
      set_PPU_bank3(0x6B);
    }
    else if(refresh_type == 5)
    {
      set_PPU_banks(0x74,0x75,0x76,0x77,0x74,0x75,0x76,0x77);
    }
  }
  if(scanline == 160)
  {
    if(refresh_type == 6)
    {
      set_PPU_banks(0x60,0x61,0x5E,0x5F,0x7C,0x7D,0x7E,0x7F);
    }
  }
  if(irq_enabled)
  {
    if(irq_counter == 0xFF)
    {
      parent_NES->cpu->DoIRQ();
      irq_counter = 0;
      irq_enabled = 0;
    }
    else
    {
      irq_counter++;
    }
  }
}
/////////////////////////////////////////////////////////////////////

