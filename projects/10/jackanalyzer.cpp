#include <iostream>

#include "jackanalyzer.h"

bool
JackAnalyzer::advanceToken(JackTokenizer &jt)
{
	bool b = jt.hasMoreTokens();
	if (b)
		jt.advance();
	else
		std::cerr << "Unexpected end of file: " << jt.getFilenameAndLineNumber() << std::endl;
	return b;
}

bool
JackAnalyzer::compileSymbol(JackTokenizer &jt, CompilationEngine &compiler, char symbol)
{
	if (!(jt.tokenType() == JackTokenizer::T_SYMBOL &&
		jt.symbol() == symbol))
	{
		std::cerr << "Expected '" << symbol << "': " << jt.getFilenameAndLineNumber() << std::endl;
		return false;
	}
	compiler.compileTerm(symbol);

	return true;
}

bool
JackAnalyzer::compileClassVarDec(JackTokenizer &jt, CompilationEngine &compiler)
{
	/*
	 * ('static' | 'field') type varName (',' varName)* ';'
	 */
	return true;
}

bool
JackAnalyzer::compileClassName(JackTokenizer &jt, CompilationEngine &compiler)
{
	if (jt.tokenType() == JackTokenizer::T_IDENTIFIER)
	{
		compiler.compileTerm(jt.identifier());
	}
	else
	{
		std::cerr << "Expected class name: " << jt.getFilenameAndLineNumber() << std::endl;
		return false;
	}
	return true;
}

bool
JackAnalyzer::compileType(JackTokenizer &jt, CompilationEngine &compiler)
{
	/*
	 * 'int' | 'char' | 'boolean' | className
	 */
	if (jt.tokenType() == JackTokenizer::T_KEYWORD)
	{
		switch (jt.keyword())
		{
		case JackTokenizer::K_INT:
		case JackTokenizer::K_CHAR:
		case JackTokenizer::K_BOOLEAN:
			compiler.compileTerm(jt.keyword());
			break;
		default:
			std::cerr << "Expected type: " << jt.getFilenameAndLineNumber() << std::endl;
			return false;
		}
	}
	else
	{
		if (!compileClassName(jt, compiler))
			return false;
	}
	return true;
}

bool
JackAnalyzer::compileSubroutineName(JackTokenizer &jt, CompilationEngine &compiler)
{
	if (jt.tokenType() != JackTokenizer::T_IDENTIFIER)
	{
		std::cerr << "Expected subroutine name: " << jt.getFilenameAndLineNumber() << std::endl;
		return false;
	}
	compiler.compileTerm(jt.identifier());
	return true;
}

bool
JackAnalyzer::compileVarName(JackTokenizer &jt, CompilationEngine &compiler)
{
	if (jt.tokenType() != JackTokenizer::T_IDENTIFIER)
	{
		std::cerr << "Expected var name: " << jt.getFilenameAndLineNumber() << std::endl;
		return false;
	}
	std::string varName = jt.identifier();
	compiler.compileIdentifier(varName);

	return true;
}

bool
JackAnalyzer::compileParameterList(JackTokenizer &jt, CompilationEngine &compiler)
{
	compiler.compileParameterList();

	/*
	 * ((type varName) (',' type varName)*)?
	 */
	if (jt.tokenType() == JackTokenizer::T_SYMBOL && jt.symbol() == ')')
		return true;

	while (compileType(jt, compiler))
	{
		if (!advanceToken(jt))
			return false;

		if (!compileVarName(jt, compiler))
			break;

		if (!advanceToken(jt))
			return false;

		if (!compileSymbol(jt, compiler, ','))
			break;
	}

	return true;
}

