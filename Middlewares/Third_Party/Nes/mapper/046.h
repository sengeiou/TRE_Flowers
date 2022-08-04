
/////////////////////////////////////////////////////////////////////
// Mapper 46
class NES_mapper46 : public NES_mapper
{
  friend void adopt_MPRD(SnssMapperBlock* block, NES* nes);
  friend int extract_MPRD(SnssMapperBlock* block, NES* nes);

public:
  NES_mapper46(NES* parent) : NES_mapper(parent) {}
  ~NES_mapper46() {}

  void  Reset();
  void  MemoryWriteSaveRAM(uint32 addr, uint8 data);
  void  MemoryWrite(uint32 addr, uint8 data);

protected:
  void set_rom_banks();
  uint8 regs[4];

private:
};
/////////////////////////////////////////////////////////////////////

