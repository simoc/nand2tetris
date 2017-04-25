#include "xmlcompiler.h"

XmlCompiler::XmlCompiler(const std::string &filename)
{
	m_xmlFs.open(filename.c_str());
	m_indent = 0;
	m_inParameterList = false;
	m_inVarDec = false;
	m_inReturn = false;
}

XmlCompiler::~XmlCompiler()
{
	m_xmlFs.close();
}

void
XmlCompiler::indent()
{
	for (int i = 0; i < m_indent; i++)
	{
		m_xmlFs << " ";
	}
}

void
XmlCompiler::beginCompileClass()
{
	indent();
	m_xmlFs << "<class>" << std::endl;
	m_indent += 2;
}

void
XmlCompiler::endCompileClass()
{
	m_indent -= 2;
	indent();
	m_xmlFs << "</class>" << std::endl;
}

void
XmlCompiler::beginCompileClassVarDec()
{
	indent();
	m_xmlFs << "<classVarDec>" << std::endl;
	m_indent += 2;
}

void
XmlCompiler::endCompileClassVarDec()
{
	m_indent -= 2;
	indent();
	m_xmlFs << "</classVarDec>" << std::endl;
}

void
XmlCompiler::beginCompileSubroutineDec(JackTokenizer::KEYWORD_TYPE keyword)
{
	std::string type;

	switch (keyword)
	{
	case JackTokenizer::K_METHOD:
		type = "method";
		break;
	case JackTokenizer::K_FUNCTION:
		type = "function";
		break;
	case JackTokenizer::K_CONSTRUCTOR:
		type = "constructor";
		break;
	default:
		type = "";
	}

	indent();
	m_xmlFs << "<subroutineDec>" << std::endl;
	m_indent += 2;
	indent();
	m_xmlFs << "<keyword> " << type << " </keyword>" << std::endl;
}

void
XmlCompiler::endCompileSubroutineDec()
{
	m_indent -= 2;
	indent();
	m_xmlFs << "</subroutineDec>" << std::endl;
}

void
XmlCompiler::beginCompileSubroutineBody()
{
	indent();
	m_xmlFs << "<subroutineBody>" << std::endl;
	m_indent += 2;
}

void
XmlCompiler::endCompileSubroutineBody()
{
	m_indent -= 2;
	indent();
	m_xmlFs << "</subroutineBody>" << std::endl;
}

void
XmlCompiler::compileParameterList()
{
	indent();
	m_xmlFs << "<parameterList>" << std::endl;
	m_indent += 2;
	m_inParameterList = true;
}

void
XmlCompiler::beginCompileExpressionList()
{
	indent();
	m_xmlFs << "<expressionList>" << std::endl;
	m_indent += 2;
}

void
XmlCompiler::endCompileExpressionList()
{
	m_indent -= 2;
	indent();
	m_xmlFs << "</expressionList>" << std::endl;
}

void
XmlCompiler::compileVarDec()
{
	indent();
	m_xmlFs << "<varDec>" << std::endl;
	m_indent += 2;
	m_inVarDec = true;
}

void
XmlCompiler::beginCompileStatements()
{
	indent();
	m_xmlFs << "<statements>" << std::endl;
	m_indent += 2;
}

void
XmlCompiler::endCompileStatements()
{
	m_indent -= 2;
	indent();
	m_xmlFs << "</statements>" << std::endl;
}

void
XmlCompiler::beginCompileDo()
{
	indent();
	m_xmlFs << "<doStatement>" << std::endl;
	m_indent += 2;
}

void
XmlCompiler::endCompileDo()
{
	m_indent -= 2;
	indent();
	m_xmlFs << "</doStatement>" << std::endl;
}

void
XmlCompiler::beginCompileLet()
{
	indent();
	m_xmlFs << "<letStatement>" << std::endl;
	m_indent += 2;
}

void
XmlCompiler::endCompileLet()
{
	m_indent -= 2;
	indent();
	m_xmlFs << "</letStatement>" << std::endl;
}

void
XmlCompiler::beginCompileWhile()
{
	indent();
	m_xmlFs << "<whileStatement>" << std::endl;
	m_indent += 2;
}

void
XmlCompiler::endCompileWhile()
{
	m_indent -= 2;
	indent();
	m_xmlFs << "</whileStatement>" << std::endl;
}

void
XmlCompiler::compileReturnStatement()
{
	indent();
	m_xmlFs << "<returnStatement>" << std::endl;
	m_indent += 2;
	m_inReturn = true;
}

void
XmlCompiler::beginCompileIf()
{
	indent();
	m_xmlFs << "<ifStatement>" << std::endl;
	m_indent += 2;
}

void
XmlCompiler::endCompileIf()
{
	m_indent -= 2;
	indent();
	m_xmlFs << "</ifStatement>" << std::endl;
}

void
XmlCompiler::beginCompileExpression()
{
	indent();
	m_xmlFs << "<expression>" << std::endl;
	m_indent += 2;
}

