
/////////////////////////////////////////////////////////////////////
// Mapper 87
class NES_mapper87 : public NES_mapper
{
  friend void adopt_MPRD(SnssMapperBlock* block, NES* nes);
  friend int extract_MPRD(SnssMapperBlock* block, NES* nes);

public:
  NES_mapper87(NES* parent) : NES_mapper(parent) {}
  ~NES_mapper87() {}

  void  Reset();
  void  MemoryWriteSaveRAM(uint32 addr, uint8 data);

protected:
private:
};
/////////////////////////////////////////////////////////////////////

