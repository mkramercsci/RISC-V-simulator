#include "rv32i_decode.h"

#include <bitset>

std::string rv32i_decode::decode(uint32_t addr, uint32_t insn)
{
	switch (get_opcode(insn))
	{
	default:			return render_illegal_insn(insn);
	case opcode_lui:	return render_lui(insn);
	case opcode_auipc:	return render_auipc(insn);
	case opcode_jal:	return render_jal(addr, insn);
	case opcode_jalr:	return render_jalr(insn);
	case opcode_btype:
		switch (get_funct3(insn))
		{
		default:			return render_illegal_insn(insn);
		case funct3_beq:	return render_btype(addr, insn, "beq");
		case funct3_bne:	return render_btype(addr, insn, "bne");
		case funct3_blt:	return render_btype(addr, insn, "blt");
		case funct3_bge:	return render_btype(addr, insn, "bge");
		case funct3_bltu:	return render_btype(addr, insn, "bltu");
		case funct3_bgeu:	return render_btype(addr, insn, "bgeu");
		}
	case opcode_load_imm:
		switch (get_funct3(insn))
		{
		default:			return render_illegal_insn(insn);
		case funct3_lb:		return render_itype_load(insn, "lb");
		case funct3_lh:		return render_itype_load(insn, "lh");
		case funct3_lw:		return render_itype_load(insn, "lw");
		case funct3_lbu:	return render_itype_load(insn, "lbu");
		case funct3_lhu:	return render_itype_load(insn, "lhu");
		}
	case opcode_stype:
		switch (get_funct3(insn))
		{
		default:			return render_illegal_insn(insn);
		case funct3_sb:		return render_stype(insn, "sb");
		case funct3_sh:		return render_stype(insn, "sh");
		case funct3_sw:		return render_stype(insn, "sw");

		}
	case opcode_alu_imm:
		switch (get_funct3(insn))
		{//the labels for these may not be right ?? where is srai

		default:			return render_illegal_insn(insn);
		case funct3_add:	return render_itype_alu(insn, "addi", get_imm_i(insn));
		case funct3_sll:	return render_itype_alu(insn, "slli", get_imm_i(insn));
		case funct3_slt:	return render_itype_alu(insn, "slti", get_imm_i(insn));
		case funct3_sltu:	return render_itype_alu(insn, "sltiu", get_imm_i(insn));
		case funct3_xor:	return render_itype_alu(insn, "xori", get_imm_i(insn));
		case funct3_srx:	
			switch (get_funct7(insn))
			{
			default:			return render_illegal_insn(insn);
			case funct7_srl:	return render_itype_alu(insn, "srli", get_imm_i(insn));
			case funct7_sra:	return render_itype_alu(insn, "srai", get_imm_i(insn));
			}
		case funct3_or:		return render_itype_alu(insn, "ori", get_imm_i(insn));
		case funct3_and:	return render_itype_alu(insn, "andi", get_imm_i(insn));
		}
	case opcode_rtype:
		switch (get_funct3(insn))
		{
		default:			return render_illegal_insn(insn);
		case funct3_add: 
			switch (get_funct7(insn))
			{
			default:			return render_illegal_insn(insn);
			case funct7_add:		return render_rtype(insn, "add");
			case funct7_sub:		return render_rtype(insn, "sub");
			}//end of funct7 add/sub switch
		case funct3_sll:		return render_rtype(insn, "sll");
		case funct3_slt:		return render_rtype(insn, "slt");
		case funct3_sltu:		return render_rtype(insn, "sltu");
		case funct3_xor:		return render_rtype(insn, "xor");
		case funct3_srx:
			switch (get_funct7(insn))
			{
			default:			return render_illegal_insn(insn);
			case funct7_srl:	return render_rtype(insn, "srl");
			case funct7_sra:	return render_rtype(insn, "sra");
			}
		case funct3_or:		return render_rtype(insn, "or");
		case funct3_and:		return render_rtype(insn, "and");
		}//end of rtype switch
	case opcode_system:
		//switch for cssx
		switch (get_funct3(insn))
		{
		default:			return render_illegal_insn(insn);
		case funct3_csrrw:		return render_csrrx(insn, "csrrw");
		case funct3_csrrs:		return render_csrrx(insn, "csrrs");
		case funct3_csrrc:		return render_csrrx(insn, "csrrc");
		case funct3_csrrwi:		return render_csrrxi(insn, "csrrwi");
		case funct3_csrrsi:		return render_csrrxi(insn, "csrrsi");
		case funct3_csrrci:		return render_csrrxi(insn, "csrrci");
		case 0: 			
				switch(get_imm_i(insn)) {
				default:		return render_illegal_insn(insn);
				case 0:			return render_ecall(insn);
				case 1: 		return render_ebreak(insn);
				}		
		}
	}//end of opcode switch
	assert(0 && "unrecognized opcode"); // It should be impossible to ever get here
}

