
/////////////////////////////////////////////////////////////////////
// Mapper 7
class NES_mapper7 : public NES_mapper
{
public:
  NES_mapper7(NES* parent) : NES_mapper(parent) {}
  ~NES_mapper7() {}

  void  Reset();

  void  MemoryWrite(uint32 addr, uint8 data);
protected:
private:
};
/////////////////////////////////////////////////////////////////////

