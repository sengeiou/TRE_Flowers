
/////////////////////////////////////////////////////////////////////
// Mapper 90
class NES_mapper90 : public NES_mapper
{
  friend void adopt_MPRD(SnssMapperBlock* block, NES* nes);
  friend int extract_MPRD(SnssMapperBlock* block, NES* nes);

public:
  NES_mapper90(NES* parent) : NES_mapper(parent) {}
  ~NES_mapper90() {}

  void  Reset();
  uint8 MemoryReadLow(uint32 addr);
  void  MemoryWriteLow(uint32 addr, uint8 data);
  void  MemoryWrite(uint32 addr, uint8 data);
  void  HSync(uint32 scanline);

protected:
  void Sync_Prg_Banks();
  void Sync_Chr_Banks();
  void Sync_Mirror();

  uint8 prg_reg[4];
  uint8 chr_low_reg[8];
  uint8 chr_high_reg[8];
  uint8 nam_low_reg[4];
  uint8 nam_high_reg[4];

  uint8 prg_bank_size;
  uint8 prg_bank_6000;
  uint8 prg_bank_e000;
  uint8 chr_bank_size;
  uint8 mirror_mode;
  uint8 mirror_type;

  uint32 value1;
  uint32 value2;

  uint8 irq_enabled;
  uint8 irq_counter;
  uint8 irq_latch;

private:
};
/////////////////////////////////////////////////////////////////////