uint32_t rv32i_decode::get_opcode(uint32_t insn)
{
	return (insn & 0x0000007f); 
}

uint32_t rv32i_decode::get_rd(uint32_t insn)
{
	return ((insn & 0x00000f80) >> 7);
}

uint32_t rv32i_decode::get_funct3(uint32_t insn)
{
	return (insn & 0x00007000) >> 12;
}

uint32_t rv32i_decode::get_rs1(uint32_t insn)
{
	return (insn & 0x000f8000) >> 15;
}

uint32_t rv32i_decode::get_rs2(uint32_t insn)
{
	return (insn & 0x01f00000) >> 20;
}

uint32_t rv32i_decode::get_funct7(uint32_t insn)
{
	return (insn & 0xfe000000) >> 25;
}

int32_t rv32i_decode::get_imm_i(uint32_t insn)
{

	int32_t val = insn;

	val >>= 20;
	
	return val;
}

int32_t rv32i_decode::get_imm_u(uint32_t insn)
{
	return (insn & 0xfffff000);
}

int32_t rv32i_decode::get_imm_b(uint32_t insn)
{
	int32_t ux_range = (insn & 0x00000f00) >> 7;
		
	int32_t y_bit = (insn & 0x00000080) << 4;
	
	int32_t bg_range = (insn & 0x7e000000) >> 20;
		
	int32_t a_bits = (insn & 0x80000000);
	a_bits >>= 31 - 12;
	
	return a_bits | y_bit | bg_range | ux_range;
}

int32_t rv32i_decode::get_imm_s(uint32_t insn)
{
	int32_t imm_s = (insn & 0xfe000000) >> (25 - 5);
	imm_s |= (insn & 0x00000f80) >> (7 - 0);
	if (insn & 0x80000000)
		imm_s |= 0xfffff000; // sign-extend the left
	return imm_s;
}

int32_t rv32i_decode::get_imm_j(uint32_t insn)
{
	int32_t mt_range = insn & 0x000ff000;

	int32_t bk_range = insn >> 20;

	int32_t l_bit = (insn & 0x001000000) >> 9;

	int32_t a_bits = (insn & 0x80000000) >> 11;

	return a_bits | mt_range | l_bit | bk_range;
}

std::string rv32i_decode::render_illegal_insn(uint32_t insn)
{
	insn = insn;
	return "ERROR: UNIMPLEMENTED INSTRUCTION";
}

std::string rv32i_decode::render_lui(uint32_t insn)
{
	uint32_t rd = get_rd(insn);
	int32_t imm_u = get_imm_u(insn);
	std::ostringstream os;
	os << render_mnemonic("lui") << render_reg(rd) << ","
		<< hex::to_hex0x20((imm_u >> 12) & 0x0fffff);
	return os.str();
}

std::string rv32i_decode::render_auipc(uint32_t insn)
{
	uint32_t rd = get_rd(insn);
	int32_t imm_u = get_imm_u(insn);
	std::ostringstream os;
	os << render_mnemonic("auipc") << render_reg(rd) << ","
		<< hex::to_hex0x20((imm_u >> 12) & 0x0fffff);
	return os.str();
}

