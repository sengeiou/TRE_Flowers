
/////////////////////////////////////////////////////////////////////
// Mapper 90
void NES_mapper90::Reset()
{
  // set CPU bank pointers
  set_CPU_bank4(num_8k_ROM_banks-4);
  set_CPU_bank5(num_8k_ROM_banks-3);
  set_CPU_bank6(num_8k_ROM_banks-2);
  set_CPU_bank7(num_8k_ROM_banks-1);

  // set PPU bank pointers
  set_PPU_banks(0,1,2,3,4,5,6,7);

  irq_counter = 0;
  irq_latch = 0;
  irq_enabled = 0;

  for(uint8 i = 0; i < 4; i++)
  {
    prg_reg[i] = num_8k_ROM_banks-4+i;
    nam_low_reg[i] = 0;
    nam_high_reg[i] = 0;
    chr_low_reg[i] = i;
    chr_high_reg[i] = 0;
    chr_low_reg[i+4] = i+4;
    chr_high_reg[i+4] = 0;
  }
}

uint8 NES_mapper90::MemoryReadLow(uint32 addr)
{
  if(addr == 0x5000)
  {
    return (uint8)(value1*value2 & 0x00FF);
  }
  else
  {
    return (uint8)(addr >> 8);
  }
}

void NES_mapper90::MemoryWriteLow(uint32 addr, uint8 data)
{
  if(addr == 0x5000)
  {
    value1 = data;
  }
  else if(addr == 0x5001)
  {
    value2 = data;
  }
}

void NES_mapper90::MemoryWrite(uint32 addr, uint8 data)
{
  switch(addr)
  {
    case 0x8000:
    case 0x8001:
    case 0x8002:
    case 0x8003:
      {
        prg_reg[addr & 0x03] = data;
        Sync_Prg_Banks();
      }
      break;

    case 0x9000:
    case 0x9001:
    case 0x9002:
    case 0x9003:
    case 0x9004:
    case 0x9005:
    case 0x9006:
    case 0x9007:
      {
        chr_low_reg[addr & 0x07] = data;
        Sync_Chr_Banks();
      }
      break;

    case 0xA000:
    case 0xA001:
    case 0xA002:
    case 0xA003:
    case 0xA004:
    case 0xA005:
    case 0xA006:
    case 0xA007:
      {
        chr_high_reg[addr & 0x07] = data;
        Sync_Chr_Banks();
      }
      break;

    case 0xB000:
    case 0xB001:
    case 0xB002:
    case 0xB003:
      {
        nam_low_reg[addr & 0x03] = data;
        Sync_Mirror();
      }
      break;

    case 0xB004:
    case 0xB005:
    case 0xB006:
    case 0xB007:
      {
        nam_high_reg[addr & 0x03] = data;
        Sync_Mirror();
      }
      break;

    case 0xC002:
      {
        irq_enabled = 0;
      }
      break;

    case 0xC003:
    case 0xC004:
      {
        if(irq_enabled == 0)
        {
          irq_enabled = 1;
          irq_counter = irq_latch;
        }
      }
      break;

    case 0xC005:
      {
        irq_counter = data;
        irq_latch = data;
      }
      break;

    case 0xD000:
      {
        prg_bank_6000 = data & 0x80;
        prg_bank_e000 = data & 0x04;
        prg_bank_size = data & 0x03;
        chr_bank_size = (data & 0x18) >> 3;
        mirror_mode = data & 0x20;
        Sync_Prg_Banks();
        Sync_Chr_Banks();
        Sync_Mirror();
      }
      break;

    case 0xD001:
      {
        mirror_type = data & 0x03;
        Sync_Mirror();
      }
      break;

    case 0xD003:
      {
        // bank page
      }
      break;
  }
}

void NES_mapper90::HSync(uint32 scanline)
{
  if((scanline >= 0) && (scanline <= 239))
  {
    if(parent_NES->ppu->spr_enabled() || parent_NES->ppu->bg_enabled())
    {
      if(irq_counter == 0)
      {
        if(irq_enabled)
        {
          parent_NES->cpu->DoIRQ();
        }
        irq_latch = 0;
        irq_enabled = 0;
      }
      else
      {
        irq_counter--;
      }
    }
  }
}

