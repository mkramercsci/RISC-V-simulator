#include "rv32i_hart.h"
#include <bitset>


void rv32i_hart::exec (uint32_t insn, std::ostream* pos) 
{
	uint32_t opcode = get_opcode(insn);
	switch ( opcode )
	{
	default : 		exec_illegal_insn ( insn , pos ) ; return ;
	case opcode_lui : 	exec_lui ( insn , pos ) ; return ;
	case opcode_auipc : 	exec_auipc ( insn , pos ); return ;
	case opcode_jal:	exec_jal ( insn , pos ); return ;
	case opcode_jalr:	exec_jalr ( insn , pos ); return ;
	case opcode_btype: 	
		switch (get_funct3(insn))
		{
			default:		exec_illegal_insn ( insn , pos ) ; return ;
			case funct3_beq:	exec_beq ( insn , pos ); return ;
			case funct3_bne:	exec_bne ( insn , pos ); return ;
			case funct3_blt:	exec_blt(insn,pos);return;
			case funct3_bge:	exec_bge(insn,pos);return;
			case funct3_bltu:	exec_bltu(insn,pos);return;
			case funct3_bgeu:	exec_bgeu(insn,pos);return;
		}
	case opcode_load_imm:
		switch (get_funct3(insn))
		{
			default:		exec_illegal_insn ( insn , pos ) ; return ;
			case funct3_lb:		exec_lb(insn,pos);return;
			case funct3_lh:		exec_lh(insn,pos);return;
			case funct3_lw:		exec_lw(insn,pos);return;
			case funct3_lbu:	exec_lbu(insn,pos);return;
			case funct3_lhu:	exec_lhu(insn,pos);return;
		}	
	case opcode_stype:
		switch (get_funct3(insn))
		{
		default:		exec_illegal_insn ( insn , pos ) ; return ;
		case funct3_sb:		exec_sb(insn,pos);return;
		case funct3_sh:		exec_sh(insn,pos);return;
		case funct3_sw:		exec_sw(insn,pos);return;
		}

	case opcode_alu_imm:
		switch (get_funct3(insn))
		{//the labels for these may not be right ?? where is srai

		default:		exec_illegal_insn ( insn , pos ) ; return ;
		case funct3_add:	exec_addi(insn,pos);return;
		case funct3_sll:	exec_slli(insn,pos);return;
		case funct3_slt:	exec_slti(insn,pos);return;
		case funct3_sltu:	exec_sltiu(insn,pos);return;
		case funct3_xor:	exec_xor(insn,pos);return;
		case funct3_srx:	
			switch (get_funct7(insn))
			{
			default:		exec_illegal_insn ( insn , pos ) ; return ;
			case funct7_srl:	exec_srli(insn,pos);return;
			case funct7_sra:	exec_srai(insn,pos);return;
			}
		case funct3_or:		exec_or(insn,pos);return;
		case funct3_and:	exec_andi(insn,pos);return;
		}
	case opcode_rtype:
		switch (get_funct3(insn))
		{
		default:			exec_illegal_insn ( insn , pos ) ; return ;
		case funct3_add: 
			switch (get_funct7(insn))
			{
			default:			exec_illegal_insn ( insn , pos ) ; return ;
			case funct7_add:		exec_add(insn,pos);return;
			case funct7_sub:		exec_sub(insn,pos);return;
			}//end of funct7 add/sub switch
		case funct3_sll:		exec_sll(insn,pos);return;
		case funct3_slt:		exec_slt(insn,pos);return;
		case funct3_sltu:		exec_sltu(insn,pos);return;
		case funct3_xor:		exec_xorr(insn,pos);return;
		case funct3_srx:
			switch (get_funct7(insn))
			{
			default:		exec_illegal_insn ( insn , pos ) ; return; 
			case funct7_srl:	exec_srl(insn,pos);return;
			case funct7_sra:	exec_sra(insn,pos);return;
			}
		case funct3_or:			exec_orr(insn,pos);return;
		case funct3_and:		exec_and(insn,pos);return;
		}//end of rtype switch
	case opcode_system:
		//switch for cssx
		switch (get_funct3(insn))
		{
		default:			exec_illegal_insn ( insn , pos ) ; return;
		case funct3_csrrs:		exec_csrrs(insn,pos);return;
		case 0: 			
				switch(get_imm_i(insn)) {
				default:		exec_illegal_insn ( insn , pos ) ; return;				
				case 1: 		exec_ebreak(insn,pos);return;
				}		
		}
	}//opcode switch
}

