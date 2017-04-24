#pragma once

#include "jacktokenizer.h"
#include "compilationengine.h"

/*
 * Compiles a Jack class.
 */
class JackAnalyzer
{
private:
	bool isOp(char symbol);
	bool advanceToken(JackTokenizer &jt);
	bool compileSymbol(JackTokenizer &jt, CompilationEngine &compiler, char symbol);
	bool compileTerm(JackTokenizer &jt, CompilationEngine &compiler);
	bool compileExpression(JackTokenizer &jt, CompilationEngine &compiler);
	bool compileExpressionList(JackTokenizer &jt, CompilationEngine &compiler);
	bool compileLetStatement(JackTokenizer &jt, CompilationEngine &compiler);
	bool compileWhileStatement(JackTokenizer &jt, CompilationEngine &compiler);
	bool compileDoStatement(JackTokenizer &jt, CompilationEngine &compiler);
	bool compileReturnStatement(JackTokenizer &jt, CompilationEngine &compiler);
	bool compileSubroutineCall(JackTokenizer &jt, CompilationEngine &compiler);
	bool compileStatement(JackTokenizer &jt, CompilationEngine &compiler);
	bool compileStatements(JackTokenizer &jt, CompilationEngine &compiler);
	bool compileClassVarDec(JackTokenizer &jt, CompilationEngine &compiler);
	bool compileVarDec(JackTokenizer &jt, CompilationEngine &compiler);
	bool compileClassName(JackTokenizer &jt, CompilationEngine &compiler);
	bool compileType(JackTokenizer &jt, CompilationEngine &compiler);
	bool compileSubroutineName(JackTokenizer &jt, CompilationEngine &compiler);
	bool compileVarName(JackTokenizer &jt, CompilationEngine &compiler);
	bool compileParameterList(JackTokenizer &jt, CompilationEngine &compiler);
	bool compileSubroutineBody(JackTokenizer &jt, CompilationEngine &compiler);
	bool compileSubroutineDec(JackTokenizer &jt, CompilationEngine &compiler);

public:
	bool compile(JackTokenizer &jt, CompilationEngine &compiler);
};
