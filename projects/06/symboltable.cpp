#include "symboltable.h"

#include <string>
#include <sstream>

SymbolTable::SymbolTable()
{
	/*
	 * Add pre-defined symbols.
	 */
	m_entries["SP"] = 0;
	m_entries["LCL"] = 1;
	m_entries["ARG"] = 2;
	m_entries["THIS"] = 3;
	m_entries["THAT"] = 4;
	for (int i = 0; i <= 15; i++)
	{
		std::ostringstream os;
		os << "R" << i;
		m_entries[os.str()] = i;
	}
	m_entries["SCREEN"] = 16384;
	m_entries["KBD"] = 24576;
}

void
SymbolTable::addEntry(const std::string &symbol, int address)
{
	m_entries[symbol] = address;
}

bool
SymbolTable::contains(const std::string &symbol) const
{
	return m_entries.find(symbol) != m_entries.end();
}

int
SymbolTable::getAddress(const std::string &symbol) const
{
	std::map<std::string, int>::const_iterator it = m_entries.find(symbol);
	if (it != m_entries.end())
		return it->second;
	return -1;
}
