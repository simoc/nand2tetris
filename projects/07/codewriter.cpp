#include "codewriter.h"

CodeWriter::CodeWriter()
{
	m_branchCounter = 0;
}

void
CodeWriter::setFilename(const std::string &filename)
{
	m_vmFilename = filename;
}

bool
CodeWriter::open()
{
	m_vmFs.open(m_vmFilename.c_str());
	return m_vmFs.is_open();
}

void
CodeWriter::writeUnary(const std::string &command)
{
	/*
	 * Load memory at SP[-1].
	 */
	m_vmFs << "@SP" << std::endl;
	m_vmFs << "A=M-1" << std::endl;
	m_vmFs << "D=M" << std::endl;

	/*
	 * Replace value at top of stack with result of
	 * unary operation.
	 */
	if (command == "NEG")
	{
		m_vmFs << "M=-D" << std::endl;
	}
	else if (command == "NOT")
	{
		m_vmFs << "M=!D" << std::endl;
	}
}

void
CodeWriter::writeOperation(const std::string &command)
{
	/*
	 * Load memory at SP[-2].
	 */
	m_vmFs << "@SP" << std::endl;
	m_vmFs << "A=M-1" << std::endl;
	m_vmFs << "A=A-1" << std::endl;
	m_vmFs << "D=M" << std::endl;

	/*
	 * Do operation with memory at SP[-1].
	 */
	m_vmFs << "@SP" << std::endl;
	m_vmFs << "A=M-1" << std::endl;

	if (command == "ADD")
	{
		m_vmFs << "D=D+M" << std::endl;
	}
	else if (command == "SUB")
	{
		m_vmFs << "D=D-M" << std::endl;
	}
	else if (command == "AND")
	{
		m_vmFs << "D=D&M" << std::endl;
	}
	else if (command == "OR")
	{
		m_vmFs << "D=D|M" << std::endl;
	}

	/*
	 * Save result back at SP[-2].
	 */
	m_vmFs << "@SP" << std::endl;
	m_vmFs << "A=M-1" << std::endl;
	m_vmFs << "A=A-1" << std::endl;
	m_vmFs << "M=D" << std::endl;

	/*
	 * One less element on stack so decrement SP.
	 */
	m_vmFs << "@SP" << std::endl;
	m_vmFs << "D=M-1" << std::endl;
	m_vmFs << "M=D" << std::endl;
}

void
CodeWriter::writeLogic(const std::string &command)
{
	/*
	 * Load memory at SP[-2].
	 */
	m_vmFs << "@SP" << std::endl;
	m_vmFs << "A=M-1" << std::endl;
	m_vmFs << "A=A-1" << std::endl;
	m_vmFs << "D=M" << std::endl;

	/*
	 * Compare memory at SP[-2] with SP[-1].
	 */
	m_vmFs << "@SP" << std::endl;
	m_vmFs << "A=M-1" << std::endl;

	m_vmFs << "D=D-M" << std::endl;

	m_vmFs << "@branch" << m_branchCounter << std::endl;

	if (command == "EQ")
		m_vmFs << "D;JEQ" << std::endl;
	else if (command == "LT")
		m_vmFs << "D;JLT" << std::endl;
	else if (command == "GT")
		m_vmFs << "D;JGT" << std::endl;

	/*
	 * Branch for false (0).
	 */
	m_vmFs << "D=0" << std::endl;

	m_vmFs << "@branch" << (m_branchCounter + 1) << std::endl;
	m_vmFs << "0;JMP" << std::endl;

	/*
	 * Branch for true (-1).
	 */
	m_vmFs << "(branch" << m_branchCounter << ")" << std::endl;
	m_vmFs << "D=-1" << std::endl;

	/*
	 * Save result at SP[-2], then decrement SP.
	 */
	m_vmFs << "(branch" << (m_branchCounter + 1) << ")" << std::endl;

	m_vmFs << "@SP" << std::endl;
	m_vmFs << "A=M-1" << std::endl;
	m_vmFs << "A=A-1" << std::endl;
	m_vmFs << "M=D" << std::endl;

	m_vmFs << "@SP" << std::endl;
	m_vmFs << "D=M-1" << std::endl;
	m_vmFs << "M=D" << std::endl;

	m_branchCounter += 2;
}

void
CodeWriter::writeArithmetic(const std::string &command)
{
	if (command == "EQ" || command == "NE" ||
		command == "LT" || command == "GT")
	{
		writeLogic(command);
	}
	else if (command == "NOT" || command == "NEG")
	{
		writeUnary(command);
	}
	else if (command == "ADD" || command == "SUB" ||
		command == "AND" || command == "OR")
	{
		writeOperation(command);
	}
}


void
CodeWriter::writePushPop(Parser::COMMAND_TYPE command,
		const std::string &segment, int index)
{
	if (command == Parser::C_PUSH)
	{
		if (segment == Parser::SEGMENT_CONSTANT)
		{
			/*
			 * Save new value at stack pointer SP.
			 */
			m_vmFs << "@" << index << std::endl;
			m_vmFs << "D=A" << std::endl;
			m_vmFs << "@SP" << std::endl;
			m_vmFs << "A=M" << std::endl;
			m_vmFs << "M=D" << std::endl;

			/*
			 * Increment SP.
			 */
			m_vmFs << "@SP" << std::endl;
			m_vmFs << "D=M+1" << std::endl;
			m_vmFs << "M=D" << std::endl;
		}
	}
	else
	{
	}
}

void
CodeWriter::close()
{
	m_vmFs.close();
}

