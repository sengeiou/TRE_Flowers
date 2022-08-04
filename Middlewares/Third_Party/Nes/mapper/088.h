
/////////////////////////////////////////////////////////////////////
// Mapper 88
class NES_mapper88 : public NES_mapper
{
  friend void adopt_MPRD(SnssMapperBlock* block, NES* nes);
  friend int extract_MPRD(SnssMapperBlock* block, NES* nes);

public:
  NES_mapper88(NES* parent) : NES_mapper(parent) {}
  ~NES_mapper88() {}

  void  Reset();
  void  MemoryWrite(uint32 addr, uint8 data);

protected:
  uint8  regs[1];

private:
};
/////////////////////////////////////////////////////////////////////

