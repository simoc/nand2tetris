#include "parser.h"

const std::string Parser::SEGMENT_CONSTANT = "CONSTANT";
const std::string Parser::SEGMENT_LOCAL = "LOCAL";
const std::string Parser::SEGMENT_ARGUMENT = "ARGUMENT";
const std::string Parser::SEGMENT_THIS = "THIS";
const std::string Parser::SEGMENT_THAT = "THAT";
const std::string Parser::SEGMENT_POINTER = "POINTER";
const std::string Parser::SEGMENT_TEMP = "TEMP";
const std::string Parser::SEGMENT_STATIC = "STATIC";

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
	bool eol = false;
	bool inComment = false;
	std::string token;

	m_tokens.clear();

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
			if (m_tokens.size() > 0 || token.size() > 0)
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
		else if (c == ' ' || c == '\t')
		{
			if (!token.empty())
			{
				m_tokens.push_back(token);
				token.clear();
			}
		}
		else
		{
			token.push_back(c);
		}
	}

	if (!token.empty())
	{
		m_tokens.push_back(token);
		token.clear();
	}
	return (m_tokens.size() > 0);
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
	enum Parser::COMMAND_TYPE commandType = C_UNKNOWN;

	if (m_tokens.size() > 0)
	{
		std::string token0 = toUppercase(m_tokens.at(0));

		if (token0 == "ADD" || token0 == "SUB" || token0 == "NEG" ||
			token0 == "EQ" || token0 == "GT" || token0 == "LT" ||
			token0 == "AND" || token0 == "OR" || token0 == "NOT")
		{
			commandType = C_ARITHMETIC;
		}
		else if (token0 == "PUSH")
		{
			commandType = C_PUSH;
		}
		else if (token0 == "POP")
		{
			commandType = C_POP;
		}
	}
	return commandType;
}

std::string
Parser::arg1()
{
	std::string arg1;
	if (commandType() == C_ARITHMETIC)
	{
		if (m_tokens.size() > 0)
			arg1 = toUppercase(m_tokens.at(0));
	}
	else if (m_tokens.size() > 1)
	{
		arg1 = toUppercase(m_tokens.at(1));
	}
	return arg1;
}

std::string
Parser::arg2()
{
	std::string arg2;
	if (m_tokens.size() > 2)
		arg2 = toUppercase(m_tokens.at(2));
	return arg2;
}

std::string
Parser::toUppercase(const std::string &word)
{
    std::string s;

    std::string::const_iterator it = word.begin();
    while (it != word.end())
    {
        s.push_back(toupper(*it));
        it++;
    }
    return s;
}

