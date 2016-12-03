#pragma once

#include <iostream>
#include <fstream>

#include "parser.h"

class CodeWriter
{
public:
	/*
	 * Opens the input file/stream and gets ready to write into it.
	 */
	CodeWriter();

	/*
	 * Informs the code writer that the translation of a new VM file
	 * is started.
	 */
	void setFilename(const std::string &filename);

	/*
	 * Was file opened successfully?
	 */
	bool open();

	/*
	 * Writes the assembly code that is the translation of the given
	 * arithmetic command.
	 */
	void writeArithmetic(const std::string &command);

	/*
	 * Writes the assembly code tht is the translation of the given
	 * command, where command is either C_PUSH or C_POP.
	 */
	void writePushPop(Parser::COMMAND_TYPE command,
		const std::string &segment, int index);

	/*
	 * Closes the output file.
	 */
	void close();

private:
	int m_branchCounter;
	std::string m_vmFilename;
	std::ofstream m_vmFs;

	void writeLogic(const std::string &command);
	void writeUnary(const std::string &command);
	void writeOperation(const std::string &command);
};
