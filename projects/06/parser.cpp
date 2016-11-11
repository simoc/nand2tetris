#include <vector>

#include "parser.h"

Parser::Parser(const std::string &filename)
{
	m_filename = filename;
}

Parser::~Parser()
{
	m_fs.close();
}

bool
Parser::open()
{
	m_fs.open(m_filename.c_str());
	return m_fs.is_open();
}

bool
Parser::hasMoreCommands()
{
	char c;
	bool inComment = false;
	bool inSymbol = false;
	bool hasJump = false;
	bool eol = false;
	int readCount = 0;

	std::string token;

	m_currentSymbol.clear();
	m_currentDest.clear();
	m_currentComp.clear();
	m_currentJump.clear();
	m_currentCommandType = C_COMMAND;

	while (eol == false && m_fs.eof() == false)
	{
		/*
		 * Read next command, to be sure that more commands
		 * really are available.
		 */
		m_fs.get(c);

		if (c == '\r' || c == '\n')
		{
			/*
			 * Stop at end of line if we successfully read a command.
			 * Otherwise, continue trying to read a command
			 * from the next line.
			 */
			if (readCount > 0)
				eol = true;
			inComment = false;
		}
		else if (inComment)
		{
			/*
			 * Ignore everything if in comment.
			 */
		}
		else if (c == '/')
		{
			inComment = true;
		}
		else if (c == '(')
		{
			inSymbol = true;
			m_currentCommandType = L_COMMAND;
		}
		else if (c == ')')
		{
			inSymbol = false;
		}
		else if (c == '@')
		{
			inSymbol = true;
			m_currentCommandType = A_COMMAND;
		}
		else if (c == '=')
		{
			/*
			 * Finished reading dest.
			 */
			m_currentDest = token;
			token.clear();
		}
		else if (c == ';')
		{
			/*
			 * Finished reading comp.
			 */
			m_currentComp = token;
			token.clear();
			hasJump = true;
		}
		else if (c == ' ' || c == '\t')
		{
			/*
			 * Skip whitespace.
			 */
		}
		else
		{
			token.push_back(c);
			readCount++;
		}
	}

	if (m_currentCommandType == C_COMMAND)
	{
		/*
		 * Save any jump or comp that we were still reading when we
		 * reached the end of line.
		 */
		if (hasJump)
			m_currentJump = token;
		else if (!token.empty())
			m_currentComp = token;
	}
	else
	{
		/*
		 * Everything that we read is the symbol.
		 */
		m_currentSymbol = token;
	}

	return (readCount > 0);
}

void
Parser::advance()
{
	/*
	 * Nothing to do because we have already read next command.
	 */
}

enum Parser::COMMAND_TYPE
Parser::commandType()
{
	return m_currentCommandType;
}

std::string
Parser::symbol()
{
	return m_currentSymbol;
}

std::string
Parser::dest()
{
	return m_currentDest;
}

std::string
Parser::comp()
{
	return m_currentComp;
}

std::string
Parser::jump()
{
	return m_currentJump;
}
