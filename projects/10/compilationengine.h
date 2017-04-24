#pragma once

#include <string>
#include "jacktokenizer.h"

/*
 * Jack compiler interface.
 */
class CompilationEngine
{
public:
	virtual ~CompilationEngine() {};

	virtual void beginCompileClass() = 0;

	virtual void endCompileClass() = 0;

	virtual void compileClassVarDec() = 0;

	virtual void beginCompileSubroutineDec(JackTokenizer::KEYWORD_TYPE keyword) = 0;
	virtual void endCompileSubroutineDec() = 0;

	virtual void beginCompileSubroutineBody() = 0;

	virtual void endCompileSubroutineBody() = 0;

	virtual void compileParameterList() = 0;

	virtual void compileVarDec() = 0;

	virtual void beginCompileStatements() = 0;

	virtual void endCompileStatements() = 0;

	virtual void beginCompileDo() = 0;
	
	virtual void endCompileDo() = 0;

	virtual void beginCompileLet() = 0;

	virtual void endCompileLet() = 0;

	virtual void beginCompileWhile() = 0;

	virtual void endCompileWhile() = 0;

	virtual void compileReturnStatement() = 0;

	virtual void compileIf() = 0;

	virtual void beginCompileExpression() = 0;

	virtual void endCompileExpression() = 0;

	virtual void beginCompileExpressionList() = 0;

	virtual void endCompileExpressionList() = 0;

	virtual void compileIdentifier(const std::string &identifier) = 0;

	virtual void beginCompileTerm() = 0;

	virtual void endCompileTerm() = 0;

	virtual void compileTerm(JackTokenizer::KEYWORD_TYPE keyword) = 0;

	virtual void compileTerm(char term) = 0;

	virtual void compileTerm(const std::string &term) = 0;

	virtual void compileStringConst(const std::string &value) = 0;

	virtual void compileIntConst(int32_t value) = 0;
};
