#pragma once

#include <stdint.h>
#include <string>
#include <map>

/*
 * Holds identifiers defined in a program.
 */
class SymbolTable
{
public:
	SymbolTable();

	/*
	 * Starts a new subroutine scope (i.e. resets the subroutine's
	 * symbol table).
	 */
	void startSubroutine();

	/*
	 * Defines a new identifer of a given name, type and kind and assigns
	 * it a running index. STATIC and FIELD identifiers have a class scope,
	 * while ARG and VAR identifiers have a subroutine scope.
	 */
	void define(const std::string &name,
		const std::string &type,
		int32_t kind);

	/*
	 * Returns the number of variables of the given kind already defined
	 * in the current scope.
	 */
	int32_t varCount(int32_t kind);

	/*
	 * Returns the kind of the named identifier in the current scope.
	 * If the identifier is unknown in the current scope, returns NONE.
	 */
	int32_t kindOf(const std::string &name);

	/*
	 * Returns the type of the named identifier in the current scope.
	 */
	std::string typeOf(const std::string &name);

	/*
	 * Returns the index assigned to the named identifier.
	 */
	int32_t indexOf(const std::string &name);

private:
	struct Identifier
	{
		std::string m_name;
		std::string m_type;
		int32_t m_kind;
		int32_t m_index;
	};

	/*
	 * Identifiers with class scope (STATIC and FIELD types).
	 */
	std::map<std::string, struct Identifier> m_staticTable;
	std::map<std::string, struct Identifier> m_fieldTable;

	/*
	 * Identifiers with subroutine scope (ARG and VAR types).
	 */
	std::map<std::string, struct Identifier> m_argTable;
	std::map<std::string, struct Identifier> m_varTable;
};
