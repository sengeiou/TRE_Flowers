
/////////////////////////////////////////////////////////////////////
// Mapper 18
class NES_mapper18 : public NES_mapper
{
  friend void adopt_MPRD(SnssMapperBlock* block, NES* nes);
  friend int extract_MPRD(SnssMapperBlock* block, NES* nes);

public:
  NES_mapper18(NES* parent) : NES_mapper(parent) {}
  ~NES_mapper18() {}

  void  Reset();
  void  MemoryWrite(uint32 addr, uint8 data);
  void  HSync(uint32 scanline);

protected:
  uint8 patch;
  uint8 regs[11];
  uint8 irq_enabled;
  uint32 irq_latch;
  uint32 irq_counter;

  void SNSS_fixup();
private:
};
/////////////////////////////////////////////////////////////////////

