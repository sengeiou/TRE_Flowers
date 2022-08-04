
/////////////////////////////////////////////////////////////////////
// Mapper 105
class NES_mapper105 : public NES_mapper
{
  friend void adopt_MPRD(SnssMapperBlock* block, NES* nes);
  friend int extract_MPRD(SnssMapperBlock* block, NES* nes);

public:
  NES_mapper105(NES* parent) : NES_mapper(parent) {}
  ~NES_mapper105() {}

  void  Reset();
  void  MemoryWrite(uint32 addr, uint8 data);
  void  HSync(uint32 scanline);

protected:
  uint8  write_count;
  uint8  bits;
  uint8  regs[4];

  uint8  irq_enabled;
  uint32 irq_counter;
  uint8  init_state;

private:
};
/////////////////////////////////////////////////////////////////////