void rv32i_hart::dump (const std::string& hdr) const
{
	regs.dump(hdr);

	if (hdr[0])
		cout << hdr << " ";
	cout << " pc " << hex::to_hex32(pc) << "\n";
}

void rv32i_hart::reset () 
{
	pc = 0;
	regs.reset();
	insn_counter = 0;
	halt = false;
	halt_reason = "none";
}

void rv32i_hart::tick(const std::string& hdr) 
{
	if (is_halted())
		return;

	insn_counter++;

	if (show_registers) 
		rv32i_hart::dump(hdr);

	uint32_t insn = mem.get32(pc);

	if (show_instructions) {
		//print the header, pc, fetched insn
		cout << hex::to_hex32(pc) << ": " << hex::to_hex32(insn) << "  ";

		exec(insn, &std::cout);
		cout << endl;
	}
	else {
		exec(insn, nullptr);
	}	
}

void rv32i_hart :: exec_ebreak ( uint32_t insn , std :: ostream * pos )
{
	if (pos)
	{
		std::string s = render_ebreak ( insn );
		*pos << std :: setw ( instruction_width ) << std :: setfill (' ') << std :: left << s;
		*pos << "// HALT ";
	}
	halt = true ;
	halt_reason = " EBREAK instruction ";
}


void rv32i_hart::exec_illegal_insn(uint32_t insn , std::ostream* pos)
{
	(void) insn;
	if (pos)
		*pos << render_illegal_insn(insn);
	halt = true;
	halt_reason = "Illegal instruction";
}

void rv32i_hart::exec_lui(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd (insn);
	uint32_t imm_u = get_imm_u (insn);

	int32_t val = imm_u;

	if (pos) 
	{
		std::string s = render_lui(insn);
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(imm_u);
	}

	regs.set(rd, val);

	pc += 4;
}

void rv32i_hart::exec_auipc(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd (insn);
	uint32_t imm_u = get_imm_u (insn);

	int32_t val = imm_u + pc;

	if (pos) 
	{
		std::string s = render_auipc(insn);
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(pc) << " + " << hex::to_hex0x32(imm_u);
		*pos << " = " << hex::to_hex0x32(val);
	}

	regs.set(rd, val);

	pc += 4;
}

void rv32i_hart::exec_jal(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd (insn);
	uint32_t imm_j = get_imm_j (insn);

	int32_t valA = pc + 4;
	int32_t valB = pc + imm_j;

	if (pos) 
	{
		std::string s = render_jal(pc, insn);
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(valA) << ", pc = " << hex::to_hex0x32(pc);
		*pos << " + " << hex::to_hex0x32(imm_j) << " = " << hex::to_hex0x32(valB);
	}

	regs.set(rd, valA);
	pc = valB;
}

void rv32i_hart::exec_jalr(uint32_t insn, std::ostream* pos) //THIS ONE DOESN'T WORK RIGHT
{

	uint32_t rd = get_rd(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t imm_i = get_imm_i(insn);

	int32_t rs1Val;
	rs1Val = regs.get(rs1);

	int32_t pcVal;
	pcVal = (rs1Val + imm_i) & 0xfffffffe;

	if (pos) 
	{
		std::string s = render_jalr(insn);
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(pc + 4) << ", pc = (";
		*pos << hex::to_hex0x32(imm_i) << " + " << hex::to_hex0x32(rs1Val) << ") & 0xfffffffe = ";
		*pos << hex::to_hex0x32(pcVal);
	}

	regs.set(rd, pc + 4);
	pc = pcVal;
}

void rv32i_hart::exec_beq(uint32_t insn, std::ostream* pos) 
{
	int32_t rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);
	int32_t imm_b = get_imm_b(insn);

	int32_t rs1Val;
	rs1Val = regs.get(rs1);

	int32_t rs2Val;
	rs2Val = regs.get(rs2);

	int32_t pcVal = pc;


	if (rs1Val == rs2Val)
		pcVal += imm_b;
	else
		pcVal +=4;

	if (pos) 
	{
		std::string s = render_btype(pc, insn, "beq");
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// pc += (" << hex::to_hex0x32(rs1Val) << " == " << hex::to_hex0x32(rs2Val) << " ? ";
		*pos << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(pcVal);	
	}

	pc = pcVal;
}

