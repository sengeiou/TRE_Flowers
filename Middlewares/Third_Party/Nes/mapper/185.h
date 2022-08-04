
/////////////////////////////////////////////////////////////////////
// Mapper 185
class NES_mapper185 : public NES_mapper
{
  friend void adopt_MPRD(SnssMapperBlock* block, NES* nes);
  friend int extract_MPRD(SnssMapperBlock* block, NES* nes);

public:
  NES_mapper185(NES* parent) : NES_mapper(parent) {}
  ~NES_mapper185() {}

  void  Reset();
  void  MemoryWrite(uint32 addr, uint8 data);

protected:
  uint8 patch;
  uint8 dummy_chr_rom[0x400];

private:
};
/////////////////////////////////////////////////////////////////////

