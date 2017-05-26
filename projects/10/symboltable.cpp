#include "symboltable.h"
#include "jacktokenizer.h"

SymbolTable::SymbolTable()
{
}

void
SymbolTable::startSubroutine()
{
	m_subroutineTable.clear();
}

void
SymbolTable::define(const std::string &name,
	const std::string &type,
	int32_t kind)
{
	struct Identifier identifier;
	identifier.m_name = name;
	identifier.m_type = type;
	identifier.m_kind = kind;

	if (kind == JackTokenizer::K_ARG || kind == JackTokenizer::K_VAR)
	{
		identifier.m_index = m_subroutineTable.size();
		m_subroutineTable.insert(std::pair<std::string, struct Identifier>(name, identifier));
	}
	else if (kind == JackTokenizer::K_STATIC || kind == JackTokenizer::K_FIELD)
	{
		identifier.m_index = m_classTable.size();
		m_classTable.insert(std::pair<std::string, struct Identifier>(name, identifier));
	}
}

int32_t
SymbolTable::varCount(int32_t kind)
{
	int32_t count = 0;

	if (kind == JackTokenizer::K_ARG || kind == JackTokenizer::K_VAR)
	{
		auto it = m_subroutineTable.begin();
		while (it != m_subroutineTable.end())
		{
			if (it->second.m_kind == kind)
				count++;
			it++;
		}
	}
	else if (kind == JackTokenizer::K_STATIC || kind == JackTokenizer::K_FIELD)
	{
		auto it = m_classTable.begin();
		while (it != m_classTable.end())
		{
			if (it->second.m_kind == kind)
				count++;
			it++;
		}
	}
	return count;
}

int32_t
SymbolTable::kindOf(const std::string &name)
{
	auto it = m_subroutineTable.find(name);
	if (it == m_subroutineTable.end())
	{
		it = m_classTable.find(name);
		if (it == m_classTable.end())
			return -1;
	}
	return it->second.m_kind;
}

std::string
SymbolTable::typeOf(const std::string &name)
{
	auto it = m_subroutineTable.find(name);
	if (it == m_subroutineTable.end())
	{
		it = m_classTable.find(name);
		if (it == m_classTable.end())
			return "unknown";
	}
	return it->second.m_type;
}

int32_t
SymbolTable::indexOf(const std::string &name)
{
	auto it = m_subroutineTable.find(name);
	if (it == m_subroutineTable.end())
	{
		it = m_classTable.find(name);
		if (it == m_classTable.end())
			return -1;
	}
	return it->second.m_index;
}
