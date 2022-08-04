
/////////////////////////////////////////////////////////////////////
// Mapper 235
class NES_mapper235 : public NES_mapper
{
  friend void adopt_MPRD(SnssMapperBlock* block, NES* nes);
  friend int extract_MPRD(SnssMapperBlock* block, NES* nes);

public:
  NES_mapper235(NES* parent) : NES_mapper(parent) {}
  ~NES_mapper235() {}

  void  Reset();
  void  MemoryWrite(uint32 addr, uint8 data);

protected:
  uint8 dummy[0x2000];

private:
};
/////////////////////////////////////////////////////////////////////

