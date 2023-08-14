#include "cpu_single_hart.h"

void cpu_single_hart::run(uint64_t exec_limit) 
{
	regs.set(2, mem.get_size());

	if (exec_limit == 0){
		while (!is_halted())
			rv32i_hart::tick();
	}
	else {
		while (!is_halted() && get_insn_counter() < exec_limit)
			rv32i_hart::tick();
	}
	cout << "Execution terminated. Reason: " << get_halt_reason() << endl;
	cout << rv32i_hart::get_insn_counter() << " instructions executed" << endl;
}
