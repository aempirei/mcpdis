#include <sstream>
#include <iomanip>

#include <cstdlib>
#include <cstdint>
#include <climits>

#include <mcpdis.hh>

#define OARG(X)		o.args.value(X)

#define FN(a)		void a(operation&, dictionary&)
#define GN(a)		void a(operation&o, dictionary&c)
#define HN(a)		void a(operation&, dictionary&c)

#define USE_B		reg_t b = (1 << OARG(L'b'))
#define USE_W		const symbol W = L"W"
#define USE_D		const symbol d = load_d(o)
#define USE_F		const symbol f = load_f(o)
#define USE_K		const literal_t k = OARG(L'k')
#define USE_PC		const literal_t pc = o.address
#define USE_REG(R)	const symbol R = register_name(instruction::file_register::R)
#define USE_STACK	const symbol STACK = L"STACK"

#define CLEAR_BIT(reg, mask) do{ USE_REG(reg); c.touch(reg); c[reg] = function(OP_AND, { (reg_t)~(reg_t)(mask), c[reg] }); }while(0)
#define SET_BIT(reg, mask)   do{ USE_REG(reg); c.touch(reg); c[reg] = function(OP_OR , {         (reg_t)(mask), c[reg] }); }while(0)

#define SET_PC(x) do{ USE_REG(PCL); USE_REG(PCLATH); c[PCL] = (reg_t)(x); c[PCLATH] = (reg_t)((x) >> 8); }while(0)

namespace pic12f {


	symbol load_d(operation& o) {
		return dest_string(OARG(L'd'), OARG(L'f'));
	}

	symbol load_f(operation& o) {
		return register_name(OARG(L'f'));
	}

	HN(RETURN) {

		USE_REG(PCL);
		USE_REG(PCLATH);

		c[PCL] = term(L"TOS.lo");
		c[PCLATH] = term(L"TOS.hi");
	}

	GN(RETFIE) { RETURN(o,c);  SET_BIT(INTCON, instruction::flags::GIE); }

	FN(SLEEP)  { /* put microcontroller to sleep */ }
	FN(CLRWDT) { /* clear watchdog timer */         } 
	FN(NOP)    { /* no operation */                 }

	GN(MOVWF)  {

		USE_W;
		USE_F; 

		c[f] = c.touch(W);  
	}


	GN(CLR)    {

		USE_D;

		c[d] = 0;

		USE_REG(STATUS);

		term& reg = c.touch(STATUS);

		reg = function(OP_AND, { (reg_t)~instruction::flags::Z, reg });
	}

	GN(MOVF)   {

		USE_D;
		USE_F;

		c[d] = c.touch(f);

		USE_REG(STATUS);

		term& reg = c.touch(STATUS);

		reg = function(OP_AND, { (reg_t)~instruction::flags::Z, reg });
		reg = function(OP_OR , { (reg_t) instruction::flags::Z, reg });
		// reg = term(function(OP_OR, { term(L"Z"), reg }));

	}

	GN(IORWF)  { USE_W; USE_D; USE_F; c[d] = function(OP_OR   , { c.touch(f) , c.touch(W) }); }
	GN(ANDWF)  { USE_W; USE_D; USE_F; c[d] = function(OP_AND  , { c.touch(f) , c.touch(W) }); }
	GN(XORWF)  { USE_W; USE_D; USE_F; c[d] = function(OP_XOR  , { c.touch(f) , c.touch(W) }); }
	GN(SUBWF)  { USE_W; USE_D; USE_F; c[d] = function(OP_MINUS, { c.touch(f) , c.touch(W) }); }
	GN(ADDWF)  { USE_W; USE_D; USE_F; c[d] = function(OP_PLUS , { c.touch(f) , c.touch(W) }); }

	GN(DECF)   { USE_D; USE_F;         c[d] = function(OP_MINUS, { c.touch(f), 1          }); }
	GN(INCF)   { USE_D; USE_F;         c[d] = function(OP_PLUS , { c.touch(f), 1          }); }

	GN(COMF)   { USE_D; USE_F;         c[d] = function(OP_NOT  , { c.touch(f)             }); }
	GN(RRF)    { USE_D; USE_F;         c[d] = function(OP_ROTR , { c.touch(f)             }); }
	GN(RLF)    { USE_D; USE_F;         c[d] = function(OP_ROTL , { c.touch(f)             }); }
	GN(SWAPF)  { USE_D; USE_F;         c[d] = function(OP_SWAP , { c.touch(f)             }); }

