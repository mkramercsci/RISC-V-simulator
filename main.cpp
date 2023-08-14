//*********************************
//
// CSCI 463 Assignment 5
//
// Author: Michaela Kramer
//
// RISC-V Simulator
//
//*********************************
#include <iostream>
#include <unistd.h>	
#include <stdlib.h>
#include <stdio.h>
#include "hex.h"
//#include "memory.h"
//#include "registerfile.h"
#include"cpu_single_hart.h"

using std::cout;
using std::endl;
using std::cerr;

/**
 * @brief Standard errors printed when the command line is used improperly. 
 * */

static void usage()
{
	cout << "Usage : rv32i [-d ] [ -i] [-r] [- z] [-l exec - limit ] [-m hex - mem - size ] infile\n";
	cout << "-d show disassembly before program execution\n";
	cout << "-i show instruction printing during execution\n";
	cout << "-l maximum number of instructions to exec\n";
	cout << "-m specify memory size ( default = 0 x100 )\n";
	cout << "-r show register printing during executio\n";	
	cout << "-z show a dump of the regs & memory after simulation\n";
	exit(1);
}

/**
 * @brief a loop which prints addresses and fullword values from simulated memory
 * @param mem The simulated memory
 **/
static void disassemble(const memory& mem)
{

	for (uint32_t i = 0; i < mem.get_size(); i+= 4)
	{	
		cout << hex::to_hex32(i) << ": " << hex::to_hex32(mem.get32(i)) << "  "; 
		cout << rv32i_decode::decode(i, mem.get32(i)) << endl;
	}
	return;
}

/**
 * @brief The entry point which utilises hex and memory classes to simulate memory.
 * @param argc The number of command line arguments passed in.
 * @param argv A list of all the command line arguments. 
 **/
int main(int argc, char **argv)
{
	uint32_t memory_limit = 0x100;	// default memory size is 256 bytes

	bool dFlag = false;
	bool iFlag = false;
	bool rFlag = false;
	bool zFlag = false;
	uint32_t exec_limit = 0;

	int opt;
	while ((opt = getopt(argc, argv, "l:dirm:z")) != -1)
	{
		switch(opt)
		{
			case 'm':
				{ //set the given mem size
					std::istringstream iss(optarg);
					iss >> std::hex >> memory_limit;
					break;
				}
			case 'd': //show a disassembly of the memory
				{
					dFlag = true;
					break;
				}
			case 'i': //show insn printing during execution
				{
					iFlag = true;
					break;
				}
			case 'r': //show a dump of the hart, gp and pc, before each insn
				{
					rFlag = true;
					break;
				}
			case 'z': //show a dump of the hart status and memory after the simulation ends
				{
					zFlag = true;
					break;
				}
			case 'l': //needs execution val. specifies max limit of insns, if 0, run forever
				{
					std::istringstream iss(optarg);
					iss >> std::hex >> exec_limit;
					break;
				}
			default:
				  usage();
				  break;
		}
	}

	if (optind >= argc)
		usage();	

	memory mem(memory_limit);

	if (!mem.load_file(argv[optind]))
		usage();

	if (dFlag)
		disassemble(mem);
	
	cpu_single_hart cpu(mem);
	cpu.reset();

	if (iFlag)
		cpu.set_show_instructions(true);

	if (rFlag)
		cpu.set_show_registers(true);


	if (zFlag){
		cpu.dump();
		mem.dump();
	}

	cpu.run(exec_limit);

	return 0;
}
