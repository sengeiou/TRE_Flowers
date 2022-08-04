
/////////////////////////////////////////////////////////////////////
// Mapper 6
class NES_mapper6 : public NES_mapper
{
  friend void adopt_MPRD(SnssMapperBlock* block, NES* nes);
  friend int extract_MPRD(SnssMapperBlock* block, NES* nes);

  friend void adopt_ExMPRD(const char* fn, NES* nes);
  friend void extract_ExMPRD(const char* fn, NES* nes);

public:
  NES_mapper6(NES* parent) : NES_mapper(parent) {}
  ~NES_mapper6() {}

  void  Reset();
  void  MemoryWriteLow(uint32 addr, uint8 data);
  void  MemoryWrite(uint32 addr, uint8 data);
  void  HSync(uint32 scanline);

protected:
  uint8 irq_enabled;
  uint32 irq_counter;
  uint8 chr_ram[4*0x2000];

private:
};
/////////////////////////////////////////////////////////////////////