void rv32i_hart::exec_bne(uint32_t insn, std::ostream* pos) 
{
	int32_t rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);
	int32_t imm_b = get_imm_b(insn);

	int32_t rs1Val;
	rs1Val = regs.get(rs1);

	int32_t rs2Val;
	rs2Val = regs.get(rs2);

	int32_t pcVal = pc;


	if (rs1Val != rs2Val)
		pcVal += imm_b;
	else
		pcVal +=4;

	if (pos) 
	{
		std::string s = render_btype(pc, insn, "bne");
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// pc += (" << hex::to_hex0x32(rs1Val) << " != " << hex::to_hex0x32(rs2Val) << " ? ";
		*pos << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(pcVal);	
	}

	pc = pcVal;
}

void rv32i_hart::exec_blt(uint32_t insn, std::ostream* pos) 
{
	int32_t rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);
	int32_t imm_b = get_imm_b(insn);

	int32_t rs1Val;
	rs1Val = regs.get(rs1);

	int32_t rs2Val;
	rs2Val = regs.get(rs2);

	int32_t pcVal = pc;


	if (rs1Val < rs2Val)
		pcVal += imm_b;
	else
		pcVal +=4;

	if (pos) 
	{
		std::string s = render_btype(pc, insn, "blt");
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// pc += (" << hex::to_hex0x32(rs1Val) << " < " << hex::to_hex0x32(rs2Val) << " ? ";
		*pos << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(pcVal);	
	}

	pc = pcVal;
}

void rv32i_hart::exec_bge(uint32_t insn, std::ostream* pos) 
{
	int32_t rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);
	int32_t imm_b = get_imm_b(insn);

	int32_t rs1Val;
	rs1Val = regs.get(rs1);

	int32_t rs2Val;
	rs2Val = regs.get(rs2);

	int32_t pcVal = pc;


	if (rs1Val >= rs2Val)
		pcVal += imm_b;
	else
		pcVal +=4;

	if (pos) 
	{
		std::string s = render_btype(pc, insn, "bge");
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// pc += (" << hex::to_hex0x32(rs1Val) << " >= " << hex::to_hex0x32(rs2Val) << " ? ";
		*pos << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(pcVal);	
	}

	pc = pcVal;
}


void rv32i_hart::exec_bltu(uint32_t insn, std::ostream* pos) 
{
	int32_t rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);
	int32_t imm_b = get_imm_b(insn);

	uint32_t rs1Val;
	rs1Val = regs.get(rs1);

	uint32_t rs2Val;
	rs2Val = regs.get(rs2);

	int32_t pcVal = pc;


	if (rs1Val < rs2Val)
		pcVal += imm_b;
	else
		pcVal +=4;

	if (pos) 
	{
		std::string s = render_btype(pc, insn, "bltu");
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// pc += (" << hex::to_hex0x32(rs1Val) << " <U " << hex::to_hex0x32(rs2Val) << " ? ";
		*pos << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(pcVal);	
	}

	pc = pcVal;
}

