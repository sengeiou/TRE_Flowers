
/////////////////////////////////////////////////////////////////////
// Mapper 232
class NES_mapper232 : public NES_mapper
{
  friend void adopt_MPRD(SnssMapperBlock* block, NES* nes);
  friend int extract_MPRD(SnssMapperBlock* block, NES* nes);

public:
  NES_mapper232(NES* parent) : NES_mapper(parent) {}
  ~NES_mapper232() {}

  void  Reset();
  void  MemoryWrite(uint32 addr, uint8 data);

protected:
  uint8 regs[2];

private:
};
/////////////////////////////////////////////////////////////////////

