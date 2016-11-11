#pragma once

#include <string>
#include <stdint.h>

class Code
{
public:
	/*
	 * Returns the binary code of the dest mnemonic.
	 */
	static std::string dest(const std::string &mnemonic);

	/*
	 * Returns the binary code of the comp mnemonic.
	 */
	static std::string comp(const std::string &mnemonic);

	/*
	 * Returns the binary code of the jump mnemonic.
	 */
	static std::string jump(const std::string &mnemonic);

	/*
	 * Returns bits for numeric symbol, or empty string
	 * if symbol is a variable symbol.
	 */
	static std::string convertNumericSymbol(const std::string &symbol);

private:
	/*
	 * Convert string to uppercase.
	 */
	static std::string toUppercase(const std::string &word);
};