bool
JackAnalyzer::compileExpressionList(JackTokenizer &jt, CompilationEngine &compiler)
{
	/*
	 * (expression (',' expression)*)?
	 */
	compiler.beginCompileExpressionList();

	if (!(jt.tokenType() == JackTokenizer::T_SYMBOL && jt.symbol() == ')'))
	{
		while (compileExpression(jt, compiler))
		{
			if (!(jt.tokenType() == JackTokenizer::T_SYMBOL &&
				jt.symbol() == ','))
			{
				break;
			}
			compileSymbol(jt, compiler, ',');
		}
	}

	compiler.endCompileExpressionList();

	return true;
}

bool
JackAnalyzer::compileTerm(JackTokenizer &jt, CompilationEngine &compiler)
{
	/*
	 * integerConstant | stringConstant | keywordConstant |
	 * varName | varName '[' expression ']' | subroutineCall |
	 * '(' expression ')' | unaryOp term
	 */
	compiler.beginCompileTerm();

	switch (jt.tokenType())
	{
	case JackTokenizer::T_INT_CONST:
		compiler.compileIntConst(jt.intVal());
		if (!advanceToken(jt))
			return false;
		break;
	case JackTokenizer::T_STRING_CONST:
		compiler.compileStringConst(jt.stringVal());
		if (!advanceToken(jt))
			return false;
		break;
	case JackTokenizer::T_KEYWORD:
		switch (jt.keyword())
		{
		case JackTokenizer::K_TRUE:
		case JackTokenizer::K_FALSE:
		case JackTokenizer::K_NULL:
		case JackTokenizer::K_THIS:
			compiler.compileTerm(jt.stringVal());
			if (!advanceToken(jt))
				return false;
		default:
			std::cerr << "Expected term: " << jt.getFilenameAndLineNumber() << std::endl;
			return false;
		}
		break;
	case JackTokenizer::T_IDENTIFIER:
		if (!compileVarName(jt, compiler))
			return false;

		if (!advanceToken(jt))
			return false;

		if (jt.tokenType() == JackTokenizer::T_SYMBOL &&
			jt.symbol() == '[')
		{
			compileSymbol(jt, compiler, '[');

			if (!advanceToken(jt))
				return false;

			if (!compileExpression(jt, compiler))
				return false;

			if (!compileSymbol(jt, compiler, ']'))
				return false;

			if (!advanceToken(jt))
				return false;
		}
		else if (jt.tokenType() == JackTokenizer::T_SYMBOL &&
			jt.symbol() == '.')
		{
			compileSymbol(jt, compiler, '.');

			if (!advanceToken(jt))
				return false;

			if (!compileVarName(jt, compiler))
				return false;

			if (!advanceToken(jt))
				return false;

			if (!compileSymbol(jt, compiler, '('))
				return false;

			if (!advanceToken(jt))
				return false;

			if (!compileExpressionList(jt, compiler))
				return false;

			if (!compileSymbol(jt, compiler, ')'))
				return false;

			if (!advanceToken(jt))
				return false;
		}
		else if (jt.tokenType() == JackTokenizer::T_SYMBOL &&
			jt.symbol() == '(')
		{
		}

		break;
	default:
		return false;
	}

	compiler.endCompileTerm();

	return true;
}

bool
JackAnalyzer::isOp(char symbol)
{
	return (symbol == '+' ||
		symbol == '-' ||
		symbol == '*' ||
		symbol == '/' ||
		symbol == '&' ||
		symbol == '|' ||
		symbol == '<' ||
		symbol == '>' ||
		symbol == '=');
}

bool
JackAnalyzer::compileExpression(JackTokenizer &jt, CompilationEngine &compiler)
{
	/*
	 * term (op term)*
	 */
	compiler.beginCompileExpression();

	if (!compileTerm(jt, compiler))
		return false;

	while (jt.tokenType() == JackTokenizer::T_SYMBOL && isOp(jt.symbol()))
	{
		compileSymbol(jt, compiler, jt.symbol());

		if (!advanceToken(jt))
			return false;

		if (!compileTerm(jt, compiler))
			return false;
	}

	compiler.endCompileExpression();

	return true;
}

