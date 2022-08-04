
/////////////////////////////////////////////////////////////////////
// Mapper 4
class NES_mapper4 : public NES_mapper
{
  friend void adopt_MPRD(SnssMapperBlock* block, NES* nes);
  friend int extract_MPRD(SnssMapperBlock* block, NES* nes);

public:
  NES_mapper4(NES* parent) : NES_mapper(parent) {}
  ~NES_mapper4() {}

  void  Reset();

  uint8 MemoryReadLow(uint32 addr);
  void  MemoryWrite(uint32 addr, uint8 data);
  void  HSync(uint32 scanline);

protected:
  uint8  patch;
  uint8  regs[8];

  uint32 prg0,prg1;
  uint32 chr01,chr23,chr4,chr5,chr6,chr7;

  uint32 chr_swap() { return regs[0] & 0x80; }
  uint32 prg_swap() { return regs[0] & 0x40; }

  uint8 irq_enabled; // IRQs enabled
  uint8 irq_counter; // IRQ scanline counter, decreasing
  uint8 irq_latch;   // IRQ scanline counter latch

  uint8 vs_index; // VS Atari RBI Baseball and VS TKO Boxing

  void MMC3_set_CPU_banks();
  void MMC3_set_PPU_banks();

  void SNSS_fixup(); // HACK HACK HACK HACK

private:
};
/////////////////////////////////////////////////////////////////////

