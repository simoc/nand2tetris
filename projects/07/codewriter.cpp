#include "codewriter.h"

CodeWriter::CodeWriter()
{
	m_branchCounter = 0;
}

void
CodeWriter::setAsmFilename(const std::string &filename)
{
	m_asmFilename = filename;
}

bool
CodeWriter::open()
{
	m_asmFs.open(m_asmFilename.c_str());
	return m_asmFs.is_open();
}

void
CodeWriter::setVmFilename(const std::string &filename)
{
	/*
	 * Save VM filename without suffix and directory path.
	 */
	m_vmFilename = filename;
	size_t index = m_vmFilename.find_last_of('.');
	if (index != std::string::npos)
		m_vmFilename = m_vmFilename.substr(0, index);
	index = m_vmFilename.find_last_of('/');
	if (index != std::string::npos)
		m_vmFilename = m_vmFilename.substr(index + 1);
}

void
CodeWriter::writeUnary(const std::string &command)
{
	m_asmFs << "// " << command << std::endl;

	/*
	 * Load memory at SP[-1].
	 */
	m_asmFs << "@SP" << std::endl;
	m_asmFs << "A=M-1" << std::endl;
	m_asmFs << "D=M" << std::endl;

	/*
	 * Replace value at top of stack with result of
	 * unary operation.
	 */
	if (command == "NEG")
	{
		m_asmFs << "M=-D" << std::endl;
	}
	else if (command == "NOT")
	{
		m_asmFs << "M=!D" << std::endl;
	}
}

void
CodeWriter::writeOperation(const std::string &command)
{
	m_asmFs << "// " << command << std::endl;

	/*
	 * Load memory at SP[-2].
	 */
	m_asmFs << "@SP" << std::endl;
	m_asmFs << "A=M-1" << std::endl;
	m_asmFs << "A=A-1" << std::endl;
	m_asmFs << "D=M" << std::endl;

	/*
	 * Do operation with memory at SP[-1].
	 */
	m_asmFs << "@SP" << std::endl;
	m_asmFs << "A=M-1" << std::endl;

	if (command == "ADD")
	{
		m_asmFs << "D=D+M" << std::endl;
	}
	else if (command == "SUB")
	{
		m_asmFs << "D=D-M" << std::endl;
	}
	else if (command == "AND")
	{
		m_asmFs << "D=D&M" << std::endl;
	}
	else if (command == "OR")
	{
		m_asmFs << "D=D|M" << std::endl;
	}

	/*
	 * Save result back at SP[-2].
	 */
	m_asmFs << "@SP" << std::endl;
	m_asmFs << "A=M-1" << std::endl;
	m_asmFs << "A=A-1" << std::endl;
	m_asmFs << "M=D" << std::endl;

	/*
	 * One less element on stack so decrement SP.
	 */
	m_asmFs << "@SP" << std::endl;
	m_asmFs << "D=M-1" << std::endl;
	m_asmFs << "M=D" << std::endl;
}