void rv32i_hart::exec_bgeu(uint32_t insn, std::ostream* pos) 
{
	int32_t rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);
	int32_t imm_b = get_imm_b(insn);

	uint32_t rs1Val;
	rs1Val = regs.get(rs1);

	uint32_t rs2Val;
	rs2Val = regs.get(rs2);

	int32_t pcVal = pc;


	if (rs1Val >= rs2Val)
		pcVal += imm_b;
	else
		pcVal +=4;

	if (pos) 
	{
		std::string s = render_btype(pc, insn, "bgeu");
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// pc += (" << hex::to_hex0x32(rs1Val) << " >=U " << hex::to_hex0x32(rs2Val) << " ? ";
		*pos << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(pcVal);	
	}

	pc = pcVal;
}
////////////////////////////////////////////////////////

void rv32i_hart::exec_lb(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t imm_i = get_imm_i(insn);

	int32_t rs1Val;
	rs1Val = regs.get(rs1);
	
	int32_t fetch;
	fetch = rs1Val + imm_i;

	int8_t rdVal;
	rdVal = mem.get8(fetch);	

	int32_t newVal = rdVal;

	if (pos) 
	{
		std::string s = render_itype_load(insn, "lb");
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// " << render_reg(rd) << " = sx(m8(" << hex::to_hex0x32(rs1Val) << " + ";
		*pos << hex::to_hex0x32(imm_i) << ")) = " << hex::to_hex0x32(newVal);
	}
	
	regs.set(rd, newVal);
	pc += 4;
}

void rv32i_hart::exec_lw(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t imm_i = get_imm_i(insn);

	int32_t rs1Val;
	rs1Val = regs.get(rs1);
	
	int32_t fetch;
	fetch = rs1Val + imm_i;

	int32_t rdVal;
	rdVal = mem.get32(fetch);	

	if (pos) 
	{
		std::string s = render_itype_load(insn, "lw");
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// " << render_reg(rd) << " = sx(m32(" << hex::to_hex0x32(rs1Val) << " + ";
		*pos << hex::to_hex0x32(imm_i) << ")) = " << hex::to_hex0x32(rdVal);
	}
	
	regs.set(rd, rdVal);
	pc += 4;
}

void rv32i_hart::exec_lh(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t imm_i = get_imm_i(insn);

	int32_t rs1Val;
	rs1Val = regs.get(rs1);
	
	int32_t fetch;
	fetch = rs1Val + imm_i;

	int16_t rdVal;
	rdVal = mem.get16(fetch);	

	int32_t newVal = rdVal;

	if (pos) 
	{
		std::string s = render_itype_load(insn, "lh");
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// " << render_reg(rd) << " = sx(m16(" << hex::to_hex0x32(rs1Val) << " + ";
		*pos << hex::to_hex0x32(imm_i) << ")) = " << hex::to_hex0x32(newVal);
	}
	
	regs.set(rd, newVal);
	pc += 4;
}


void rv32i_hart::exec_lbu(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t imm_i = get_imm_i(insn);

	uint32_t rs1Val;
	rs1Val = regs.get(rs1);
	
	uint32_t fetch;
	fetch = rs1Val + imm_i;

	int32_t rdVal;
	rdVal = mem.get8(fetch);

	if (pos) 
	{
		std::string s = render_itype_load(insn, "lbu");
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// " << render_reg(rd) << " = zx(m8(" << hex::to_hex0x32(rs1Val) << " + ";
		*pos << hex::to_hex0x32(imm_i) << ")) = " << hex::to_hex0x32(rdVal);
	}
	
	regs.set(rd, rdVal);
	pc += 4;
}