	GN(BCF)    { USE_F; USE_B; b = ~b; c[f] = function(OP_AND  , { b         , c.touch(f) }); }
	GN(BSF)    { USE_F; USE_B;         c[f] = function(OP_OR   , { b         , c.touch(f) }); } 

	FN(DECFSZ) { throw std::runtime_error(std::string("DECFSZ performs conditional program counter modification")); }
	FN(INCFSZ) { throw std::runtime_error(std::string("INCFSZ performs conditional program counter modification")); }
	FN(BTFSC)  { throw std::runtime_error(std::string("BTFSC performs conditional program counter modification" )); }
	FN(BTFSS)  { throw std::runtime_error(std::string("BTFSS performs conditional program counter modification" )); }

	GN(GOTO)   { USE_K; SET_PC(k); }

	GN(CALL)   { GOTO(o, c); USE_PC; USE_STACK; c[STACK] = function(OP_LIST, { pc + 1, c.touch(STACK) }); }

	GN(MOVLW)  { USE_K; USE_W; c[W] = (reg_t)k; }

	GN(RETLW)  { MOVLW(o, c); RETURN(o, c); }

	GN(IORLW)  { USE_K; USE_W; c[W] = function(OP_OR   , { k, c.touch(W) }); }
	GN(ANDLW)  { USE_K; USE_W; c[W] = function(OP_AND  , { k, c.touch(W) }); }
	GN(XORLW)  { USE_K; USE_W; c[W] = function(OP_XOR  , { k, c.touch(W) }); }
	GN(SUBLW)  { USE_K; USE_W; c[W] = function(OP_MINUS, { k, c.touch(W) }); }
	GN(ADDLW)  { USE_K; USE_W; c[W] = function(OP_PLUS , { k, c.touch(W) }); }

	GN(PC)     {
		USE_PC;
		SET_PC(pc);
	}

	void finalize(dictionary&) {
		/* update any registers and anything that happens not under direct control of program flow, such as GPIO. */
	}

	void power(dictionary& c) {

		/* fix these for actual power on values */
		
		USE_REG(PCL)   ; c[PCL]    = 0;
		USE_REG(STATUS); c[STATUS] = 0;
		USE_REG(PCLATH); c[PCLATH] = 0;
		USE_REG(INTCON); c[INTCON] = 0;
		USE_REG(PIR1)  ; c[PIR1]   = 0;
		USE_REG(T1CON) ; c[T1CON]  = 0;
		USE_REG(CMCON) ; c[CMCON]  = 0;
		USE_REG(ADCON0); c[ADCON0] = 0;
	}

