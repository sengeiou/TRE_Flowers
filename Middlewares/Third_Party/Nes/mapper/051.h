
/////////////////////////////////////////////////////////////////////
// Mapper 51
class NES_mapper51 : public NES_mapper
{
  friend void adopt_MPRD(SnssMapperBlock* block, NES* nes);
  friend int extract_MPRD(SnssMapperBlock* block, NES* nes);

public:
  NES_mapper51(NES* parent) : NES_mapper(parent) {}
  ~NES_mapper51() {}

  void  Reset();
  void  MemoryWriteSaveRAM(uint32 addr, uint8 data);
  void  MemoryWrite(uint32 addr, uint8 data);

protected:
  void  Sync_Prg_Banks();
  uint8 bank, mode;

private:
};
/////////////////////////////////////////////////////////////////////

