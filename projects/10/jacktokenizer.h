#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>

/*
 * Parses a file defining a Jack class into tokens.
 */
class JackTokenizer
{
public:
	enum TOKEN_TYPE
	{
		T_KEYWORD,
		T_SYMBOL,
		T_IDENTIFIER,
		T_INT_CONST,
		T_STRING_CONST,
		T_UNKNOWN
	};

	enum KEYWORD_TYPE
	{
		K_CLASS,
		K_METHOD,
		K_FUNCTION,
		K_CONSTRUCTOR,
		K_INT,
		K_BOOLEAN,
		K_CHAR,
		K_VOID,
		K_VAR,
		K_STATIC,
		K_FIELD,
		K_LET,
		K_DO,
		K_IF,
		K_ELSE,
		K_WHILE,
		K_RETURN,
		K_TRUE,
		K_FALSE,
		K_NULL,
		K_THIS
	};

	/*
	 * Opens Jack source file for parsing.
	 */
	JackTokenizer(const std::string &filename);

	~JackTokenizer();

	/*
	 * Was file opened sucessfully?
	 */
	bool open();

	/*
	 * Do we have more tokens in the input?
	 */
	bool hasMoreTokens();

	/*
	 * Gets the next token from the input and makes it the current token.
	 * This method should only be called if hasMoreTokens() is true.
	 * Initially there is no current token.
	 */
	void advance();

	/*
	 * Returns the type of the current token.
	 */
	TOKEN_TYPE tokenType();

	/*
	 * Returns the type of keyword which is the current token.
	 * Should be called only when tokenType() is KEYWORD.
	 */
	KEYWORD_TYPE keyword();

	/*
	 * Returns the character which is the current token. Should be called
	 * only when tokenType() is SYMBOL.
	 */
	char symbol();

	/*
	 * Returns the identifier which is the current token. Should be called
	 * only when tokenType() is IDENTIFIER.
	 */
	std::string identifier();

	/*
	 * Returns the integer value which is the current token. Should be called
	 * only when tokenType() is INT_CONST.
	 */
	int32_t intVal();

	/*
	 * Returns the string value which is the current token. Should be called
	 * only when tokenType() is STRING_CONST.
	 */
	std::string stringVal();

	/*
	 * Return current position in file, for error messages.
	*/
	std::string getFilenameAndLineNumber();

private:
	std::string m_filename;
	int32_t m_lineNumber;
	std::ifstream m_fs;
	TOKEN_TYPE m_tokenType;
	KEYWORD_TYPE m_keywordType;
	std::string m_token;

	bool isIdentifierChar(char c);
};