void rv32i_hart::exec_lhu(uint32_t insn, std::ostream* pos)
{	
	uint32_t rd = get_rd(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t imm_i = get_imm_i(insn);

	uint32_t rs1Val;
	rs1Val = regs.get(rs1);
	
	uint32_t fetch;
	fetch = rs1Val + imm_i;

	int32_t rdVal;
	rdVal = mem.get16(fetch);

	if (pos) 
	{
		std::string s = render_itype_load(insn, "lhu");
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// " << render_reg(rd) << " = zx(m16(" << hex::to_hex0x32(rs1Val) << " + ";
		*pos << hex::to_hex0x32(imm_i) << ")) = " << hex::to_hex0x32(rdVal);
	}
	
	regs.set(rd, rdVal);
	pc += 4;
}

void rv32i_hart::exec_sb(uint32_t insn , std :: ostream *pos)
{
	int32_t rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);
	int32_t imm_s = get_imm_s(insn);

	uint32_t rs1Val;
	rs1Val = regs.get(rs1);

	uint32_t rs2Val;
	rs2Val = regs.get(rs2);

	uint32_t addr = rs1Val + imm_s;

	int32_t newVal = rs2Val & 0x000000ff;

	if (pos) 
	{
		std::string s = render_stype(insn, "sb");
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// m8(" << hex::to_hex0x32(rs1Val) << " + " << hex::to_hex0x32(imm_s) << ") = ";
		*pos << hex::to_hex0x32(newVal);
	}

	mem.set8(addr, newVal);
	pc += 4;
}

void rv32i_hart::exec_sh(uint32_t insn , std :: ostream *pos)
{
	int32_t rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);
	int32_t imm_s = get_imm_s(insn);

	uint32_t rs1Val;
	rs1Val = regs.get(rs1);

	uint32_t rs2Val;
	rs2Val = regs.get(rs2);

	uint32_t addr = rs1Val + imm_s;

	int32_t newVal = rs2Val & 0x0000ffff;

	if (pos) 
	{
		std::string s = render_stype(insn, "sh");
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// m16(" << hex::to_hex0x32(rs1Val) << " + " << hex::to_hex0x32(imm_s) << ") = ";
		*pos << hex::to_hex0x32(newVal);
	}

	mem.set16(addr, newVal);
	pc += 4;

}

void rv32i_hart::exec_sw(uint32_t insn , std :: ostream *pos)
{
	int32_t rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);
	int32_t imm_s = get_imm_s(insn);

	uint32_t rs1Val;
	rs1Val = regs.get(rs1);

	uint32_t rs2Val;
	rs2Val = regs.get(rs2);

	uint32_t addr = rs1Val + imm_s;

	if (pos) 
	{
		std::string s = render_stype(insn, "sw");
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// m32(" << hex::to_hex0x32(rs1Val) << " + " << hex::to_hex0x32(imm_s) << ") = ";
		*pos << hex::to_hex0x32(rs2Val);
	}

	mem.set32(addr, rs2Val);
	pc += 4;
}

void rv32i_hart::exec_addi(uint32_t insn , std :: ostream *pos)
{	
	uint32_t rd = get_rd(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t imm_i = get_imm_i(insn);

	int32_t rs1Val;
	rs1Val = regs.get(rs1);

	int32_t rdVal;
	rdVal = rs1Val + imm_i;
	
	if (pos) 
	{
		std::string s = render_itype_alu(insn, "addi", imm_i);
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Val) << " + ";
		*pos << hex::to_hex0x32(imm_i) << " = " << hex::to_hex0x32(rdVal);
	}

	regs.set(rd, rdVal);
	pc += 4;
}

