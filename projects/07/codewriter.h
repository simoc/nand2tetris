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
	 * Writes assembly code that effects the VM initialization, also
	 * called bootstrap code. This code must also be placed at the
	 * beginning of the output file.
	 */
	void writeInit();

	/*
	 * Writes assembly code that effects the label command.
	 */
	void writeLabel(const std::string &label);

	/*
	 * Writes assembly code that effects the goto command.
	 */
	void writeGoto(const std::string &label);

	/*
	 * Writes assembly code that effects the if-goto command.
	 */
	void writeIf(const std::string &label);

	/*
	 * Writes assembly code that effects the call command.
	 */
	void writeCall(const std::string &functionName, int numArgs);

	/*
	 * Writes assembly code that effects the return command.
	 */
	void writeReturn();

	/*
	 * Writes assembly code that effects the function command.
	 */
	void writeFunction(const std::string &functionName, int numLocals);

	/*
	 * Closes the output file.
	 */
	void close();

private:
	int m_branchCounter;
	int m_returnAddressCounter;
	std::string m_vmFilename;
	std::string m_asmFilename;
	std::string m_functionName;
	std::ofstream m_asmFs;

	void writeLogic(const std::string &command);
	void writeUnary(const std::string &command);
	void writeOperation(const std::string &command);
	void pushValue(int value);
	void pushValueAtAddress(const std::string &address);
};