void
XmlCompiler::endCompileExpression()
{
	m_indent -= 2;
	indent();
	m_xmlFs << "</expression>" << std::endl;
}

void
XmlCompiler::compileIdentifier(const std::string &identifier)
{
	indent();
	m_xmlFs << "<identifier> " << identifier << " </identifier>" << std::endl;
}

void
XmlCompiler::beginCompileTerm()
{
	indent();
	m_xmlFs << "<term>" << std::endl;
	m_indent += 2;
}

void
XmlCompiler::endCompileTerm()
{
	m_indent -= 2;
	indent();
	m_xmlFs << "</term>" << std::endl;
}

void
XmlCompiler::compileTerm(JackTokenizer::KEYWORD_TYPE keyword)
{
	indent();
	if (keyword == JackTokenizer::K_CLASS)
		m_xmlFs << "<keyword> class </keyword>" << std::endl;
	else if (keyword == JackTokenizer::K_METHOD)
		m_xmlFs << "<keyword> method </keyword>" << std::endl;
	else if (keyword == JackTokenizer::K_FUNCTION)
		m_xmlFs << "<keyword> function </keyword>" << std::endl;
	else if (keyword == JackTokenizer::K_INT)
		m_xmlFs << "<keyword> int </keyword>" << std::endl;
	else if (keyword == JackTokenizer::K_BOOLEAN)
		m_xmlFs << "<keyword> boolean </keyword>" << std::endl;
	else if (keyword == JackTokenizer::K_CHAR)
		m_xmlFs << "<keyword> char </keyword>" << std::endl;
	else if (keyword == JackTokenizer::K_VOID)
		m_xmlFs << "<keyword> void </keyword>" << std::endl;
	else if (keyword == JackTokenizer::K_VAR)
		m_xmlFs << "<keyword> var </keyword>" << std::endl;
	else if (keyword == JackTokenizer::K_STATIC)
		m_xmlFs << "<keyword> static </keyword>" << std::endl;
	else if (keyword == JackTokenizer::K_FIELD)
		m_xmlFs << "<keyword> field </keyword>" << std::endl;
	else if (keyword == JackTokenizer::K_LET)
		m_xmlFs << "<keyword> let </keyword>" << std::endl;
	else if (keyword == JackTokenizer::K_DO)
		m_xmlFs << "<keyword> do </keyword>" << std::endl;
	else if (keyword == JackTokenizer::K_IF)
		m_xmlFs << "<keyword> if </keyword>" << std::endl;
	else if (keyword == JackTokenizer::K_ELSE)
		m_xmlFs << "<keyword> else </keyword>" << std::endl;
	else if (keyword == JackTokenizer::K_WHILE)
		m_xmlFs << "<keyword> while </keyword>" << std::endl;
	else if (keyword == JackTokenizer::K_RETURN)
		m_xmlFs << "<keyword> return </keyword>" << std::endl;
	else if (keyword == JackTokenizer::K_TRUE)
		m_xmlFs << "<keyword> true </keyword>" << std::endl;
	else if (keyword == JackTokenizer::K_FALSE)
		m_xmlFs << "<keyword> false </keyword>" << std::endl;
	else if (keyword == JackTokenizer::K_NULL)
		m_xmlFs << "<keyword> null </keyword>" << std::endl;
	else if (keyword == JackTokenizer::K_THIS)
		m_xmlFs << "<keyword> this </keyword>" << std::endl;
}

void
XmlCompiler::compileTerm(char term)
{
	if (term == ')')
	{
		if (m_inParameterList)
		{
			m_indent -= 2;
			indent();
			m_xmlFs << "</parameterList>" << std::endl;
			m_inParameterList = false;
		}
	}

	indent();
	m_xmlFs << "<symbol> ";

	/*
	 * Protect against XML special characters.
	 */
	if (term == '<')
		m_xmlFs << "&lt;";
	else if (term == '>')
		m_xmlFs << "&gt;";
	else if (term == '&')
		m_xmlFs << "&amp;";
	else
		m_xmlFs << term;
	m_xmlFs << " </symbol> " << std::endl;

	if (term == ';')
	{
		if (m_inVarDec)
		{
			m_indent -= 2;
			indent();
			m_xmlFs << "</varDec>" << std::endl;
			m_inVarDec = false;
		}
		if (m_inReturn)
		{
			m_indent -= 2;
			indent();
			m_xmlFs << "</returnStatement>" << std::endl;
			m_inReturn = false;
		}
	}
}

void
XmlCompiler::compileTerm(const std::string &term)
{
	indent();
	m_xmlFs << "<identifier> " << term << " </identifier>" << std::endl;
}

void
XmlCompiler::compileStringConst(const std::string &value)
{
	indent();
	m_xmlFs << "<stringConstant> " << value << " </stringConstant>" << std::endl;
}

void
XmlCompiler::compileIntConst(int32_t value)
{
	indent();
	m_xmlFs << "<integerConstant> " << value << " </integerConstant>" << std::endl;
}