	instruction_set pic12f675 = {

		{ L"00000000001000", L"RETURN", RETURN, instruction::pcl_types::ret   , instruction::flags::none       },
		{ L"00000000001001", L"RETFIE", RETFIE, instruction::pcl_types::ret   , instruction::flags::none       },
		{ L"0000000xx00000", L"NOP"   , NOP   , instruction::pcl_types::normal, instruction::flags::none       },
		{ L"00000001100011", L"SLEEP" , SLEEP , instruction::pcl_types::normal, instruction::flags::power      },
		{ L"00000001100100", L"CLRWDT", CLRWDT, instruction::pcl_types::normal, instruction::flags::power      },
		{ L"0000001fffffff", L"MOVWF" , MOVWF , instruction::pcl_types::normal, instruction::flags::none       },
		{ L"000001dfffffff", L"CLR"   , CLR   , instruction::pcl_types::normal, instruction::flags::Z          },
		{ L"000100dfffffff", L"IORWF" , IORWF , instruction::pcl_types::normal, instruction::flags::Z          },
		{ L"000101dfffffff", L"ANDWF" , ANDWF , instruction::pcl_types::normal, instruction::flags::Z          },
		{ L"000110dfffffff", L"XORWF" , XORWF , instruction::pcl_types::normal, instruction::flags::Z          },
		{ L"000010dfffffff", L"SUBWF" , SUBWF , instruction::pcl_types::normal, instruction::flags::arithmetic },
		{ L"000111dfffffff", L"ADDWF" , ADDWF , instruction::pcl_types::normal, instruction::flags::arithmetic },
		{ L"001000dfffffff", L"MOVF"  , MOVF  , instruction::pcl_types::normal, instruction::flags::Z          },
		{ L"001001dfffffff", L"COMF"  , COMF  , instruction::pcl_types::normal, instruction::flags::Z          },
		{ L"000011dfffffff", L"DECF"  , DECF  , instruction::pcl_types::normal, instruction::flags::Z          },
		{ L"001010dfffffff", L"INCF"  , INCF  , instruction::pcl_types::normal, instruction::flags::Z          },
		{ L"001100dfffffff", L"RRF"   , RRF   , instruction::pcl_types::normal, instruction::flags::C          },
		{ L"001101dfffffff", L"RLF"   , RLF   , instruction::pcl_types::normal, instruction::flags::C          },
		{ L"001110dfffffff", L"SWAPF" , SWAPF , instruction::pcl_types::normal, instruction::flags::none       },
		{ L"0100bbbfffffff", L"BCF"   , BCF   , instruction::pcl_types::normal, instruction::flags::none       },
		{ L"0101bbbfffffff", L"BSF"   , BSF   , instruction::pcl_types::normal, instruction::flags::none       },
		{ L"001011dfffffff", L"DECFSZ", DECFSZ, instruction::pcl_types::skip  , instruction::flags::none       },
		{ L"001111dfffffff", L"INCFSZ", INCFSZ, instruction::pcl_types::skip  , instruction::flags::none       },
		{ L"0110bbbfffffff", L"BTFSC" , BTFSC , instruction::pcl_types::skip  , instruction::flags::none       },
		{ L"0111bbbfffffff", L"BTFSS" , BTFSS , instruction::pcl_types::skip  , instruction::flags::none       },
		{ L"100kkkkkkkkkkk", L"CALL"  , CALL  , instruction::pcl_types::call  , instruction::flags::none       },
		{ L"101kkkkkkkkkkk", L"GOTO"  , GOTO  , instruction::pcl_types::jump  , instruction::flags::none       },
		{ L"1101xxkkkkkkkk", L"RETLW" , RETLW , instruction::pcl_types::ret   , instruction::flags::none       },
		{ L"1100xxkkkkkkkk", L"MOVLW" , MOVLW , instruction::pcl_types::normal, instruction::flags::none       },
		{ L"111000kkkkkkkk", L"IORLW" , IORLW , instruction::pcl_types::normal, instruction::flags::Z          },
		{ L"111001kkkkkkkk", L"ANDLW" , ANDLW , instruction::pcl_types::normal, instruction::flags::Z          },
		{ L"111010kkkkkkkk", L"XORLW" , XORLW , instruction::pcl_types::normal, instruction::flags::Z          },
		{ L"11110xkkkkkkkk", L"SUBLW" , SUBLW , instruction::pcl_types::normal, instruction::flags::arithmetic },
		{ L"11111xkkkkkkkk", L"ADDLW" , ADDLW , instruction::pcl_types::normal, instruction::flags::arithmetic }
	};


	symbol address_string(literal_t x) {
		std::wstringstream ts;
		ts << std::uppercase << std::right << std::hex << std::setw(3) << std::setfill(L'0') << x << L'h';
		return ts.str();
	}

	symbol register_string(reg_t x) {
		std::wstringstream ts;
		ts << L'r' << std::uppercase << std::right << std::hex << std::setw(2) << std::setfill(L'0') << x;
		return ts.str();
	}

	symbol dest_string(bool f, reg_t x) {
		return f ? register_name(x) : L"W";
	}

	symbol register_name(reg_t x) {

		switch(x) {
			case 0x00: return L"INDF";
			case 0x01: return L"TMR0";
			case 0x02: return L"PCL";
			case 0x03: return L"STATUS";
			case 0x04: return L"FSR";
			case 0x05: return L"GPIO";
			case 0x0a: return L"PCLATH";
			case 0x0b: return L"INTCON";
			case 0x0c: return L"PIR1";
			case 0x0e: return L"TMR1L";
			case 0x0f: return L"TMR1H";
			case 0x10: return L"T1CON";
			case 0x19: return L"CMCON";
			case 0x1e: return L"ADRESH";
			case 0x1f: return L"ADCON0";
		}
		return register_string(x);
	}
}

#undef USE_B
#undef USE_W
#undef USE_D
#undef USE_F
#undef USE_K
#undef USE_PC
#undef USE_REG
#undef USE_STACK

#undef FN
#undef GN
#undef HN
