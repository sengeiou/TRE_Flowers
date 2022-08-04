
/////////////////////////////////////////////////////////////////////
// Mapper 80
class NES_mapper80 : public NES_mapper
{
  friend void adopt_MPRD(SnssMapperBlock* block, NES* nes);
  friend int extract_MPRD(SnssMapperBlock* block, NES* nes);

public:
  NES_mapper80(NES* parent) : NES_mapper(parent) {}
  ~NES_mapper80() {}

  void  Reset();

  void  MemoryWriteSaveRAM(uint32 addr, uint8 data);

protected:
  uint8 patch;

private:
};
/////////////////////////////////////////////////////////////////////

