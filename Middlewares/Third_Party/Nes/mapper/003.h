
/////////////////////////////////////////////////////////////////////
// Mapper 3
class NES_mapper3 : public NES_mapper
{
public:
  NES_mapper3(NES* parent) : NES_mapper(parent) {}
  ~NES_mapper3() {}

  void  Reset();
  void  MemoryWrite(uint32 addr, uint8 data);

protected:
private:
};
/////////////////////////////////////////////////////////////////////