void rv32i_hart::exec_slti(uint32_t insn , std :: ostream *pos)
{	
	uint32_t rd = get_rd(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t imm_i = get_imm_i(insn);

	int32_t rs1Val;
	rs1Val = regs.get(rs1);

	int32_t rdVal = 0;

	if (rs1Val < imm_i)
		rdVal = 1;

	if (pos) 
	{
		std::string s = render_itype_alu(insn, "slti", imm_i);
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// " << render_reg(rd) << " = (" << hex::to_hex0x32(rs1Val) << " < ";
		*pos << imm_i << " ? 1 : 0 = " << hex::to_hex0x32(rdVal);
	}

	regs.set(rd, rdVal);
	pc += 4;
}

void rv32i_hart::exec_sltiu(uint32_t insn , std :: ostream *pos)
{
	uint32_t rd = get_rd(insn);
	int32_t rs1 = get_rs1(insn);
	uint32_t imm_i = get_imm_i(insn);

	uint32_t rs1Val;
	rs1Val = regs.get(rs1);

	int32_t rdVal = 0;

	if (rs1Val < imm_i)
		rdVal = 1;

	if (pos) 
	{
		std::string s = render_itype_alu(insn, "sltiu", imm_i);
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// " << render_reg(rd) << " = (" << hex::to_hex0x32(rs1Val) << " <U ";
		*pos << imm_i << " ? 1 : 0 = " << hex::to_hex0x32(rdVal);
	}


	regs.set(rd, rdVal);
	pc += 4;
}	

void rv32i_hart::exec_xor(uint32_t insn , std :: ostream *pos)
{

	uint32_t rd = get_rd(insn);
	int32_t rs1 = get_rs1(insn);
	uint32_t imm_i = get_imm_i(insn);

	uint32_t rs1Val;
	rs1Val = regs.get(rs1);

	uint32_t rdVal;
	rdVal = rs1Val ^ imm_i;

	if (pos) 
	{
		std::string s = render_itype_alu(insn, "xori", imm_i);
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Val) << " ^ ";
		*pos << hex::to_hex0x32(imm_i) << " = " << hex::to_hex0x32(rdVal);
	}

	regs.set(rd, rdVal);
	pc += 4;
}

void rv32i_hart::exec_or(uint32_t insn , std :: ostream *pos)
{
	uint32_t rd = get_rd(insn);
	int32_t rs1 = get_rs1(insn);
	uint32_t imm_i = get_imm_i(insn);

	uint32_t rs1Val;
	rs1Val = regs.get(rs1);

	uint32_t rdVal;
	rdVal = rs1Val | imm_i;

	if (pos) 
	{
		std::string s = render_itype_alu(insn, "ori", imm_i);
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Val) << " | ";
		*pos << hex::to_hex0x32(imm_i) << " = " << hex::to_hex0x32(rdVal);
	}

	regs.set(rd, rdVal);
	pc += 4;
}

void rv32i_hart::exec_slli(uint32_t insn , std :: ostream *pos)
{
	uint32_t rd = get_rd(insn);
	int32_t rs1 = get_rs1(insn);
	uint32_t imm_i = get_imm_i(insn);

	uint32_t rs1Val;
	rs1Val = regs.get(rs1);

	uint32_t rdVal;
	rdVal = rs1Val << imm_i;

	if (pos) 
	{
		std::string s = render_itype_alu(insn, "slli", imm_i);
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Val) << " << ";
		*pos << imm_i << " = " << hex::to_hex0x32(rdVal);
	}

	regs.set(rd, rdVal);
	pc += 4;
}

void rv32i_hart::exec_andi(uint32_t insn , std :: ostream *pos)
{
	uint32_t rd = get_rd(insn);
	int32_t rs1 = get_rs1(insn);
	uint32_t imm_i = get_imm_i(insn);

	uint32_t rs1Val;
	rs1Val = regs.get(rs1);

	uint32_t rdVal;
	rdVal = rs1Val & imm_i;

	if (pos) 
	{
		std::string s = render_itype_alu(insn, "andi", imm_i);
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Val) << " & ";
		*pos << hex::to_hex0x32(imm_i) << " = " << hex::to_hex0x32(rdVal);
	}

	regs.set(rd, rdVal);
	pc += 4;
}

void rv32i_hart::exec_srli(uint32_t insn , std :: ostream *pos)
{
	uint32_t rd = get_rd(insn);
	int32_t rs1 = get_rs1(insn);
	uint32_t imm_i = get_imm_i(insn);

	uint32_t rs1Val;
	rs1Val = regs.get(rs1);

	uint32_t rdVal;
	rdVal = rs1Val >> imm_i;

	if (pos) 
	{
		std::string s = render_itype_alu(insn, "srli", imm_i);
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Val) << " >> ";
		*pos << hex::to_hex0x32(imm_i) << " = " << hex::to_hex0x32(rdVal);
	}

	regs.set(rd, rdVal);
	pc += 4;
}