bool
JackAnalyzer::compileLetStatement(JackTokenizer &jt, CompilationEngine &compiler)
{
	/*
	 * 'let' varName ('[' expression ']')? '=' expression ';'
	 */
	compiler.beginCompileLet();
	compiler.compileTerm(JackTokenizer::K_LET);

	if (!advanceToken(jt))
		return false;

	if (!compileVarName(jt, compiler))
		return false;

	if (!advanceToken(jt))
		return false;

	if (jt.tokenType() == JackTokenizer::T_SYMBOL && jt.symbol() == '[')
	{
		compiler.compileTerm('[');

		if (!advanceToken(jt))
			return false;

		if (!compileExpression(jt, compiler))
			return false;

		if (!compileSymbol(jt, compiler, ']'))
			return false;

		if (!advanceToken(jt))
			return false;
	}

	if (!compileSymbol(jt, compiler, '='))
		return false;

	if (!advanceToken(jt))
		return false;

	if (!compileExpression(jt, compiler))
		return false;

	if (!compileSymbol(jt, compiler, ';'))
		return false;

	if (!advanceToken(jt))
		return false;

	compiler.endCompileLet();

	return true;
}

bool
JackAnalyzer::compileWhileStatement(JackTokenizer &jt, CompilationEngine &compiler)
{
	/*
	 * 'while' '(' expression ')' '{' statements '}'
	 */
	compiler.beginCompileWhile();
	compiler.compileTerm(JackTokenizer::K_WHILE);

	if (!advanceToken(jt))
		return false;

	if (!compileSymbol(jt, compiler, '('))
		return false;

	if (!advanceToken(jt))
		return false;

	if (!compileExpression(jt, compiler))
		return false;

	if (!compileSymbol(jt, compiler, ')'))
		return false;

	if (!advanceToken(jt))
		return false;

	if (!compileSymbol(jt, compiler, '{'))
		return false;

	if (!advanceToken(jt))
		return false;

	if (!compileStatements(jt, compiler))
		return false;

	if (!compileSymbol(jt, compiler, '}'))
		return false;

	if (!advanceToken(jt))
		return false;

	compiler.endCompileWhile();
	return true;
}

bool
JackAnalyzer::compileSubroutineCall(JackTokenizer &jt, CompilationEngine &compiler)
{
	/*
	 * subroutineName '(' expressionList ')' | (className | varName)
	 * '.' subroutineName '(' expressionList ')'
	 */
	if (!compileSubroutineName(jt, compiler))
		return false;

	if (!advanceToken(jt))
		return false;

	if (jt.tokenType() == JackTokenizer::T_SYMBOL && jt.symbol() == '.')
	{
		compileSymbol(jt, compiler, '.');

		if (!advanceToken(jt))
			return false;

		if (!compileSubroutineName(jt, compiler))
			return false;

		if (!advanceToken(jt))
			return false;
	}

	if (!compileSymbol(jt, compiler, '('))
		return false;

	if (!advanceToken(jt))
		return false;

	if (!compileExpressionList(jt, compiler))
		return false;

	if (!compileSymbol(jt, compiler, ')'))
		return false;

	if (!advanceToken(jt))
		return false;

	return true;
}

bool
JackAnalyzer::compileDoStatement(JackTokenizer &jt, CompilationEngine &compiler)
{
	/*
	 * 'do' subroutineCall ';'
	 */
	compiler.beginCompileDo();
	compiler.compileTerm(JackTokenizer::K_DO);

	if (!advanceToken(jt))
		return false;

	if (!compileSubroutineCall(jt, compiler))
		return false;

	if (!compileSymbol(jt, compiler, ';'))
		return false;

	if (!advanceToken(jt))
		return false;

	compiler.endCompileDo();

	return true;
}

