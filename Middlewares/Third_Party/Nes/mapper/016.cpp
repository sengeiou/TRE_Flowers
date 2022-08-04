#include "nes_mapper.h"
 

void MAP16_MemoryWrite2(uint32 addr, uint8 data);
 
// Mapper 16
void MAP16_Reset()
{
//  patch = 0;

//  if(parent_NES->crc32() == 0x3f15d20d) // Famicom Jump 2 - Saikyou no 7 Nin
//  {
//    patch = 1;
//  }

//  if(!patch)
//  {
//    NES_6502::Context context;
//    parent_NES->cpu->GetContext(&context);
//    context.mem_page[3] = serial_out;
//    parent_NES->cpu->SetContext(&context);
//    eeprom_status = 0x00;
//    barcode_status = 0x00;
//    barcode_enabled = 0;
//    //memset(serial_out, eeprom_status | barcode_status, 0x2000);
//    serial_out[0] = eeprom_status | barcode_status;
//  }

//  // set CPU bank pointers
//  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  MAPx->regs[0] = 0;
  MAPx->regs[1] = 0;
  MAPx->regs[2] = 0;

  MAPx->irq_enabled = 0;
  MAPx->irq_counter = 0;
  MAPx->irq_latch = 0;
}

void MAP16_MemoryWrite(uint16 addr, uint8 data)
{
//  if(patch)
//  {
//    MemoryWrite3(addr, data);
//  }
//  else
  {
    MAP16_MemoryWrite2(addr, data);
  }
}
 
void MAP16_HSync(int scanline)
{
//  if(barcode_enabled)
//  {
//    if(barcode_phase == 0)
//    {
//      barcode_status = 0x08;
//      if(++barcode_wait > 600)
//      {
//        barcode_wait = 0;
//        barcode_phase = 1;
//      }
//      serial_out[0] = eeprom_status | barcode_status;
//    }
//    else if(barcode_phase == 2)
//    {
//      barcode_status = 0x08;
//      if(++barcode_wait > 600)
//      {
//        barcode_status = 0x00;
//        barcode_enabled = 0;
//      }
//      serial_out[0] = eeprom_status | barcode_status;
//    }
//  }

  if(MAPx->irq_enabled)
  {
    if(MAPx->irq_counter <= 114)
    {
      CPU_IRQ;
      MAPx->irq_counter = 0;
      MAPx->irq_enabled = 0;
    }
    else
    {
      MAPx->irq_counter -= 114;
    }
  }
}

void MAP16_MemoryWrite2(uint32 addr, uint8 data)
{
  switch(addr & 0x000F)
  {
    case 0x0000:
      {
        set_PPU_bank0(data);
      }
      break;

    case 0x0001:
      {
        set_PPU_bank1(data);
      }
      break;

    case 0x0002:
      {
        set_PPU_bank2(data);
      }
      break;

    case 0x0003:
      {
        set_PPU_bank3(data);
      }
      break;

    case 0x0004:
      {
        set_PPU_bank4(data);
      }
      break;

    case 0x0005:
      {
        set_PPU_bank5(data);
      }
      break;

    case 0x0006:
      {
        set_PPU_bank6(data);
      }
      break;

    case 0x0007:
      {
        set_PPU_bank7(data);
      }
      break;

    case 0x0008:
      {
        set_CPU_bank4(data*2+0);
        set_CPU_bank5(data*2+1);
      }
      break;

    case 0x0009:
      {
        data &= 0x03;
        if(data == 0)
        {
             set_mirroring(0,1,0,1);//´¹Ö±¾µÏñInfoNES_Mirroring( 1 );   //0101
             //set_mirroring(NES_PPU::MIRROR_VERT);
        }
        else if(data == 1)
        {
             set_mirroring(0,0,1,1);//Ë®Æ½¾µÏñInfoNES_Mirroring( 0 );   //0011
             //set_mirroring(NES_PPU::MIRROR_HORIZ);
        }
        else if(data == 2)
        {
          set_mirroring(0,0,0,0);
        }
        else
        {
          set_mirroring(1,1,1,1);
        }
      }
      break;

    case 0x000A:
      {
        MAPx->irq_enabled = data & 0x01;
        MAPx->irq_counter = MAPx->irq_latch;
      }
      break;

    case 0x000B:
      {
        MAPx->irq_latch = (MAPx->irq_latch & 0xFF00) | data;
      }
      break;

    case 0x000C:
      {
         MAPx->irq_latch = ((uint32)data << 8) | (MAPx->irq_latch & 0x00FF);
      }
      break;

    case 0x000D:
//      {
//        if(data == 0x80)
//        {
//          // reset
//          eeprom_addr = 0x00;
//          eeprom_mode = 0x00;
//          eeprom_wbit = 0x00;
//          eeprom_rbit = 0x00;
//        }
//        else if(eeprom_cmd[3] == 0x00 && eeprom_cmd[2] == 0x20 &&
//                eeprom_cmd[1] == 0x60 && eeprom_cmd[0] == 0x40 && data == 0x00)
//        {
//          // reset ?
//        }
//        else if(eeprom_cmd[3] == 0x00 && eeprom_cmd[2] == 0x40 &&
//                eeprom_cmd[1] == 0x60 && eeprom_cmd[0] == 0x20 && data == 0x00)
//        {
//          // start
//          eeprom_wbit = 0x01;
//          eeprom_rbit = 0x01;
//          eeprom_data = 0;
//          eeprom_mode = 0;
//        }
//        else if (eeprom_cmd[0] == 0x60 && data == 0xE0)
//        {
//          if(!eeprom_mode)
//          {
//            // sync
//            eeprom_wbit = 0x01;
//            eeprom_rbit = 0x01;
//            eeprom_data = 0;
//            eeprom_mode = 1;
//            eeprom_status = 0x00;
//            //memset(serial_out, eeprom_status | barcode_status, 0x2000);
//            serial_out[0] = eeprom_status | barcode_status;
//          }
//          else
//          {
//            // read
//            eeprom_data = parent_NES->ReadSaveRAM(eeprom_addr);
//            if(eeprom_data & eeprom_rbit)
//            {
//              eeprom_status = 0x10;
//              //memset(serial_out, eeprom_status | barcode_status, 0x2000);
//              serial_out[0] = eeprom_status | barcode_status;
//            }
//            else
//            {
//              eeprom_status = 0x00;
//              //memset(serial_out, eeprom_status | barcode_status, 0x2000);
//              serial_out[0] = eeprom_status | barcode_status;
//            }
//            eeprom_rbit <<= 1;
//            eeprom_wbit = 0x00;
//          }
//        }
//        else if(eeprom_cmd[1] == 0x00 && eeprom_cmd[0] == 0x20 && data == 0x00)
//        {
//          // write 0
//          eeprom_data &= 0xFF - eeprom_wbit;
//          if(eeprom_wbit == 0x80)
//          {
//            if(eeprom_mode)
//            {
//              parent_NES->WriteSaveRAM(eeprom_addr, eeprom_data);
//            }
//            else
//            {
//              eeprom_addr = eeprom_data & 0x7F;
//            }
//            eeprom_wbit = 0x00;
//          }
//          else
//          {
//            eeprom_wbit <<= 1;
//          }
//          eeprom_rbit = 0x00;
//        }
//        else if(eeprom_cmd[3] == 0x00 && eeprom_cmd[2] == 0x40 &&
//                eeprom_cmd[1] == 0x60 && eeprom_cmd[0] == 0x40 && data == 0x00)
//        {
//          // write 1
//          eeprom_data |= eeprom_wbit;
//          if(eeprom_wbit == 0x80)
//          {
//            if(eeprom_mode)
//            {
//              parent_NES->WriteSaveRAM(eeprom_addr, eeprom_data);
//            }
//            else
//            {
//              eeprom_addr = eeprom_data & 0x7F;
//            }
//            eeprom_wbit = 0x00;
//          }
//          else
//          {
//            eeprom_wbit <<= 1;
//          }
//          eeprom_rbit = 0x00;
//        }
//        eeprom_cmd[3] = eeprom_cmd[2];
//        eeprom_cmd[2] = eeprom_cmd[1];
//        eeprom_cmd[1] = eeprom_cmd[0];
//        eeprom_cmd[0] = data;
//      }
      break;
  }
}