void NES_mapper90::Sync_Mirror()
{
  uint8 i;
  uint32 nam_bank[4];

  for(i = 0; i < 4; i++)
  {
    nam_bank[i] = ((uint32)nam_high_reg[i] << 8) | (uint32)nam_low_reg[i];
  }

  if(mirror_mode)
  {
    for(i = 0; i < 4; i++)
    {
      if(!nam_high_reg[i] && (nam_low_reg[i] == i))
      {
        mirror_mode = 0;
      }
    }

    if(mirror_mode)
    {
      set_PPU_bank8(nam_bank[0]);
      set_PPU_bank9(nam_bank[1]);
      set_PPU_bank10(nam_bank[2]);
      set_PPU_bank11(nam_bank[3]);
    }
  }
  else
  {
    if(mirror_type == 0)
    {
      set_mirroring(NES_PPU::MIRROR_VERT);
    }
    else if(mirror_type == 1)
    {
      set_mirroring(NES_PPU::MIRROR_HORIZ);
    }
    else
    {
      set_mirroring(0,0,0,0);
    }
  }
}

void NES_mapper90::Sync_Chr_Banks()
{
  uint8 i;
  uint32 chr_bank[8];

  for(i = 0; i < 8; i++)
  {
    chr_bank[i] = ((uint32)chr_high_reg[i] << 8) | (uint32)chr_low_reg[i];
  }

  if(chr_bank_size == 0)
  {
    set_PPU_bank0(chr_bank[0]*8+0);
    set_PPU_bank1(chr_bank[0]*8+1);
    set_PPU_bank2(chr_bank[0]*8+2);
    set_PPU_bank3(chr_bank[0]*8+3);
    set_PPU_bank4(chr_bank[0]*8+4);
    set_PPU_bank5(chr_bank[0]*8+5);
    set_PPU_bank6(chr_bank[0]*8+6);
    set_PPU_bank7(chr_bank[0]*8+7);
  }
  else if(chr_bank_size == 1)
  {
    set_PPU_bank0(chr_bank[0]*4+0);
    set_PPU_bank1(chr_bank[0]*4+1);
    set_PPU_bank2(chr_bank[0]*4+2);
    set_PPU_bank3(chr_bank[0]*4+3);
    set_PPU_bank4(chr_bank[4]*4+0);
    set_PPU_bank5(chr_bank[4]*4+1);
    set_PPU_bank6(chr_bank[4]*4+2);
    set_PPU_bank7(chr_bank[4]*4+3);
  }
  else if(chr_bank_size == 2)
  {
    set_PPU_bank0(chr_bank[0]*2+0);
    set_PPU_bank1(chr_bank[0]*2+1);
    set_PPU_bank2(chr_bank[2]*2+0);
    set_PPU_bank3(chr_bank[2]*2+1);
    set_PPU_bank4(chr_bank[4]*2+0);
    set_PPU_bank5(chr_bank[4]*2+1);
    set_PPU_bank6(chr_bank[6]*2+0);
    set_PPU_bank7(chr_bank[6]*2+1);
  }
  else
  {
    set_PPU_bank0(chr_bank[0]);
    set_PPU_bank1(chr_bank[1]);
    set_PPU_bank2(chr_bank[2]);
    set_PPU_bank3(chr_bank[3]);
    set_PPU_bank4(chr_bank[4]);
    set_PPU_bank5(chr_bank[5]);
    set_PPU_bank6(chr_bank[6]);
    set_PPU_bank7(chr_bank[7]);
  }
}

void NES_mapper90::Sync_Prg_Banks()
{
  if(prg_bank_size == 0)
  {
    set_CPU_bank4(num_8k_ROM_banks-4);
    set_CPU_bank5(num_8k_ROM_banks-3);
    set_CPU_bank6(num_8k_ROM_banks-2);
    set_CPU_bank7(num_8k_ROM_banks-1);
  }
  else if(prg_bank_size == 1)
  {
    set_CPU_bank4(prg_reg[1]*2);
    set_CPU_bank5(prg_reg[1]*2+1);
    set_CPU_bank6(num_8k_ROM_banks-2);
    set_CPU_bank7(num_8k_ROM_banks-1);
  }
  else if(prg_bank_size == 2)
  {
    if(prg_bank_e000)
    {
      set_CPU_bank4(prg_reg[0]);
      set_CPU_bank5(prg_reg[1]);
      set_CPU_bank6(prg_reg[2]);
      set_CPU_bank7(prg_reg[3]);
    }
    else
    {
      if(prg_bank_6000)
      {
        set_CPU_bank3(prg_reg[3]);
      }
      set_CPU_bank4(prg_reg[0]);
      set_CPU_bank5(prg_reg[1]);
      set_CPU_bank6(prg_reg[2]);
      set_CPU_bank7(num_8k_ROM_banks-1);
    }
  }
  else
  {
    // 8k in reverse mode?
    set_CPU_bank4(prg_reg[3]);
    set_CPU_bank5(prg_reg[2]);
    set_CPU_bank6(prg_reg[1]);
    set_CPU_bank7(prg_reg[0]);
  }
}
/////////////////////////////////////////////////////////////////////