bool
JackAnalyzer::compileReturnStatement(JackTokenizer &jt, CompilationEngine &compiler)
{
	/*
	 * 'do' subroutineCall ';'
	 */
	compiler.compileReturnStatement();
	compiler.compileTerm(JackTokenizer::K_RETURN);

	if (!advanceToken(jt))
		return false;

	if (jt.tokenType() == JackTokenizer::T_SYMBOL &&
		jt.symbol() == ';')
	{
		compileSymbol(jt, compiler, ';');
	}
	else
	{
		if (!compileExpression(jt, compiler))
			return false;

		if (!compileSymbol(jt, compiler, ';'))
			return false;
	}

	if (!advanceToken(jt))
		return false;

	return true;
}

bool
JackAnalyzer::compileStatement(JackTokenizer &jt, CompilationEngine &compiler)
{
	/*
	 * letStatement | ifStatement | whileStatement | doStatement | returnStatement
	 */
	if (jt.tokenType() != JackTokenizer::T_KEYWORD)
	{
		std::cerr << "Expected statement: " << jt.getFilenameAndLineNumber() << std::endl;
		return false;
	}

	switch (jt.keyword())
	{
	case JackTokenizer::K_LET:
		if (!compileLetStatement(jt, compiler))
			return false;
		break;
	case JackTokenizer::K_IF:
		break;
	case JackTokenizer::K_WHILE:
		if (!compileWhileStatement(jt, compiler))
			return false;
		break;
	case JackTokenizer::K_DO:
		if (!compileDoStatement(jt, compiler))
			return false;
		break;
	case JackTokenizer::K_RETURN:
		if (!compileReturnStatement(jt, compiler))
			return false;
		break;
	default:
		std::cerr << "Expected statement: " << jt.getFilenameAndLineNumber() << std::endl;
		return false;
	}
	return true;
}

bool
JackAnalyzer::compileStatements(JackTokenizer &jt, CompilationEngine &compiler)
{
	/*
	 * statement*
	 */
	compiler.beginCompileStatements();
	while (jt.tokenType() == JackTokenizer::T_KEYWORD &&
		compileStatement(jt, compiler))
	{
	}
	compiler.endCompileStatements();

	return true;
}

bool
JackAnalyzer::compileVarDec(JackTokenizer &jt, CompilationEngine &compiler)
{
	/*
	 * 'var' type varName (',' varName)* ';'
	 */
	if (jt.tokenType() == JackTokenizer::T_KEYWORD &&
		jt.keyword() == JackTokenizer::K_VAR)
	{
		compiler.compileVarDec();
		compiler.compileTerm(jt.keyword());
	}
	else
	{
		std::cerr << "Expected 'var': " << jt.getFilenameAndLineNumber() << std::endl;
		return false;
	}

	if (!advanceToken(jt))
		return false;

	if (!compileType(jt, compiler))
		return false;

	if (!advanceToken(jt))
		return false;

	if (!compileVarName(jt, compiler))
		return false;

	if (!advanceToken(jt))
		return false;

	if (jt.tokenType() == JackTokenizer::T_SYMBOL)
	{
		while (jt.tokenType() == JackTokenizer::T_SYMBOL &&
			jt.symbol() == ',')
		{
			compileSymbol(jt, compiler, ',');

			if (!advanceToken(jt))
				return false;

			if (!compileVarName(jt, compiler))
				return false;

			if (!advanceToken(jt))
				return false;
		}
		if (!compileSymbol(jt, compiler, ';'))
			return false;
	}
	else
	{
		std::cerr << "Expected variable name: " << jt.getFilenameAndLineNumber() << std::endl;
		return false;
	}

	return true;
}

bool
JackAnalyzer::compileSubroutineBody(JackTokenizer &jt, CompilationEngine &compiler)
{
	compiler.beginCompileSubroutineBody();

	/*
	 * '{' varDec* statements '}'
	 */
	if (!compileSymbol(jt, compiler, '{'))
		return false;

	if (!advanceToken(jt))
		return false;

	while (jt.tokenType() == JackTokenizer::T_KEYWORD &&
		jt.keyword() == JackTokenizer::K_VAR &&
		compileVarDec(jt, compiler))
	{
		if (!advanceToken(jt))
			return false;
	}

	if (!compileStatements(jt, compiler))
		return false;

	if (!compileSymbol(jt, compiler, '}'))
		return false;

	compiler.endCompileSubroutineBody();

	return true;
}