void rv32i_hart::exec_srai(uint32_t insn , std :: ostream *pos)
{
	uint32_t rd = get_rd(insn);
	int32_t rs1 = get_rs1(insn);
	uint32_t imm_i = get_imm_i(insn);

	int32_t rs1Val;
	rs1Val = regs.get(rs1);

	int32_t rdVal;
	rdVal = rs1Val >> imm_i;

	if (pos) 
	{
		std::string s = render_itype_alu(insn, "srai", imm_i);
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Val) << " >> ";
		*pos << hex::to_hex0x32(imm_i) << " = " << hex::to_hex0x32(rdVal);
	}

	regs.set(rd, rdVal);
	pc += 4;
}

void rv32i_hart::exec_add(uint32_t insn , std :: ostream *pos)
{
	uint32_t rd = get_rd(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);

	int32_t rs1Val;
	rs1Val = regs.get(rs1);

	int32_t rs2Val;
	rs2Val = regs.get(rs2);

	int32_t rdVal = rs1Val + rs2Val;

	if (pos) 
	{
		std::string s = render_rtype(insn, "add");
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Val) << " + ";
		*pos << hex::to_hex0x32(rs2Val) << " = " << hex::to_hex0x32(rdVal);
	}

	regs.set(rd, rdVal);
	pc += 4;
}

void rv32i_hart::exec_sub(uint32_t insn , std :: ostream *pos)
{
	uint32_t rd = get_rd(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);

	int32_t rs1Val;
	rs1Val = regs.get(rs1);

	int32_t rs2Val;
	rs2Val = regs.get(rs2);

	int32_t rdVal = rs1Val - rs2Val;

	if (pos) 
	{
		std::string s = render_rtype(insn, "sub");
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Val) << " - ";
		*pos << hex::to_hex0x32(rs2Val) << " = " << hex::to_hex0x32(rdVal);
	}

	regs.set(rd, rdVal);
	pc += 4;

}

void rv32i_hart::exec_sll(uint32_t insn , std :: ostream *pos)
{
	uint32_t rd = get_rd (insn);
	uint32_t rs1 = get_rs1 (insn);
	uint32_t rs2 = get_rs2 (insn);

	int32_t rs1Val;
	rs1Val = regs.get(rs1);

	int32_t rs2Val;
	rs2Val = regs.get(rs2);

	int32_t LSB = rs2Val & 0x0000001f;

	int32_t rdVal = rs1Val << LSB;

	if (pos)
	{
		std::string s = render_rtype(insn , "sll");
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(regs.get(rs1)) << " << " << LSB << " = ";
		*pos << hex::to_hex0x32(rdVal);
	}
	regs.set(rd, rdVal);
	pc += 4;
}


void rv32i_hart::exec_slt (uint32_t insn , std :: ostream *pos)
{
	uint32_t rd = get_rd (insn);
	uint32_t rs1 = get_rs1 (insn);
	uint32_t rs2 = get_rs2 (insn);

	int32_t val = (regs.get (rs1) < regs.get (rs2)) ? 1 : 0;

	if (pos)
	{
		std::string s = render_rtype(insn , "slt");
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// " << render_reg(rd) << " = (" << hex::to_hex0x32(regs.get(rs1)) << " < " << hex ::to_hex0x32(regs.get(rs2)) << ") ? 1 : 0 = " << hex::to_hex0x32(val);
	}
	regs.set(rd, val);
	pc += 4;
}

void rv32i_hart::exec_sltu(uint32_t insn , std :: ostream *pos)
{
	uint32_t rd = get_rd (insn);
	uint32_t rs1 = get_rs1 (insn);
	uint32_t rs2 = get_rs2 (insn);

	uint32_t rs1Val;
	rs1Val = regs.get(rs1);

	uint32_t rs2Val;
	rs2Val = regs.get(rs2);

	int32_t rdVal = 0;

	if (rs1Val < rs2Val)
		rdVal = 1;

	if (pos)
	{
		std::string s = render_rtype(insn , "sltu");
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// " << render_reg(rd) << " = (" << hex::to_hex0x32(rs1Val) << " <U " << hex::to_hex0x32(rs2Val) << ") ? 1 : 0 = ";
		*pos << hex::to_hex0x32(rdVal);
	}

	regs.set(rd, rdVal);
	pc += 4;
}

