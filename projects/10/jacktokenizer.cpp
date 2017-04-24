#include <sstream>
#include "jacktokenizer.h"

JackTokenizer::JackTokenizer(const std::string &filename)
{
	m_filename = filename;
	m_lineNumber = 1;
}

JackTokenizer::~JackTokenizer()
{
	m_fs.close();
}

bool
JackTokenizer::open()
{
	m_fs.open(m_filename.c_str());
	return m_fs.is_open();
}

bool
JackTokenizer::isIdentifierChar(char c)
{
	return ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') ||
		(c >= 'A' && c <= 'Z') || c == '_');
}

bool
JackTokenizer::hasMoreTokens()
{
	char c, nextC;
	bool inComment = false;
	bool inSingleLineComment = false;
	bool inQuotedString = false;
	bool tokenComplete = false;

	m_token.clear();
	m_tokenType = T_UNKNOWN;
	while (m_fs.eof() == false && tokenComplete == false)
	{
		m_fs.get(c);

		if (inQuotedString)
		{
			if (c == '"')
				tokenComplete = true;
			else
				m_token.push_back(c);
		}
		else if (c == ' ' || c == '\t')
		{
			if (m_tokenType != T_UNKNOWN)
				tokenComplete = true;
		}
		else if (c == '\r' || c == '\n')
		{
			inSingleLineComment = false;
			if (c == '\n')
				m_lineNumber++;

			if (m_tokenType != T_UNKNOWN)
				tokenComplete = true;
		}
		else if (inSingleLineComment)
		{
			/*
			 * Ignore everything until end of line.
			 */
		}
		else if (c == '/' && m_fs.peek() == '/')
		{
			m_fs.get(c);
			inSingleLineComment = true;
		}
		else if (c == '/' && m_fs.peek() == '*')
		{
			m_fs.get(c);
			inComment = true;
		}
		else if (c == '*' && m_fs.peek() == '/')
		{
			m_fs.get(c);
			if (inComment)
			{
				inComment = false;
			}
			else
			{
				std::cerr << "Not in comment" << std::endl;
				tokenComplete = true;
			}
		}
		else if (inComment)
		{
			/*
			 * Ignore everything until end of comment.
			 */
		}
		else if (c == '"')
		{
			inQuotedString = true;
			m_tokenType = T_STRING_CONST;
		}
		else if (c == '(' || c == ')' || c == '{' || c == '}' ||
			c == '[' || c == ']' || c == ',' || c == ';' ||
			c == '=' || c == '.' || c == '+' || c == '-' ||
			c == '*' || c == '/' || c == '&' || c == '|' ||
			c == '~' || c == '<' || c == '>')
		{
			m_token.push_back(c);
			m_tokenType = T_SYMBOL;
			tokenComplete = true;
		}
		else if (c >= '0' && c <= '9' && m_token.size() == 0)
		{
			m_tokenType = T_INT_CONST;
			m_token.push_back(c);
			nextC = m_fs.peek();
			if (nextC < '0' || nextC > '9')
				tokenComplete = true;
		}
		else if (c >= '0' && c <= '9' && m_tokenType == T_INT_CONST)
		{
			m_token.push_back(c);
			nextC = m_fs.peek();
			if (nextC < '0' || nextC > '9')
				tokenComplete = true;
		}
		else if (isIdentifierChar(c))
		{
			m_tokenType = T_IDENTIFIER;
			m_token.push_back(c);
			nextC = m_fs.peek();
			if (!isIdentifierChar(nextC))
				tokenComplete = true;
		}
	}

	if (m_tokenType == T_IDENTIFIER)
	{
		if (m_token == "class")
		{
			m_tokenType = T_KEYWORD;
			m_keywordType = K_CLASS;
		}
		else if (m_token == "constructor")
		{
			m_tokenType = T_KEYWORD;
			m_keywordType = K_CONSTRUCTOR;
		}
		else if (m_token == "method")
		{
			m_tokenType = T_KEYWORD;
			m_keywordType = K_METHOD;
		}
		else if (m_token == "function")
		{
			m_tokenType = T_KEYWORD;
			m_keywordType = K_FUNCTION;
		}
		else if (m_token == "int")
		{
			m_tokenType = T_KEYWORD;
			m_keywordType = K_INT;
		}
		else if (m_token == "boolean")
		{
			m_tokenType = T_KEYWORD;
			m_keywordType = K_BOOLEAN;
		}
		else if (m_token == "char")
		{
			m_tokenType = T_KEYWORD;
			m_keywordType = K_CHAR;
		}
		else if (m_token == "void")
		{
			m_tokenType = T_KEYWORD;
			m_keywordType = K_VOID;
		}
		else if (m_token == "var")
		{
			m_tokenType = T_KEYWORD;
			m_keywordType = K_VAR;
		}
		else if (m_token == "static")
		{
			m_tokenType = T_KEYWORD;
			m_keywordType = K_STATIC;
		}
		else if (m_token == "field")
		{
			m_tokenType = T_KEYWORD;
			m_keywordType = K_FIELD;
		}
		else if (m_token == "let")
		{
			m_tokenType = T_KEYWORD;
			m_keywordType = K_LET;
		}
		else if (m_token == "do")
		{
			m_tokenType = T_KEYWORD;
			m_keywordType = K_DO;
		}
		else if (m_token == "if")
		{
			m_tokenType = T_KEYWORD;
			m_keywordType = K_IF;
		}
		else if (m_token == "else")
		{
			m_tokenType = T_KEYWORD;
			m_keywordType = K_ELSE;
		}
		else if (m_token == "while")
		{
			m_tokenType = T_KEYWORD;
			m_keywordType = K_WHILE;
		}
		else if (m_token == "return")
		{
			m_tokenType = T_KEYWORD;
			m_keywordType = K_RETURN;
		}
		else if (m_token == "true")
		{
			m_tokenType = T_KEYWORD;
			m_keywordType = K_TRUE;
		}
		else if (m_token == "false")
		{
			m_tokenType = T_KEYWORD;
			m_keywordType = K_FALSE;
		}
		else if (m_token == "null")
		{
			m_tokenType = T_KEYWORD;
			m_keywordType = K_NULL;
		}
		else if (m_token == "this")
		{
			m_tokenType = T_KEYWORD;
			m_keywordType = K_THIS;
		}
	}

	return (m_tokenType != T_UNKNOWN);
}

void
JackTokenizer::advance()
{
}

JackTokenizer::TOKEN_TYPE
JackTokenizer::tokenType()
{
	return m_tokenType;
}

JackTokenizer::KEYWORD_TYPE
JackTokenizer::keyword()
{
	return m_keywordType;
}

char
JackTokenizer::symbol()
{
	return m_token.at(0);
}

std::string
JackTokenizer::identifier()
{
	return m_token;
}

int32_t
JackTokenizer::intVal()
{
	size_t sz;
	return std::stoi(m_token, &sz);
}

std::string
JackTokenizer::stringVal()
{
	return m_token;
}

std::string
JackTokenizer::getFilenameAndLineNumber()
{
	std::ostringstream os;
	os << m_filename << ":" << m_lineNumber;
	return os.str();
}

