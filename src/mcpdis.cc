#include <mcpdis.hh>

#include <list>

std::list<instruction> pic12f675 = {

	{ "00000000001000", "RETURN", 0 },
	{ "00000000001001", "RETFIE", 0 },
	{ "00000001100011", "SLEEP" , instruction::status_bit::power_bits },
	{ "00000001100100", "CLRWDT", instruction::status_bit::power_bits },
	{ "0000000xx00000", "NOP"   , 0 },
	{ "0000001fffffff", "MOVWF" , 0 },
	{ "000001dfffffff", "CLR"   , instruction::status_bit::Z },
	{ "000010dfffffff", "SUBWF" , instruction::status_bit::arithmetic_bits },
	{ "000011dfffffff", "DECF"  , instruction::status_bit::Z },
	{ "000100dfffffff", "IORWF" , instruction::status_bit::Z },
	{ "000101dfffffff", "ANDWD" , instruction::status_bit::Z },
	{ "000110dfffffff", "XORWF" , instruction::status_bit::Z },
	{ "000111dfffffff", "ADDWF" , instruction::status_bit::arithmetic_bits },
	{ "001000dfffffff", "MOVF"  , instruction::status_bit::Z },
	{ "001001dfffffff", "COMF"  , instruction::status_bit::Z },
	{ "001010dfffffff", "INCF"  , instruction::status_bit::Z },
	{ "001011dfffffff", "DECFSZ", 0 },
	{ "001100dfffffff", "RRF"   , instruction::status_bit::C },
	{ "001101dfffffff", "RLF"   , instruction::status_bit::C },
	{ "001110dfffffff", "SWAPF" , 0 },
	{ "001111dfffffff", "INCFSZ", 0 },
	{ "0100bbbfffffff", "BCF"   , 0 },
	{ "0101bbbfffffff", "BSF"   , 0 },
	{ "0110bbbfffffff", "BTFSC" , 0 },
	{ "0111bbbfffffff", "BTFSS" , 0 },
	{ "100kkkkkkkkkkk", "CALL"  , 0 },
	{ "101kkkkkkkkkkk", "GOTO"  , 0 },
	{ "1100xxkkkkkkkk", "MOVLW" , 0 },
	{ "1101xxkkkkkkkk", "RETLW" , 0 },
	{ "111000kkkkkkkk", "IORLW" , 0 },
	{ "111001kkkkkkkk", "ANDLW" , 0 },
	{ "111010kkkkkkkk", "XORLW" , instruction::status_bit::Z },
	{ "11110xkkkkkkkk", "SUBLW" , instruction::status_bit::arithmetic_bits },
	{ "11111xkkkkkkkk", "ADDLW" , instruction::status_bit::arithmetic_bits }

};
