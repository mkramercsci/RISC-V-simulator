#include "hex.h"

std::string hex::to_hex8(uint8_t i) 
{
	std::ostringstream os;
	os << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(i);
	return os.str();
}

std::string hex::to_hex32(uint32_t i) 
{
	std::ostringstream os;
	os << std::hex << std::setfill('0') << std::setw(8) << static_cast<uint32_t>(i);
	return os.str();
}

std::string hex::to_hex0x32(uint32_t i) 
{
	std::ostringstream os;
	os << std::hex << std::setfill('0') << std::setw(8) << static_cast<uint32_t>(i);
	return "0x" + os.str();
}

std::string hex::to_hex0x20(uint32_t i)
{
	std::ostringstream os;
	os << std::hex << std::setfill('0') << std::setw(5) << i;
	return "0x" + os.str();
}

std::string hex::to_hex0x12(uint32_t i)
{
	std::ostringstream os;
	os << std::hex << std::setfill('0') << std::setw(3) << i;

	std::string s = os.str();
	s.resize(3);

	//std::cout << std::endl << os.str() << std::endl;
	return "0x" + s;
}
