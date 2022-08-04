
/////////////////////////////////////////////////////////////////////
// Mapper 99
class NES_mapper99 : public NES_mapper
{
  friend void adopt_MPRD(SnssMapperBlock* block, NES* nes);
  friend int extract_MPRD(SnssMapperBlock* block, NES* nes);

public:
  NES_mapper99(NES* parent) : NES_mapper(parent) {}
  ~NES_mapper99() {}

  void  Reset();
  void  WriteHighRegs(uint32 addr, uint8 data);

protected:
private:
};
/////////////////////////////////////////////////////////////////////