void
CodeWriter::writeLogic(const std::string &command)
{
	m_asmFs << "// " << command << std::endl;

	/*
	 * Load memory at SP[-2].
	 */
	m_asmFs << "@SP" << std::endl;
	m_asmFs << "A=M-1" << std::endl;
	m_asmFs << "A=A-1" << std::endl;
	m_asmFs << "D=M" << std::endl;

	/*
	 * Compare memory at SP[-2] with SP[-1].
	 */
	m_asmFs << "@SP" << std::endl;
	m_asmFs << "A=M-1" << std::endl;

	m_asmFs << "D=D-M" << std::endl;

	m_asmFs << "@branch" << m_branchCounter << std::endl;

	if (command == "EQ")
		m_asmFs << "D;JEQ" << std::endl;
	else if (command == "LT")
		m_asmFs << "D;JLT" << std::endl;
	else if (command == "GT")
		m_asmFs << "D;JGT" << std::endl;

	/*
	 * Branch for false (0).
	 */
	m_asmFs << "D=0" << std::endl;

	m_asmFs << "@branch" << (m_branchCounter + 1) << std::endl;
	m_asmFs << "0;JMP" << std::endl;

	/*
	 * Branch for true (-1).
	 */
	m_asmFs << "(branch" << m_branchCounter << ")" << std::endl;
	m_asmFs << "D=-1" << std::endl;

	/*
	 * Save result at SP[-2], then decrement SP.
	 */
	m_asmFs << "(branch" << (m_branchCounter + 1) << ")" << std::endl;

	m_asmFs << "@SP" << std::endl;
	m_asmFs << "A=M-1" << std::endl;
	m_asmFs << "A=A-1" << std::endl;
	m_asmFs << "M=D" << std::endl;

	m_asmFs << "@SP" << std::endl;
	m_asmFs << "D=M-1" << std::endl;
	m_asmFs << "M=D" << std::endl;

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
		m_asmFs << "// PUSH " << segment << " " << index << std::endl;

		if (segment == Parser::SEGMENT_CONSTANT)
		{

			/*
			 * Save new value at stack pointer SP.
			 */
			m_asmFs << "@" << index << std::endl;
			m_asmFs << "D=A" << std::endl;
			m_asmFs << "@SP" << std::endl;
			m_asmFs << "A=M" << std::endl;
			m_asmFs << "M=D" << std::endl;
		}
		else if (segment == Parser::SEGMENT_LOCAL ||
			segment == Parser::SEGMENT_ARGUMENT ||
			segment == Parser::SEGMENT_THIS ||
			segment == Parser::SEGMENT_THAT)
		{
			if (segment == Parser::SEGMENT_LOCAL)
				m_asmFs << "@LCL" << std::endl;
			else if (segment == Parser::SEGMENT_ARGUMENT)
				m_asmFs << "@ARG" << std::endl;
			else if (segment == Parser::SEGMENT_THIS)
				m_asmFs << "@THIS" << std::endl;
			else if (segment == Parser::SEGMENT_THAT)
				m_asmFs << "@THAT" << std::endl;

			/*
			 * Load segment[index].
			 */
			m_asmFs << "D=M" << std::endl;
			m_asmFs << "@" << index << std::endl;
			m_asmFs << "A=D+A" << std::endl;
			m_asmFs << "D=M" << std::endl;

			/*
			 * Save at SP.
			 */
			m_asmFs << "@SP" << std::endl;
			m_asmFs << "A=M" << std::endl;
			m_asmFs << "M=D" << std::endl;
		}
		else if (segment == Parser::SEGMENT_POINTER)
		{
			m_asmFs << "@" << (index + 3) << std::endl;
			m_asmFs << "D=M" << std::endl;

			/*
			 * Save at SP.
			 */
			m_asmFs << "@SP" << std::endl;
			m_asmFs << "A=M" << std::endl;
			m_asmFs << "M=D" << std::endl;
		}
		else if (segment == Parser::SEGMENT_TEMP)
		{
			m_asmFs << "@" << (index + 5) << std::endl;
			m_asmFs << "D=M" << std::endl;

			/*
			 * Save at SP.
			 */
			m_asmFs << "@SP" << std::endl;
			m_asmFs << "A=M" << std::endl;
			m_asmFs << "M=D" << std::endl;
		}
		else if (segment == Parser::SEGMENT_STATIC)
		{
			/*
			 * Load named symbol and save it at SP.
			 */
			m_asmFs << "@" << m_vmFilename << "." << index << std::endl;
			m_asmFs << "D=M" << std::endl;

			m_asmFs << "@SP" << std::endl;
			m_asmFs << "A=M" << std::endl;
			m_asmFs << "M=D" << std::endl;
		}

		/*
		 * Increment SP.
		 */
		m_asmFs << "@SP" << std::endl;
		m_asmFs << "D=M+1" << std::endl;
		m_asmFs << "M=D" << std::endl;
	}
	else if (command == Parser::C_POP)
	{
		m_asmFs << "// POP " << segment << " " << index << std::endl;

		if (segment == Parser::SEGMENT_LOCAL ||
			segment == Parser::SEGMENT_ARGUMENT ||
			segment == Parser::SEGMENT_THIS ||
			segment == Parser::SEGMENT_THAT)
		{
			/*
			 * Calculate segment[index] address and
			 * save in @R13.
			 */
			if (segment == Parser::SEGMENT_LOCAL)
				m_asmFs << "@LCL" << std::endl;
			else if (segment == Parser::SEGMENT_ARGUMENT)
				m_asmFs << "@ARG" << std::endl;
			else if (segment == Parser::SEGMENT_THIS)
				m_asmFs << "@THIS" << std::endl;
			else if (segment == Parser::SEGMENT_THAT)
				m_asmFs << "@THAT" << std::endl;

			m_asmFs << "D=M" << std::endl;
			m_asmFs << "@" << index << std::endl;
			m_asmFs << "D=D+A" << std::endl;
			m_asmFs << "@R13" << std::endl;
			m_asmFs << "M=D" << std::endl;

			/*
			 * Save SP[-1] at address stored in @R13.
			 */
			m_asmFs << "@SP" << std::endl;
			m_asmFs << "A=M-1" << std::endl;
			m_asmFs << "D=M" << std::endl;
			m_asmFs << "@R13" << std::endl;
			m_asmFs << "A=M" << std::endl;
			m_asmFs << "M=D" << std::endl;
		}
		else if (segment == Parser::SEGMENT_POINTER)
		{
			/*
			 * Load SP[-1].
			 */
			m_asmFs << "@SP" << std::endl;
			m_asmFs << "A=M-1" << std::endl;
			m_asmFs << "D=M" << std::endl;

			/*
			 * Save at address + 5.
			 */
			m_asmFs << "@" << (index + 3) << std::endl;
			m_asmFs << "M=D" << std::endl;
		}
		else if (segment == Parser::SEGMENT_TEMP)
		{
			/*
			 * Load SP[-1].
			 */
			m_asmFs << "@SP" << std::endl;
			m_asmFs << "A=M-1" << std::endl;
			m_asmFs << "D=M" << std::endl;

			/*
			 * Save at address + 5.
			 */
			m_asmFs << "@" << (index + 5) << std::endl;
			m_asmFs << "M=D" << std::endl;
		}
		else if (segment == Parser::SEGMENT_STATIC)
		{
			/*
			 * Load SP[-1].
			 */
			m_asmFs << "@SP" << std::endl;
			m_asmFs << "A=M-1" << std::endl;
			m_asmFs << "D=M" << std::endl;

			/*
			 * Save at address of named symbol.
			 */
			m_asmFs << "@" << m_vmFilename << "." << index << std::endl;
			m_asmFs << "M=D" << std::endl;
		}

		/*
		 * Decrement SP.
		 */
		m_asmFs << "@SP" << std::endl;
		m_asmFs << "D=M-1" << std::endl;
		m_asmFs << "M=D" << std::endl;
	}
}

void
CodeWriter::close()
{
	m_asmFs.close();
}

