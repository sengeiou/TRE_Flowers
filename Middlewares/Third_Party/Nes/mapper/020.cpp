#ifdef _NES_MAPPER_CPP_

/////////////////////////////////////////////////////////////////////
// Mapper 20
void NES_mapper20::Reset()
{
  // Init ExSound
  parent_NES->apu->SelectExSound(4);

  FILE* fp = NULL;
  char fn[256];

  GetModuleFileName(NULL, fn, 256);
  int pt = strlen(fn);
  while(fn[pt] != '\\') pt--;
  fn[pt+1] = '\0';
  strcat(fn, "DISKSYS.ROM");

  if((fp = fopen(fn, "rb")) != NULL)
  {
    uint8 head1 = fgetc(fp);
    uint8 head2 = fgetc(fp);
    uint8 head3 = fgetc(fp);
    if(head1 == 'N' && head2 == 'E' && head3 == 'S')
    {
      fseek(fp, 0x6010, SEEK_SET);
    }
    else
    {
      fseek(fp, 0, SEEK_SET);
    }
    for(uint32 i = 0; i < 0x2000; i++)
    {
      bios[i] = fgetc(fp);
    }
    fclose(fp);
  }
  else
  {
    throw "DISKSYS.ROM not found.";
  }

  // cancel license screen
  //bios[0xfb0] = 0x00;

  NES_6502::Context context;
  parent_NES->cpu->GetContext(&context);
  context.mem_page[3] = wram + 0x0000;
  context.mem_page[4] = wram + 0x2000;
  context.mem_page[5] = wram + 0x4000;
  context.mem_page[6] = wram + 0x6000;
  context.mem_page[7] = bios;
  parent_NES->cpu->SetContext(&context);

  // read FDS disk image
  if(ROM_banks[0] == 'F' && ROM_banks[1] == 'D' && ROM_banks[2] == 'S')
  {
    for(uint32 i = 0; i < ROM_banks[4]; i++)
    {
      for(uint32 j = 0; j < 65500; j++)
      {
        disk[0x10000*i+j] = ROM_banks[16+65500*i+j];
      }
    }
  }
  else
  {
    throw "Invalid DISK image.";
  }

  if(parent_NES->fds_id() == 0xc7525744) // Reflect World
  {
    parent_NES->frame_irq_disenabled = 1;
  }

  irq_enabled = 0;
  irq_counter = 0;
  irq_latch = 0;
  irq_wait = 0;

  access_flag = 0;
  disk_enabled = 1;
  head_position = 0;
  write_skip = 0;
  disk_status = 0;
  write_reg = 0;
  current_side = 1;
  last_side = 1;
  insert_wait = 180;
}

uint8 NES_mapper20::MemoryReadLow(uint32 addr)
{
  uint8 val = 0x00;

  switch (addr)
  {
    case 0x4030:
      {
        val = disk_status;
      }
      break;

    case 0x4031:
      {
        if((current_side != 0) && (current_side == last_side))
        {
          val = disk[(current_side-1)*0x10000+head_position];
          if(write_reg & 0x01)
          {
            head_position += (head_position < 64999) ? 1 : 0;
            irq_wait = 2;
          }
          access_flag = 1;
        }
        else
        {
          val = 0xff;
        }
      }
      break;

    case 0x4032:
      {
        uint8 eject = ((current_side != 0) && (current_side == last_side)) ? 0 : 1;
        val = 0x40;
        val |= eject ? 1 : 0;
        val |= eject ? 4 : 0;
        val |= (!eject && (write_reg & 0x01) && !(write_reg & 0x02)) ? 0 : 2;

        if(last_side != current_side)
        {
          // wait 2.0 sec for change disk
          if(insert_wait > 120)
          {
            last_side = current_side;
          }
        }
      }
      break;

    case 0x4033:
      {
        val = 0x80;
      }
      break;

    default:
      {
        val = parent_NES->apu->ExRead(addr);
      }
      break;
  }

  return val;
}

void NES_mapper20::MemoryWriteLow(uint32 addr, uint8 data)
{
  switch (addr)
  {
    case 0x4020:
      {
        irq_latch = (irq_latch & 0xFF00) | data;
      }
      break;

    case 0x4021:
      {
        irq_latch = (irq_latch & 0x00FF) | ((uint32)data << 8);
      }
      break;

    case 0x4022:
      {
        irq_counter = irq_latch;
        irq_enabled = data & 0x03;
      }
      break;

    case 0x4023:
      {
        disk_enabled = data & 0x01;
      }
      break;

    case 0x4024:
      {
        if((current_side != 0) && (current_side == last_side))
        {
          if(disk_enabled && !(write_reg & 0x04) && head_position < 65000)
          {
            if(write_skip)
            {
              write_skip--;
            }
            else if(head_position >= 2)
            {
              disk[(current_side-1)*0x10000+head_position-2] = data;
              access_flag = 1;
            }
          }
        }
      }
      break;

    case 0x4025:
      {
        if(data & 0x08)
        {
          set_mirroring(NES_PPU::MIRROR_HORIZ);
        }
        else
        {
          set_mirroring(NES_PPU::MIRROR_VERT);
        }
        if((current_side != 0) && (current_side == last_side))
        {
          if((write_reg & 0x40) && !(data & 0x10) && !(data & 0x40))
          {
            head_position = (head_position < 2) ? 0 : head_position - 2;
          }
          if(!(data & 0x04))
          {
            write_skip = 2;
          }
          if(data & 0x02)
          {
            head_position = 0;
            irq_wait = 2;
          }
          if(data & 0x80)
          {
            irq_wait = 2;
          }
        }
        write_reg = data;
      }
      break;

    default:
      {
        parent_NES->apu->ExWrite(addr, data);
      }
      break;
  }
}

void NES_mapper20::MemoryWriteSaveRAM(uint32 addr, uint8 data)
{
  wram[addr - 0x6000] = data;
}

void NES_mapper20::MemoryWrite(uint32 addr, uint8 data)
{
  if(addr < 0xE000)
  {
    wram[addr - 0x6000] = data;
  }
}

void NES_mapper20::HSync(uint32 scanline)
{
  disk_status &= 0xfc;

  if(irq_enabled)
  {
    if(irq_counter < 113)
    {
      irq_enabled &= 0x01;
      irq_counter = irq_latch;
      disk_status |= 0x01;
      parent_NES->cpu->DoIRQ();
    }
    else
    {
      irq_counter -= 113;
    }
  }
  else if(irq_wait)
  {
    irq_wait--;
    if(!irq_wait && (write_reg & 0x80))
    {
      disk_status |= 0x02;
      parent_NES->cpu->DoIRQ();
    }
  }
}

void NES_mapper20::VSync()
{
  // count MAX 3sec
  insert_wait += (insert_wait < 180) ? 1 : 0;
}

void NES_mapper20::SNSS_fixup()
{
  NES_6502::Context context;
  parent_NES->cpu->GetContext(&context);
  context.mem_page[3] = wram + 0x0000;
  context.mem_page[4] = wram + 0x2000;
  context.mem_page[5] = wram + 0x4000;
  context.mem_page[6] = wram + 0x6000;
  context.mem_page[7] = bios;
  parent_NES->cpu->SetContext(&context);
}
/////////////////////////////////////////////////////////////////////

#endif
