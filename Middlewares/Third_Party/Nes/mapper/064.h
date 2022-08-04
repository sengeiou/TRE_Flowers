
/////////////////////////////////////////////////////////////////////
// Mapper 64
class NES_mapper64 : public NES_mapper
{
  friend void adopt_MPRD(SnssMapperBlock* block, NES* nes);
  friend int extract_MPRD(SnssMapperBlock* block, NES* nes);

public:
  NES_mapper64(NES* parent) : NES_mapper(parent) {}
  ~NES_mapper64() {}

  void  Reset();
  void  MemoryWrite(uint32 addr, uint8 data);
  void  HSync(uint32 scanline);

protected:
  uint8 regs[3];
  uint8 irq_latch;
  uint8 irq_counter;
  uint8 irq_enabled;

private:
};
/////////////////////////////////////////////////////////////////////

