#include <cstdint>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
using std::ifstream;

class memory
{
public:
	memory(uint32_t s);
	~memory();
	/**
	* @param addr An address to check the existence of.
 	* @return true The address is illegal.
	* @return false The address is legal.
 	**/
	bool check_illegal(uint32_t addr) const;

	/**
 	* @return The number of elements in the simulated memory vector.
 	**/ 
	uint32_t get_size() const;

	/**
	* @defgroup getX Get little-endian
	* Read and return a little-endian value from memory.
	*
	* @param addr The address of the first/lowest simulated memory address
	* 	from which to read the least-significant byte of the value.
 	* @return The little-endian value from the simulated memory starting at address addr.
	* @note If one or more of the requested bytes are not in the simulated memory address range then
	* 	a warning message will be printed to std::cout
	* 	@{
 	**/ 

	uint8_t get8(uint32_t addr) const; ///< Get an 8bit value from the simulated memory.
	uint16_t get16(uint32_t addr) const; ///< Get a 16bit little-endian value from the simulated memory.
	uint32_t get32(uint32_t addr) const; ///< Get a 32bit little-endian value from the simulated memory.
	/**@}*/

	/**
	* @defgroup getX_sx Get a little-endian, sign-extended
	* Read and return a sign-extended little-endian value from memory.
	*
	* @param addr The address of the first/lowest simulated memory address
	* 	from which to read the least-significant byte of the value.
 	* @return The sign-extended value of one byte from the simulated memory at address addr.
	* @note If addr is not in the simulated memory address range 
	* 	then a warning message will be printed to std::cout
	*  	@{
 	**/ 
	
	/// Get an 8-bit value from the simulated memory & sign-extend it to 32 bits.
	int32_t get8_sx(uint32_t addr) const;
	/// Get a 16-bit value from the simulated memory & sign-extend it to 32 bits.
	int32_t get16_sx(uint32_t addr) const;
	/// @brief Get a 32-bit value from the simulated memory & sign-extend it to 32 bits.
	/// @note This is only here for consistency.
	int32_t get32_sx(uint32_t addr) const;
	/**@}*/

	/**
	* @defgroup setX Store a little-endian value into the simulated memory.
	* Store a value into the simulated memory as a little endian value with the least
	* 	significant end starting at the given address.
	*
	* @param addr The first/lowest simulated memory address to store va.
	* @param val The value to store into the simulated memory.
	*
	* @note If one or more of the target address is not in the range 
	* 	then a warning message will be printed to std::cout
	*  	@{
 	**/ 
	void set8(uint32_t addr, uint8_t val); ///< Store an 8-bit value into the simulated memory
	void set16(uint32_t addr, uint16_t val);///< Store a 16-bit value into the simulated memory
	void set32(uint32_t addr, uint32_t val);///< Store a 32-bit value into the simulated memory	
	/**@}*/

	/**
	 * @brief Print a hex+ASCII display of the entire simulated memory contents.
	 **/
	void dump() const;

	/**
	 * @brief Load file contents into memory.
	 *
	 * Open and read the binary contents of fname into the simulated memory
	 * 	starting at address zero.
	 * @param fname the name of a file to open and read.
	 * @return true If fname was opened and read into the simulated memory.
	 * @return false If fname could not be opened, or its size is larger than the simulated
	 * 	memory size.
	 **/
	bool load_file(const std::string& fname);
private:
	std::vector < uint8_t > mem; ///< The simulated memory buffer.
};
