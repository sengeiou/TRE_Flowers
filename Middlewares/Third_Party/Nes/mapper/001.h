
/////////////////////////////////////////////////////////////////////
// Mapper 1
class NES_mapper1 : public NES_mapper
{
  friend void adopt_MPRD(SnssMapperBlock* block, NES* nes);
  friend int extract_MPRD(SnssMapperBlock* block, NES* nes);

public:
  NES_mapper1(NES* parent) : NES_mapper(parent) {}
  ~NES_mapper1() {}

  void  Reset();
  void  MemoryWriteSaveRAM(uint32 addr, uint8 data);
  void  MemoryWrite(uint32 addr, uint8 data);

protected:
  // this uses MMC1_256K_base and MMC1_bankX
  void MMC1_set_CPU_banks();

  uint32 write_count;
  uint8  bits;
  uint8  regs[4];
  uint32 last_write_addr;

  // Best Play - Pro Yakyuu Special
  uint8 patch;
  uint8 wram_bank, wram_flag, wram_count;
  uint8 wram[0x4000];

  enum MMC1_Size_t
  {
    MMC1_SMALL,
    MMC1_512K,
    MMC1_1024K
  };

  MMC1_Size_t MMC1_Size;
  uint32 MMC1_256K_base;
  uint32 MMC1_swap;

  // these are the 4 ROM banks currently selected
  uint32 MMC1_bank1;
  uint32 MMC1_bank2;
  uint32 MMC1_bank3;
  uint32 MMC1_bank4;

  uint32 MMC1_HI1;
  uint32 MMC1_HI2;
private:
};
/////////////////////////////////////////////////////////////////////