void rv32i_hart::exec_xorr(uint32_t insn , std :: ostream *pos)
{
	uint32_t rd = get_rd(insn);
	int32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2 (insn);

	uint32_t rs1Val;
	rs1Val = regs.get(rs1);

	uint32_t rs2Val;
	rs2Val = regs.get(rs2);

	uint32_t rdVal;
	rdVal = rs1Val ^ rs2Val;

	if (pos) 
	{
		std::string s = render_rtype(insn, "xor");
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Val) << " ^ ";
		*pos << hex::to_hex0x32(rs2Val) << " = " << hex::to_hex0x32(rdVal);
	}

	regs.set(rd, rdVal);
	pc += 4;
}


void rv32i_hart::exec_srl(uint32_t insn , std :: ostream *pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2 (insn);

	uint32_t rs1Val;
	rs1Val = regs.get(rs1);

	uint32_t rs2Val;
	rs2Val = regs.get(rs2);

	uint32_t rdVal;
	rdVal = rs1Val >> rs2Val;

	if (pos) 
	{
		std::string s = render_rtype(insn, "srl");
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Val) << " >> ";
		*pos << rs2Val << " = " << hex::to_hex0x32(rdVal);
	}

	regs.set(rd, rdVal);
	pc += 4;
}

void rv32i_hart::exec_sra(uint32_t insn , std :: ostream *pos)
{
	uint32_t rd = get_rd(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2 (insn);

	int32_t rs1Val;
	rs1Val = regs.get(rs1);

	int32_t rs2Val;
	rs2Val = regs.get(rs2);

	int32_t rdVal;
	rdVal = rs1Val >> rs2Val;

	if (pos) 
	{
		std::string s = render_rtype(insn, "sra");
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Val) << " >> ";
		*pos << rs2Val << " = " << hex::to_hex0x32(rdVal);
	}

	regs.set(rd, rdVal);
	pc += 4;
}

void rv32i_hart::exec_and(uint32_t insn , std :: ostream *pos)
{
	uint32_t rd = get_rd(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2 (insn);

	int32_t rs1Val;
	rs1Val = regs.get(rs1);

	int32_t rs2Val;
	rs2Val = regs.get(rs2);

	int32_t rdVal;
	rdVal = rs1Val & rs2Val;

	if (pos) 
	{
		std::string s = render_rtype(insn, "and");
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Val) << " & ";
		*pos << hex::to_hex0x32(rs2Val) << " = " << hex::to_hex0x32(rdVal);
	}

	regs.set(rd, rdVal);
	pc += 4;
}

void rv32i_hart::exec_orr(uint32_t insn , std :: ostream *pos)
{
	uint32_t rd = get_rd(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2 (insn);

	int32_t rs1Val;
	rs1Val = regs.get(rs1);

	int32_t rs2Val;
	rs2Val = regs.get(rs2);

	int32_t rdVal;
	rdVal = rs1Val | rs2Val;

	if (pos) 
	{
		std::string s = render_rtype(insn, "or");
		*pos << std::setw(instruction_width) << std :: setfill(' ') << std :: left << s;
		*pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Val) << " | ";
		*pos << hex::to_hex0x32(rs2Val) << " = " << hex::to_hex0x32(rdVal);
	}

	regs.set(rd, rdVal);
	pc += 4;
}

void rv32i_hart::exec_csrrs(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t csr = get_imm_i(insn) & 0x00000fff;;

    if (csr != 0xf14 || rs1 != 0)
    {
        halt = true;
        halt_reason = "Illegal CSR in CRRSS instruction";
    }

    if (pos)
    {
        std::string s = render_csrrx(insn, "csrrs");
        s.resize(instruction_width, ' ');
        *pos << s << "// " << render_reg(rd) << " = " << std::dec << mhartid;
    }
    if (!halt)
    {
        regs.set(rd, mhartid);
        pc += 4;
    }
} 