bool
JackAnalyzer::compileSubroutineDec(JackTokenizer &jt, CompilationEngine &compiler)
{
	/*
	 * ('constructor' | 'function' | 'method')
	 * ('void' | type) subroutineName '(' parameterList ')'
	 * subroutineBody
	 */
	switch (jt.keyword())
	{
	case JackTokenizer::K_METHOD:
	case JackTokenizer::K_FUNCTION:
	case JackTokenizer::K_CONSTRUCTOR:
		compiler.beginCompileSubroutineDec(jt.keyword());
		break;
	default:
		std::cerr << "Expected subroutine: " << jt.getFilenameAndLineNumber() << std::endl;
		return false;
	}

	if (!advanceToken(jt))
		return false;

	if (jt.tokenType() == JackTokenizer::T_KEYWORD &&
		jt.keyword() == JackTokenizer::K_VOID)
	{
		compiler.compileTerm(JackTokenizer::K_VOID);
	}
	else
	{
		if (!compileType(jt, compiler))
			return false;
	}

	if (!advanceToken(jt))
		return false;

	if (!compileSubroutineName(jt, compiler))
		return false;

	if (!advanceToken(jt))
		return false;

	if (!compileSymbol(jt, compiler, '('))
		return false;

	if (!advanceToken(jt))
		return false;

	if (!compileParameterList(jt, compiler))
		return false;

	if (!compileSymbol(jt, compiler, ')'))
		return false;

	if (!advanceToken(jt))
		return false;

	if (!compileSubroutineBody(jt, compiler))
		return false;

	compiler.endCompileSubroutineDec();

	return true;
}

bool
JackAnalyzer::compile(JackTokenizer &jt, CompilationEngine &compiler)
{
	/*
	 * Compile a class: 'class' className '{' classVarDec* subroutineDec* '}'
	 */
	if (!advanceToken(jt))
		return false;

	if (jt.tokenType() != JackTokenizer::T_KEYWORD ||
		jt.keyword() != JackTokenizer::K_CLASS)
	{
		std::cerr << "Expected class: " << jt.getFilenameAndLineNumber() << std::endl;
		return false;
	}
	compiler.beginCompileClass();
	compiler.compileTerm(jt.keyword());

	if (!advanceToken(jt))
		return false;

	if (jt.tokenType() != JackTokenizer::T_IDENTIFIER)
	{
		std::cerr << "Expected class name: " << jt.getFilenameAndLineNumber() << std::endl;
		return false;
	}
	std::string className = jt.identifier();
	compiler.compileIdentifier(className);

	if (!advanceToken(jt))
		return false;

	if (!compileSymbol(jt, compiler, '{'))
		return false;

	if (!advanceToken(jt))
		return false;

	if (jt.tokenType() != JackTokenizer::T_KEYWORD)
	{
		std::cerr << "Expected subroutine declaration: " << jt.getFilenameAndLineNumber() << std::endl;
		return false;
	}

	do
	{
		switch (jt.keyword())
		{
		case JackTokenizer::K_VAR:
			compileClassVarDec(jt, compiler);
			break;
		case JackTokenizer::K_METHOD:
		case JackTokenizer::K_FUNCTION:
		case JackTokenizer::K_CONSTRUCTOR:
			compileSubroutineDec(jt, compiler);
			break;
		default:
			std::cerr << "Expected subroutine declaration: " << jt.getFilenameAndLineNumber() << std::endl;
			return false;
		}

		if (!advanceToken(jt))
			return false;
	}
	while (!(jt.tokenType() == JackTokenizer::T_SYMBOL && jt.symbol() == '}'));

	compiler.compileTerm('}');

	compiler.endCompileClass();

	return true;
}
