
/////////////////////////////////////////////////////////////////////
// Mapper 65
class NES_mapper65 : public NES_mapper
{
  friend void adopt_MPRD(SnssMapperBlock* block, NES* nes);
  friend int extract_MPRD(SnssMapperBlock* block, NES* nes);

public:
  NES_mapper65(NES* parent) : NES_mapper(parent) {}
  ~NES_mapper65() {}

  void  Reset();
  void  MemoryWrite(uint32 addr, uint8 data);
  void  HSync(uint32 scanline);

protected:
  uint8 patch;

  uint8 irq_enabled;
  uint32 irq_counter;
  uint32 irq_latch;

private:
};
/////////////////////////////////////////////////////////////////////

