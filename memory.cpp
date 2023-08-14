#include "memory.h"
#include "hex.h"

using std::cout;
using std::endl;

memory::memory(uint32_t siz)
{
	siz = (siz + 15) & 0xfffffff0; // round the length up, mod-16

	std::vector< uint8_t > temp (siz);
	fill(temp.begin(), temp.end(), 0xa5);
	mem = temp;
}

memory::~memory() 
{
	//a destructor goes here
}

bool memory::check_illegal(uint32_t addr) const
{
	hex obj;
	if (addr > mem.size())
	{
		cout << "WARNING: Address out of range : " << obj.to_hex0x32(addr) << endl;

		return true;
	}	
	return false;
}

uint32_t memory::get_size() const
{
	return mem.size();
}

uint8_t memory::get8(uint32_t addr) const
{
	if (check_illegal(addr))
	{
		return 0;
	}
	else return mem.at(addr);
}

uint16_t memory::get16(uint32_t addr) const
{
	uint8_t firstByte = get8(addr);
	uint8_t secondByte = get8(addr + 1);
	uint16_t littleEndianReturn = secondByte << 8 | firstByte;
	return littleEndianReturn;
}

uint32_t memory::get32(uint32_t addr) const
{
	uint16_t firstChunk = get16(addr);
	uint16_t secondChunk = get16(addr + 2);
	uint32_t littleEndianReturn = secondChunk << 16 | firstChunk;
	return littleEndianReturn;
}

int32_t memory::get8_sx(uint32_t addr) const
{
	uint8_t eightBit = get8(addr);
	int8_t returnVal = (int8_t)eightBit;
	return returnVal;
}

int32_t memory::get16_sx(uint32_t addr) const
{
	int32_t thirtytwoBitSigned = get16(addr);
	return thirtytwoBitSigned;
}

int32_t memory::get32_sx(uint32_t addr) const
{
	return get32(addr);
}

void memory::set8(uint32_t addr, uint8_t val)
{
	if (check_illegal(addr))
	{
		return;
	}
	mem.at(addr) = val;
	return;
}

void memory::set16(uint32_t addr, uint16_t val)
{
	uint8_t byteA = val >> 8;
	uint8_t byteB = val & 0x00FF;
	set8(addr, byteB);
	set8(addr + 1, byteA);
	return;
}

void memory::set32(uint32_t addr, uint32_t val)
{
	uint16_t byteA = val >> 16;
	uint16_t byteB = val & 0xFFFF;
	set16(addr, byteB);
	set16(addr + 2, byteA);
	return;
}

void memory::dump() const
{
	hex obj; //for printing

	int counterA = 0;
	int counterB = 0;

	for (long unsigned int i = 0; i < mem.size() / 16; i++) //do this for number of lines
	{
		cout << obj.to_hex32(i * 16) << ": ";
		for (int j = 0; j < 16; j++) //do this for number of row elements
		{
			cout << obj.to_hex8(get8(counterA)) << " ";
			if (j == 7)
			{
				cout << " ";
			}
			counterA++;
		}
		cout << "*";
		for (int k = 0; k < 16; k++)
		{
			uint8_t ch = get8(counterB);
			ch = isprint(ch) ? ch : '.';
			cout << ch;
			counterB++;
		}
		cout << "*" << endl;
	}
}

bool memory::load_file(const std::string& fname)
{
	ifstream inFile;
	inFile.open(fname);

	if (inFile.fail()) 
	{
		std::cerr << "Can’t open file '" << fname << "' for reading." << endl;
		return false;
	}	
	uint8_t i;
	inFile >> std::noskipws;
	for (uint32_t addr = 0; inFile >> i; ++addr)
	{
		if (check_illegal(addr)) 
		{
			std::cerr<< "Program too big." << endl;
			inFile.close();
			return false;
		}
		set8(addr, i);
	}
	inFile.close();
	return true;
}
