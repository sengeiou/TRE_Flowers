
/////////////////////////////////////////////////////////////////////
// Mapper 188
class NES_mapper188 : public NES_mapper
{
public:
  NES_mapper188(NES* parent) : NES_mapper(parent) {}
  ~NES_mapper188() {}

  void  Reset();
  void  MemoryWrite(uint32 addr, uint8 data);

protected:
  uint8 dummy[0x2000];

private:
};
/////////////////////////////////////////////////////////////////////

