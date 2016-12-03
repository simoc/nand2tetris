#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

class Parser
{
public:
	enum COMMAND_TYPE
	{
		C_UNKNOWN,
		C_ARITHMETIC,
		C_PUSH,
		C_POP,
		C_LABEL,
		C_GOTO,
		C_IF,
		C_FUNCTION,
		C_RETURN,
		C_CALL
	};

	static const std::string SEGMENT_CONSTANT;

	/*
	 * Opens the input file/stream and gets ready to parse it.
	 */
	Parser(const std::string &filename);

	/*
	 * Frees a Parser created with createParser().
	 */
	~Parser();

	/*
	 * Was file opened successfully?
	 */
	bool open();

	/*
	 * Are there any more commands in the input?
	 */
	bool hasMoreCommands();

	/*
	 * Reads the next command from the input and makes it the current command.
	 * Should be called only if hasMoreCommands() is true.  Initially there is
	 * no current command.
	 */
	void advance();

	/*
	 * Returns the type of the current VM command. C_ARITHMETIC is returned
	 * for all the arithmetic commands.
	 */
	enum COMMAND_TYPE commandType();

	/*
	 * Returns the first argument of the current command.  In the case of
	 * C_ARITHMETIC, the command itself (add, sub, etc.) is returned.
	 * Should not be called if the current command is C_RETURN.
	 */
	std::string arg1();

	/*
	 * Returns the second argument of the current command.  Should be called
	 * only if the current command is C_PUSH, C_POP, C_FUNCTION, or C_CALL.
	 */
	std::string arg2();

private:
	std::string toUppercase(const std::string &word);

	std::string m_filename;
	std::ifstream m_fs;

	std::vector<std::string> m_tokens;
};

