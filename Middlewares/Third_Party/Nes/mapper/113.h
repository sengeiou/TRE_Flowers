
/////////////////////////////////////////////////////////////////////
// Mapper 113
class NES_mapper113 : public NES_mapper
{
  friend void adopt_MPRD(SnssMapperBlock* block, NES* nes);
  friend int extract_MPRD(SnssMapperBlock* block, NES* nes);

public:
  NES_mapper113(NES* parent) : NES_mapper(parent) {}
  ~NES_mapper113() {}

  void  Reset();
  void  MemoryWriteLow(uint32 addr, uint8 data);
  void  MemoryWrite(uint32 addr, uint8 data);

protected:
private:
};
/////////////////////////////////////////////////////////////////////

