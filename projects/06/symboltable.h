#pragma once

#include <string>
#include <map>

class SymbolTable
{
public:
	SymbolTable();

	/*
	 * Adds the pair ( symbol , address ) to the table.
	 */
	void addEntry(const std::string &symbol, int address);

	/*
	 * Does the symbol table contain the given symbol?
	 */
	bool contains(const std::string &symbol) const;

	/*
	 * Returns the address associated with the symbol.
	 */
	int getAddress(const std::string &symbol) const;

private:
	std::map<std::string, int> m_entries;
};

