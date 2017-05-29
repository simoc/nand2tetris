#include "symboltable.h"
#include "jacktokenizer.h"

SymbolTable::SymbolTable()
{
}

void
SymbolTable::startSubroutine()
{
	m_argTable.clear();
	m_varTable.clear();
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

	if (kind == JackTokenizer::K_ARG)
	{
		identifier.m_index = m_argTable.size();
		m_argTable.insert(std::pair<std::string, struct Identifier>(name, identifier));
	}
	else if (kind == JackTokenizer::K_VAR)
	{
		identifier.m_index = m_varTable.size();
		m_varTable.insert(std::pair<std::string, struct Identifier>(name, identifier));
	}
	else if (kind == JackTokenizer::K_STATIC)
	{
		identifier.m_index = m_staticTable.size();
		m_staticTable.insert(std::pair<std::string, struct Identifier>(name, identifier));
	}
	else if (kind == JackTokenizer::K_FIELD)
	{
		identifier.m_index = m_fieldTable.size();
		m_fieldTable.insert(std::pair<std::string, struct Identifier>(name, identifier));
	}
}

int32_t
SymbolTable::varCount(int32_t kind)
{
	int32_t count = 0;

	if (kind == JackTokenizer::K_ARG)
	{
		count = m_argTable.size();
	}
	else if (kind == JackTokenizer::K_VAR)
	{
		count = m_varTable.size();
	}
	else if (kind == JackTokenizer::K_STATIC)
	{
		count = m_staticTable.size();
	}
	else if (kind == JackTokenizer::K_FIELD)
	{
		count = m_fieldTable.size();
	}
	return count;
}

int32_t
SymbolTable::kindOf(const std::string &name)
{
	auto it = m_argTable.find(name);
	if (it == m_argTable.end())
	{
		it = m_varTable.find(name);
		if (it == m_varTable.end())
		{
			it = m_staticTable.find(name);
			if (it == m_staticTable.end())
			{
				it = m_fieldTable.find(name);
				if (it == m_fieldTable.end())
					return -1;
			}
		}
	}
	return it->second.m_kind;
}

std::string
SymbolTable::typeOf(const std::string &name)
{
	auto it = m_argTable.find(name);
	if (it == m_argTable.end())
	{
		it = m_varTable.find(name);
		if (it == m_varTable.end())
		{
			it = m_staticTable.find(name);
			if (it == m_staticTable.end())
			{
				it = m_fieldTable.find(name);
				if (it == m_fieldTable.end())
					return "unknown";
			}
		}
	}
	return it->second.m_type;
}

int32_t
SymbolTable::indexOf(const std::string &name)
{
	auto it = m_argTable.find(name);
	if (it == m_argTable.end())
	{
		it = m_varTable.find(name);
		if (it == m_varTable.end())
		{
			it = m_staticTable.find(name);
			if (it == m_staticTable.end())
			{
				it = m_fieldTable.find(name);
				if (it == m_fieldTable.end())
					return -1;
			}
		}
	}
	return it->second.m_index;
}
