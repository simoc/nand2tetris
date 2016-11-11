#include <ctype.h>
#include <iterator>
#include <bitset>
#include <sstream>
#include <stdexcept>

#include "code.h"

std::string
Code::dest(const std::string &mnemonic)
{
	std::string bits;
	std::string s = toUppercase(mnemonic);

	if (s == "M")
		bits = "001";
	else if (s == "D")
		bits = "010";
	else if (s == "MD")
		bits = "011";
	else if (s == "A")
		bits = "100";
	else if (s == "AM")
		bits = "101";
	else if (s == "AD")
		bits = "110";
	else if (s == "AMD")
		bits = "111";
	else
		bits = "000";

	return bits;
}

std::string
Code::comp(const std::string &mnemonic)
{
	std::string bits;
	std::string s = toUppercase(mnemonic);

	if (s == "0")
		bits = "0101010";
	else if (s == "1")
		bits = "0111111";
	else if (s == "-1")
		bits = "0111010";
	else if (s == "D")
		bits = "0001100";
	else if (s == "A")
		bits = "0110000";
	else if (s == "!D")
		bits = "0001101";
	else if (s == "!A")
		bits = "0110001";
	else if (s == "-D")
		bits = "0001111";
	else if (s == "-A")
		bits = "0110011";
	else if (s == "D+1")
		bits = "0011111";
	else if (s == "A+1")
		bits = "0110111";
	else if (s == "D-1")
		bits = "0001110";
	else if (s == "A-1")
		bits = "0110010";
	else if (s == "D+A")
		bits = "0000010";
	else if (s == "D-A")
		bits = "0010011";
	else if (s == "A-D")
		bits = "0000111";
	else if (s == "D&A")
		bits = "0000000";
	else if (s == "D|A")
		bits = "0010101";
	else if (s == "M")
		bits = "1110000";
	else if (s == "!M")
		bits = "1110001";
	else if (s == "-M")
		bits = "1110011";
	else if (s == "M+1")
		bits = "1110111";
	else if (s == "M-1")
		bits = "1110010";
	else if (s == "D+M")
		bits = "1000010";
	else if (s == "D-M")
		bits = "1010011";
	else if (s == "M-D")
		bits = "1000111";
	else if (s == "D&M")
		bits = "1000000";
	else if (s == "D|M")
		bits = "1010101";

	return bits;
}

std::string
Code::jump(const std::string &mnemonic)
{
	std::string bits;
	std::string s = toUppercase(mnemonic);

	if (s == "JGT")
		bits = "001";
	else if (s == "JEQ")
		bits = "010";
	else if (s == "JGE")
		bits = "011";
	else if (s == "JLT")
		bits = "100";
	else if (s == "JNE")
		bits = "101";
	else if (s == "JLE")
		bits = "110";
	else if (s == "JMP")
		bits = "111";
	else
		bits = "000";

	return bits;
}

std::string
Code::convertNumericSymbol(const std::string &symbol)
{
	std::ostringstream os;
	size_t index;
	try
	{
		unsigned long converted = std::stoul(symbol, &index);
		if (index == symbol.length())
		{
			std::bitset<15> bits(converted);
			os << bits;
		}
	}
	catch (const std::invalid_argument &e)
	{
	}
	return os.str();
}

std::string
Code::toUppercase(const std::string &word)
{
	std::string s;

	std::string::const_iterator it = word.begin();
	while (it != word.end())
	{
		s.push_back(toupper(*it));
		it++;
	}
	return s;
}
