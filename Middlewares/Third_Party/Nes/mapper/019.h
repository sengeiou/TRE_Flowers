
/////////////////////////////////////////////////////////////////////
// Mapper 19
class NES_mapper19 : public NES_mapper
{
  friend void adopt_MPRD(SnssMapperBlock* block, NES* nes);
  friend int extract_MPRD(SnssMapperBlock* block, NES* nes);

public:
  NES_mapper19(NES* parent) : NES_mapper(parent) {}
  ~NES_mapper19() {}

  void  Reset();

  uint8 MemoryReadLow(uint32 addr);
  void  MemoryWriteLow(uint32 addr, uint8 data);
  void  MemoryWrite(uint32 addr, uint8 data);
  void  HSync(uint32 scanline);

protected:
  uint8 patch;

  uint8 regs[3];
  uint8 irq_enabled;
  uint32 irq_counter;

private:
};
/////////////////////////////////////////////////////////////////////

