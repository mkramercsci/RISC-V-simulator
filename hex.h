#ifndef HEX_H
#define HEX_H
#include <iostream>
#include <sstream>
#include <iomanip>
/**
 * A helper class which allowes printing of numbers in different lengths. 
 **/
class hex
{
public:
	/**
	 *@param i A number to be printed
	 *@return A two char wide hexadecimal number
	**/
	static std::string to_hex8(uint8_t i);

	/**
	 *@param i A number to be printed
	 *@return An eight char wide hexadecimal number
	**/
	static std::string to_hex32(uint32_t i);

	/**
	 *@param i A number to be printed
	 *@return An eight char wide hexadecimal number with prefix '0x'
	**/
	static std::string to_hex0x32(uint32_t i);

	static std::string to_hex0x20(uint32_t i);
	static std::string to_hex0x12(uint32_t i);
};

#endif // HEX_H