void MAP16_MemoryWrite3(uint32 addr, uint8 data)
{
  switch(addr)
  {
    case 0x8000:
    case 0x8001:
    case 0x8002:
    case 0x8003:
      {
        MAPx->regs[0] = data & 0x01;
        set_CPU_bank4(MAPx->regs[0]*0x20+MAPx->regs[2]*2+0);
        set_CPU_bank5(MAPx->regs[0]*0x20+MAPx->regs[2]*2+1);
      }
      break;

    case 0x8004:
    case 0x8005:
    case 0x8006:
    case 0x8007:
      {
        MAPx->regs[1] = data & 0x01;
        set_CPU_bank6(MAPx->regs[1]*0x20+0x1E);
        set_CPU_bank7(MAPx->regs[1]*0x20+0x1F);
      }
      break;

    case 0x8008:
      {
        MAPx->regs[2] = data;
        set_CPU_bank4(MAPx->regs[0]*0x20+MAPx->regs[2]*2+0);
        set_CPU_bank5(MAPx->regs[0]*0x20+MAPx->regs[2]*2+1);
        set_CPU_bank6(MAPx->regs[1]*0x20+0x1E);
        set_CPU_bank7(MAPx->regs[1]*0x20+0x1F);
      }
      break;

    case 0x8009:
      {
        data &= 0x03;
        if(data == 0)
        {
             set_mirroring(0,1,0,1);//´¹Ö±¾µÏñInfoNES_Mirroring( 1 );   //0101
             //set_mirroring(NES_PPU::MIRROR_VERT);
        }
        else if(data == 1)
        {
             set_mirroring(0,0,1,1);//Ë®Æ½¾µÏñInfoNES_Mirroring( 0 );   //0011
             //set_mirroring(NES_PPU::MIRROR_HORIZ);
        }
        else if(data == 2)
        {
          set_mirroring(0,0,0,0);
        }
        else
        {
          set_mirroring(1,1,1,1);
        }
      }
      break;

    case 0x800A:
      {
        MAPx->irq_enabled = data & 0x01;
        MAPx->irq_counter = MAPx->irq_latch;
      }
      break;

    case 0x800B:
      {
        MAPx->irq_latch = (MAPx->irq_latch & 0xFF00) | data;
      }
      break;

    case 0x800C:
      {
         MAPx->irq_latch = ((uint32)data << 8) | (MAPx->irq_latch & 0x00FF);
      }
      break;

    case 0x800D:
      {
        //write protect
      }
      break;
  }
}
 
void MAP16_Init()
{
	NES_Mapper->Reset = MAP16_Reset;
	NES_Mapper->Write = MAP16_MemoryWrite; 
	NES_Mapper->HSync = MAP16_HSync; 
}




