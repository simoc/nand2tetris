#pragma once

#include <iostream>
#include <fstream>

#include "parser.h"

class CodeWriter
{
public:
	CodeWriter();

	/*
	 * Opens the ouput ASM file/stream and gets ready to write into it.
	 */
	void setAsmFilename(const std::string &filename);

	/*
	 * Was ASM file opened successfully for writing?
	 */
	bool open();

	/*
	 * Informs the code writer that the translation of a new VM file
	 * is started.
	 */
	void setVmFilename(const std::string &filename);

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
	std::string m_asmFilename;
	std::ofstream m_asmFs;

	void writeLogic(const std::string &command);
	void writeUnary(const std::string &command);
	void writeOperation(const std::string &command);
};