std::string rv32i_decode::render_jal(uint32_t addr, uint32_t insn)
{
	uint32_t rd = get_rd(insn);
	int32_t imm_j = get_imm_j(insn);
	std::ostringstream os;
	os << render_mnemonic("jal") << render_reg(rd) << ","
		<< hex::to_hex0x32(imm_j + addr);
	return os.str();
}

std::string rv32i_decode::render_jalr(uint32_t insn)
{
	uint32_t rd = get_rd(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t imm_i = get_imm_i(insn);
	std::ostringstream os;
	os << render_mnemonic("jalr") << render_reg(rd) << ","
		<< imm_i << "(" << render_reg(rs1) << ")";
	return os.str();
}

std::string rv32i_decode::render_btype(uint32_t addr, uint32_t insn, const char* mnemonic)
{
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);
	int32_t imm_b = get_imm_b(insn); 
	std::ostringstream os;
	os << render_mnemonic(mnemonic) << render_reg(rs1) << ","
		<< render_reg(rs2) << "," << hex::to_hex0x32(imm_b + addr);
	return os.str();
}

std::string rv32i_decode::render_itype_load(uint32_t insn, const char* mnemonic)
{
	uint32_t rd = get_rd(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t imm_i = get_imm_i(insn);
	std::ostringstream os;
	os << render_mnemonic(mnemonic) << render_reg(rd) << ","
		<< imm_i << "(" << render_reg(rs1) << ")";
	return os.str();
}

std::string rv32i_decode::render_stype(uint32_t insn, const char* mnemonic)
{
	uint32_t rs2 = get_rs2(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_s = get_imm_s(insn);
	std::ostringstream os;
	os << render_mnemonic(mnemonic) << render_reg(rs2) << ","
		<< imm_s << "(" << render_reg(rs1) << ")";
	return os.str();
}

std::string rv32i_decode::render_itype_alu(uint32_t insn, const char* mnemonic, int32_t imm_i)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);

	if (strcmp(mnemonic, "slli") == 0 || 
			strcmp(mnemonic, "srli") == 0 || 
			strcmp(mnemonic, "srai") == 0){

		imm_i = (imm_i & 0x0000001f);
	}
	std::ostringstream os;
	os << render_mnemonic(mnemonic) << render_reg(rd) << ","
		<< render_reg(rs1) << "," << imm_i;

	return os.str();
}

std::string rv32i_decode::render_rtype(uint32_t insn, const char* mnemonic)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);


	std::ostringstream os;
	os << render_mnemonic(mnemonic) << render_reg(rd) << ","
		<< render_reg(rs1) << "," << render_reg(rs2);

	return os.str();
}

std::string rv32i_decode::render_ecall(uint32_t insn)
{
	insn = insn;
	return "ecall";
}

std::string rv32i_decode::render_ebreak(uint32_t insn)
{
	insn = insn;
	return "ebreak";
}

std::string rv32i_decode::render_csrrx(uint32_t insn, const char* mnemonic)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t csr = get_imm_i(insn);

	std::ostringstream os;
	os << render_mnemonic(mnemonic) << render_reg(rd) << ","
		<< hex::to_hex0x12(csr) << "," << render_reg(rs1);

	return os.str();
}

std::string rv32i_decode::render_csrrxi(uint32_t insn, const char* mnemonic)
{
	uint32_t rd = get_rd(insn);
	uint32_t zimm = get_rs1(insn);
	int32_t csr = get_imm_i(insn);

	std::ostringstream os;
	os << render_mnemonic(mnemonic) << render_reg(rd) << ","
		<< std::setw(5) << hex::to_hex0x12(csr) << "," << zimm;

	return os.str();

}

//last 3 helpers
std::string rv32i_decode::render_reg(int r)
{
	return "x" + std::to_string(r);
}

std::string rv32i_decode::render_mnemonic(const std::string& m)
{
	std::ostringstream os;
	os << std::left << std::setw(mnemonic_width) << m;
	return os.str();
}
