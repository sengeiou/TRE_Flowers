
/////////////////////////////////////////////////////////////////////
// Mapper 40 (smb2j)
class NES_mapper40 : public NES_mapper
{
  friend void adopt_MPRD(SnssMapperBlock* block, NES* nes);
  friend int extract_MPRD(SnssMapperBlock* block, NES* nes);

public:
  NES_mapper40(NES* parent) : NES_mapper(parent) {}
  ~NES_mapper40() {}

  void  Reset();

  void  MemoryWrite(uint32 addr, uint8 data);

  void  HSync(uint32 scanline);
protected:
  uint8 irq_enabled;
  uint32 lines_to_irq;
private:
};
/////////////////////////////////////////////////////////////////////

