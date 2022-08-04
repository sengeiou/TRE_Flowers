
/////////////////////////////////////////////////////////////////////
// Mapper 189
class NES_mapper189 : public NES_mapper
{
  friend void adopt_MPRD(SnssMapperBlock* block, NES* nes);
  friend int extract_MPRD(SnssMapperBlock* block, NES* nes);

public:
  NES_mapper189(NES* parent) : NES_mapper(parent) {}
  ~NES_mapper189() {}

  void  Reset();
  void  MemoryWriteLow(uint32 addr, uint8 data);
  void  MemoryWrite(uint32 addr, uint8 data);
  void  HSync(uint32 scanline);

protected:
  uint8 regs[1];
  uint8 irq_counter;
  uint8 irq_latch;
  uint8 irq_enabled;

private:
};
/////////////////////////////////////////////////////////////////////

