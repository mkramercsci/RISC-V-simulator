#include "hex.h"
#include "rv32i_decode.h"
#include <vector>

class registerfile : public hex 
{
public:
	registerfile();
	void reset();
	void set(uint32_t r, int32_t val);
	int32_t get(uint32_t r) const;
	void dump(const std::string &hdr) const;
private:
	std::vector<int32_t> regs;
};

