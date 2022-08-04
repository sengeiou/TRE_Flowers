
/////////////////////////////////////////////////////////////////////
// Mapper 255
class NES_mapper255 : public NES_mapper
{
  friend void adopt_MPRD(SnssMapperBlock* block, NES* nes);
  friend int extract_MPRD(SnssMapperBlock* block, NES* nes);

public:
  NES_mapper255(NES* parent) : NES_mapper(parent) {}
  ~NES_mapper255() {}

  void  Reset();
  uint8 MemoryReadLow(uint32);
  void  MemoryWriteLow(uint32 addr, uint8 data);
  void  MemoryWrite(uint32 addr, uint8 data);

protected:
  uint8 regs[4];

private:
};
/////////////////////////////////////////////////////////////////////

