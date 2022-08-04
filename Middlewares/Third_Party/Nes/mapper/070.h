
/////////////////////////////////////////////////////////////////////
// Mapper 70
class NES_mapper70 : public NES_mapper
{
  friend void adopt_MPRD(SnssMapperBlock* block, NES* nes);
  friend int extract_MPRD(SnssMapperBlock* block, NES* nes);

public:
  NES_mapper70(NES* parent) : NES_mapper(parent) {}
  ~NES_mapper70() {}

  void  Reset();

  void  MemoryWrite(uint32 addr, uint8 data);

protected:
  uint8 patch;

private:
};
/////////////////////////////////////////////////////////////////////

