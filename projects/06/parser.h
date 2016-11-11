#pragma once

#include <string>
#include <fstream>

class Parser
{
public:
	enum COMMAND_TYPE
	{
		A_COMMAND,	// for @Xxx where Xxx is either a symbol or a decimal number
		C_COMMAND,	// for dest=comp;jump
		L_COMMAND	// (actually, pseudo-command) for (Xxx) where Xxx is a symbol
	};

	Parser(const std::string &filename);

	~Parser();

	/*
	 * Opens the input file/stream and gets ready to parse it.
	 */
	bool open();

	/*
	 * Are there more commands in the input?
	 */
	bool hasMoreCommands();

	/*
	 * Reads the next command from the input and makes it the current
	 * command. Should be called only if hasMoreCommands() is true.
	 * Initially there is no current command.
	 */
	void advance();

	/*
	 * Returns the type of the current command.
	 */
	enum Parser::COMMAND_TYPE commandType();

	/*
	 * Returns the symbol or decimal Xxx of the current command
	 * @Xxx or (Xxx) . Should be called only when commandType() is
	 * A_COMMAND or L_COMMAND.
	 */
	std::string symbol();

	/*
	 * Returns the dest mnemonic in the current C-command (8 possibilities).
	 * Should be called only when commandType() is C_COMMAND.
	 */
	std::string dest();

	/*
	 * Returns the comp mnemonic in the current C-command (28 pos sibilities).
	 * Should be called only when commandType() is C_COMMAND.
	 */
	std::string comp();

	/*
	 * Returns the jump mnemonic in the current C-command (8 possibilities).
	 * Should be called only when commandType() is C_COMMAND.
	 */
	std::string jump();

private:
	std::string m_filename;
	std::ifstream m_fs;
	COMMAND_TYPE m_currentCommandType;
	std::string m_currentSymbol;
	std::string m_currentDest;
	std::string m_currentComp;
	std::string m_currentJump;
};

