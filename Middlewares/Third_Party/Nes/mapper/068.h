
/////////////////////////////////////////////////////////////////////
// Mapper 68
class NES_mapper68 : public NES_mapper
{
  friend void adopt_MPRD(SnssMapperBlock* block, NES* nes);
  friend int extract_MPRD(SnssMapperBlock* block, NES* nes);

public:
  NES_mapper68(NES* parent) : NES_mapper(parent) {}
  ~NES_mapper68() {}

  void  Reset();
  void  MemoryWrite(uint32 addr, uint8 data);

protected:
  void SyncMirror();
  uint8 regs[4];

private:
};
/////////////////////////////////////////////////////////////////////

