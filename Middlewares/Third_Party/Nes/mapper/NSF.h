
/////////////////////////////////////////////////////////////////////
// Mapper NSF - private mapper number = 12 (decimal)
class NES_mapperNSF : public NES_mapper
{
  friend void adopt_MPRD(SnssMapperBlock* block, NES* nes);
  friend int extract_MPRD(SnssMapperBlock* block, NES* nes);

public:
  NES_mapperNSF(NES* parent) : NES_mapper(parent) {}
  ~NES_mapperNSF() {}

  void  Reset();
  uint8 MemoryReadLow(uint32 addr);
  void  MemoryWriteLow(uint32 addr, uint8 data);
  void  MemoryWriteSaveRAM(uint32 addr, uint8 data);
  void  MemoryWrite(uint32 addr, uint8 data);

protected:
  void  BankSwitch(uint8 num, uint8 bank);
  void  LoadPlayer();

  uint8 wram1[0x2000];
  uint8 wram2[0x8000];
  uint8 chip_type;

private:
};
/////////////////////////////////////////////////////////////////////

