
/////////////////////////////////////////////////////////////////////
// Mapper 117
class NES_mapper117 : public NES_mapper
{
  friend void adopt_MPRD(SnssMapperBlock* block, NES* nes);
  friend int extract_MPRD(SnssMapperBlock* block, NES* nes);

public:
  NES_mapper117(NES* parent) : NES_mapper(parent) {}
  ~NES_mapper117() {}

  void  Reset();
  void  MemoryWrite(uint32 addr, uint8 data);
  void  HSync(uint32 scanline);

protected:
  uint8 irq_line;
  uint8 irq_enabled1;
  uint8 irq_enabled2;

private:
};
/////////////////////////////////////////////////////////////////////

