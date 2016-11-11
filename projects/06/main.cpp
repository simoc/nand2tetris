#include <iostream>
#include <sstream>
#include <bitset>
#include <fstream>

#include "parser.h"
#include "code.h"
#include "symboltable.h"

static bool
firstPass(const std::string &asmFilename, SymbolTable &symbolTable)
{
	bool retval;

	Parser p(asmFilename);

	int romAddress = 0;
	if (p.open())
	{
		while (p.hasMoreCommands())
		{
			p.advance();
			switch (p.commandType())
			{
			case Parser::C_COMMAND:
				if (!p.dest().empty())
					std::cout << p.dest() << "=";
				std::cout << p.comp();
				if (!p.jump().empty())
					std::cout << ";" << p.jump();
				std::cout << std::endl;
				romAddress++;
				break;
			case Parser::A_COMMAND:
				std::cout << "@" << p.symbol() << std::endl;
				romAddress++;
				break;
			case Parser::L_COMMAND:
				std::cout << "(" << p.symbol() << ")" << std::endl;
				symbolTable.addEntry(p.symbol(), romAddress);
				break;
			}
		}
		retval = true;
	}
	else
	{
		std::cerr << "Failed opening file: " << asmFilename << std::endl;
		retval = false;
	}
	return retval;
}

static bool
secondPass(const std::string &asmFilename, const std::string &hackFilename,
	SymbolTable &symbolTable)
{
	bool retval;
	int nextVariableAddress = 16;

	std::ofstream ofs;
	ofs.open(hackFilename.c_str());
	if (!ofs.is_open())
	{
		std::cerr << "Failed opening output file: " << hackFilename << std::endl;
		return false;
	}

	Code c;
	Parser p(asmFilename);
	if (p.open())
	{
		while (p.hasMoreCommands())
		{
			p.advance();
			if (p.commandType() == Parser::C_COMMAND)
			{
				ofs << "111" << c.comp(p.comp()) << c.dest(p.dest()) <<
					 c.jump(p.jump()) << std::endl;
			}
			else if (p.commandType() == Parser::A_COMMAND)
			{
				std::string bits = c.convertNumericSymbol(p.symbol());
				if (bits.empty())
				{
					/*
					 * Lookup address of non-numeric symbols in symbol table.
					 */
					int romAddress = symbolTable.getAddress(p.symbol());
					if (romAddress >= 0)
					{
						std::ostringstream os;
						std::bitset<15> symbolBits(romAddress);
						os << symbolBits;
						bits = os.str();
					}
					else
					{
						/*
						 * Add new variable to symbol table.
						 */
						symbolTable.addEntry(p.symbol(), nextVariableAddress);
						std::ostringstream os;
						std::bitset<15> symbolBits(nextVariableAddress);
						os << symbolBits;
						bits = os.str();

						nextVariableAddress++;
					}
				}
				ofs << "0" << bits << std::endl;
			}
		}
		retval = true;
	}
	else
	{
		std::cerr << "Failed opening file: " << asmFilename << std::endl;
		retval = false;
	}

	ofs.close();

	return retval;
}

int
main(int argc, char *argv[])
{
	std::string asmFilename;
	std::string hackFilename;
	size_t lastPos;
	SymbolTable symbolTable;

	int status = 1;

	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " [-e] filename.asm" << std::endl;
		std::cerr << "Two pass assembler for Hack assembly language." << std::endl;
		std::cerr << "Generated machine code is written to filename.hack." << std::endl;
		std::cerr << "Parsed assembly language commands are echoed to stdout." << std::endl;
		goto CLEANUP;
	}

	asmFilename = argv[1];
	hackFilename = argv[1];

	/*
	 * Replace file suffix.
	 */
	lastPos = hackFilename.find_last_of('.');
	if (lastPos != std::string::npos)
		hackFilename = hackFilename.substr(0, lastPos);
	hackFilename.append(".hack");

	/*
	 * First pass calculates addresses for symbols and echoes
	 * parsed assembly language commands.
	 */
	if (!firstPass(asmFilename, symbolTable))
		goto CLEANUP;

	/*
	 * Second pass generates assembly output.
	 */
	if (!secondPass(asmFilename, hackFilename, symbolTable))
		goto CLEANUP;

	status = 0;
CLEANUP:
	return status;
}
