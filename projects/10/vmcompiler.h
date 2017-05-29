#pragma once

#include "compilationengine.h"
#include "symboltable.h"
#include <iostream>
#include <fstream>
#include <vector>

class VmCompiler : public CompilationEngine
{
public:
	VmCompiler(const std::string &filename);
	~VmCompiler();

	void beginCompileClass();

	void endCompileClass();

	void beginCompileClassVarDec();

	void endCompileClassVarDec();

	void beginCompileSubroutineDec(JackTokenizer::KEYWORD_TYPE keyword);

	void endCompileSubroutineDec();

	void beginCompileParameterList();

	void endCompileParameterList();

	void beginCompileSubroutineBody();

	void endCompileSubroutineBody();

	void beginCompileVarDec();

	void endCompileVarDec();

	void beginCompileStatements();

	void endCompileStatements();

	void beginCompileDo();

	void endCompileDo();

	void beginCompileLet();

	void endCompileLet();

	void beginCompileWhile();

	void endCompileWhile();

	void beginCompileReturnStatement();

	void endCompileReturnStatement();

	void beginCompileIf();

	void endCompileIf();

	void beginCompileExpression();

	void endCompileExpression();

	void beginCompileExpressionList();

	void endCompileExpressionList();

	void compileIdentifier(const std::string &identifier);

	void compileTerm(JackTokenizer::KEYWORD_TYPE keyword);

	void beginCompileTerm();

	void endCompileTerm();

	void compileTerm(char term);

	void compileTerm(const std::string &term);

	void compileStringConst(const std::string &value);

	void compileIntConst(int32_t value);

private:
	void evaluateExpressionStack();
	void printExpressionStack();

	std::ofstream m_vmFs;

	int32_t m_labelCounter;

	bool m_atClass;
	bool m_atSubroutineType;
	bool m_atSubroutineName;
	bool m_atSubroutineStatements;
	bool m_atLetLeftHandSide;
	bool m_atIfStatement;
	bool m_atWhileStatement;
	bool m_inVarDec;
	bool m_inClassVarDec;
	bool m_inParameterList;
	bool m_inReturnStatement;
	bool m_hasReturnExpression;

	JackTokenizer::KEYWORD_TYPE m_subroutineType;
	std::string m_className;
	std::string m_methodName;
	std::string m_varType;
	std::string m_classVarType;
	std::string m_subroutineVarType;
	std::string m_identifier;
	std::vector<std::string> m_expressionStack;
	std::vector<int32_t> m_ifStack;
	std::vector<int32_t> m_elseStack;
	std::vector<int32_t> m_loopStack;
	SymbolTable m_symbolTable;
};
