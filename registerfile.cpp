#include "registerfile.h"
#include <string>

using std::cout;
using std::endl;

registerfile::registerfile() 
{
	std::vector<int32_t> temp (32);
	regs = temp;
	reset();
}

void registerfile::reset() 
{
	regs.at(0) = 0;
	for (uint i = 1; i < regs.size(); i++) {
		regs.at(i) = 0xf0f0f0f0;
	}
}

void registerfile::set(uint32_t r, int32_t val) 
{
	if (r == 0)
		return;
	else
		regs[r] = val;
}

int32_t registerfile::get(uint32_t r) const 
{
	return regs.at(r);
}

void registerfile::dump(const std::string &hdr) const 
{
	int counter = 0; //current register position

	//IMPLEMENT THE HEADER VAR SOMEHOW

	for (uint i = 0; i < 4; i++) //do this for each line
	{	
		if (hdr[0])
			cout << hdr << " ";
		std::string regstring = "x" + std::to_string(i * 8);
		cout << std:: right << std::setw(3) << regstring << " ";
		for (uint j = 0; j < 8; j++) 
		{
			cout << hex::to_hex32(regs.at(counter)) << " ";
			if (j == 3) 
			{
				cout << " "; //extra whitespace
			}
			counter++;
		}
		cout << endl;
	}
}

