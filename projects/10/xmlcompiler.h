#pragma once

#include "compilationengine.h"
#include <iostream>
#include <fstream>

class XmlCompiler : public CompilationEngine
{
public:
	XmlCompiler(const std::string &filename);
	~XmlCompiler();

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
	void indent();

	std::ofstream m_xmlFs;

	int m_indent;

};
