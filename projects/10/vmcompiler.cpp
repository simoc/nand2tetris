#include <sstream>

#include "vmcompiler.h"

VmCompiler::VmCompiler(const std::string &filename)
{
	/*
	 * Replace .jack suffix with .vm.
	 */
	std::string vmFilename = filename;
	size_t dotIndex = vmFilename.find_last_of('.');
	if (dotIndex != std::string::npos)
		vmFilename = vmFilename.substr(0, dotIndex);
	vmFilename += ".vm";

	m_vmFs.open(vmFilename.c_str());

	m_atClass = false;
	m_atSubroutineType = false;
	m_atSubroutineName = false;
	m_atSubroutineStatements = false;
	m_atLetLeftHandSide = false;
	m_inVarDec = false;
	m_inClassVarDec = false;
	m_inParameterList = false;
	m_inReturnStatement = false;

	m_subroutineType = JackTokenizer::K_VOID;
}

VmCompiler::~VmCompiler()
{
	m_vmFs.close();
}

void
VmCompiler::beginCompileClass()
{
}

void
VmCompiler::endCompileClass()
{
}

void
VmCompiler::beginCompileClassVarDec()
{
	m_inClassVarDec = true;
}

void
VmCompiler::endCompileClassVarDec()
{
	m_inClassVarDec = false;
}

void
VmCompiler::beginCompileSubroutineDec(JackTokenizer::KEYWORD_TYPE keyword)
{
	m_symbolTable.startSubroutine();

	m_atSubroutineType = true;
	m_subroutineType = keyword;
	m_vmFs << "// beginCompileSubroutineDec" << std::endl;
}

void
VmCompiler::endCompileSubroutineDec()
{
}

void
VmCompiler::beginCompileSubroutineBody()
{
}

void
VmCompiler::endCompileSubroutineBody()
{
}

void
VmCompiler::beginCompileParameterList()
{
	std::string openParens = "(";

	if (m_expressionStack.back() == openParens)
		m_expressionStack.pop_back();

	m_vmFs << "// beginCompileParameterList" << std::endl;
	m_inParameterList = true;
}

void
VmCompiler::endCompileParameterList()
{
	m_vmFs << "// endCompileParameterList" << std::endl;
	m_expressionStack.push_back("endCompileParameterList");
	m_inParameterList = false;
}

void
VmCompiler::beginCompileExpressionList()
{
	std::string openParens = "(";

	if (m_expressionStack.back() == openParens)
		m_expressionStack.pop_back();

	m_expressionStack.push_back(m_identifier);
	m_identifier.clear();
	m_expressionStack.push_back("beginCompileExpressionList");
	printExpressionStack();
}

void
VmCompiler::endCompileExpressionList()
{
	std::string sentinel = "beginCompileExpressionList";
	unsigned int index = m_expressionStack.size() - 1;
	while (m_expressionStack.at(index) != sentinel)
		index--;

	std::string functionName = m_expressionStack.at(index - 1);

	std::ostringstream os;
	unsigned int exprIndex = index + 1;
	int argCount = 0;
	while (exprIndex < m_expressionStack.size())
	{
		os << m_expressionStack.at(exprIndex) << std::endl;
		exprIndex++;
		argCount++;
	}
	os << "call " << functionName << " " << argCount;

	m_expressionStack.resize(index - 1);
	m_expressionStack.push_back(os.str());
	m_expressionStack.push_back("endCompileExpressionList");

	printExpressionStack();
}

void
VmCompiler::beginCompileVarDec()
{
	m_vmFs << "// beginVarDec" << std::endl;
	m_inVarDec = true;
}

void
VmCompiler::endCompileVarDec()
{
	m_vmFs << "// endVarDec" << std::endl;
	m_inVarDec = false;
}

void
VmCompiler::beginCompileStatements()
{
	m_vmFs << "// beginStatements" << std::endl;

	if (m_atSubroutineStatements)
	{
		m_vmFs << "function " << m_className <<
			"." << m_methodName <<  " " <<
			m_symbolTable.varCount(JackTokenizer::K_VAR) << std::endl;
		m_atSubroutineStatements = false;
	}
}

void
VmCompiler::endCompileStatements()
{
	m_vmFs << "// endStatements" << std::endl;
}

void
VmCompiler::beginCompileDo()
{
	m_vmFs << "// do" << std::endl;
}

void
VmCompiler::endCompileDo()
{
	std::string expression = m_expressionStack.back();
	m_expressionStack.pop_back();

	m_vmFs << "// end do" << std::endl;
	m_vmFs << expression << std::endl;
}

void
VmCompiler::beginCompileLet()
{
		m_vmFs << "// let" << std::endl;
		m_atLetLeftHandSide = true;
}

void
VmCompiler::endCompileLet()
{
	std::string rhs = m_expressionStack.back();
	m_expressionStack.pop_back();
	std::string lhs = m_expressionStack.back();
	m_expressionStack.pop_back();

	m_vmFs << "// end let" << std::endl;
	m_vmFs << rhs << std::endl;
	m_vmFs << lhs << std::endl;
}

void
VmCompiler::beginCompileWhile()
{
		m_vmFs << "// while" << std::endl;
}

void
VmCompiler::endCompileWhile()
{
}

void
VmCompiler::beginCompileReturnStatement()
{
	m_vmFs << "// beginCompileReturnStatement" << std::endl;
	m_inReturnStatement = true;
}

void
VmCompiler::endCompileReturnStatement()
{
	m_inReturnStatement = false;
	m_vmFs << "return" << std::endl;
}

void
VmCompiler::beginCompileIf()
{
		m_vmFs << "// if" << std::endl;
}

void
VmCompiler::endCompileIf()
{
		m_vmFs << "// end if" << std::endl;
}

void
VmCompiler::beginCompileExpression()
{
	m_expressionStack.push_back("beginCompileExpression");
	printExpressionStack();
}

void
VmCompiler::endCompileExpression()
{
	m_vmFs << "// endCompileExpression" << std::endl;

	/*
	 * Replace <beginCompileExpression> <expr> with just <expr>.
	 */
	std::string expression = m_expressionStack.back();
	if (expression != "beginCompileExpression")
	{
		m_expressionStack.pop_back();
		m_expressionStack.pop_back();

		m_expressionStack.push_back(expression);
	}
	printExpressionStack();
}

void
VmCompiler::compileIdentifier(const std::string &identifier)
{
	std::string direction = m_atLetLeftHandSide ? "pop" : "push";

	if (m_atClass)
	{
		m_vmFs << "// class: " << identifier << std::endl;
		m_className = identifier;
		m_atClass = false;
	}
	else if (m_atSubroutineType)
	{
		m_vmFs << "// subroutineType: " << identifier << std::endl;
		m_subroutineVarType = identifier;
		m_atSubroutineType = false;
		m_atSubroutineName = true;
	}
	else if (m_atSubroutineName)
	{
		switch (m_subroutineType)
		{
		case JackTokenizer::K_METHOD:
			m_vmFs << "// method: ";
			break;
		case JackTokenizer::K_FUNCTION:
			m_vmFs << "// function: ";
			break;
		case JackTokenizer::K_CONSTRUCTOR:
			m_vmFs << "// constructor: ";
			break;
		default:
			m_vmFs << "// ???: ";
			break;
		}
		m_vmFs << "type: " << m_subroutineVarType << ": " <<
			identifier << std::endl;
		m_methodName = identifier;
		m_atSubroutineName = false;
		m_atSubroutineStatements = true;
	}
	else if (m_inVarDec)
	{
		m_symbolTable.define(identifier, m_varType, JackTokenizer::K_VAR);
		m_vmFs << "// var: " << identifier <<
			" type: " << m_varType <<
			" index: " << m_symbolTable.indexOf(identifier) << std::endl;
	}
	else if (m_inClassVarDec)
	{
		m_symbolTable.define(identifier, m_classVarType, JackTokenizer::K_FIELD);
		m_vmFs << "// field: " << identifier <<
			" type: " << m_classVarType <<
			" index: " << m_symbolTable.indexOf(identifier) << std::endl;
	}
	else if (m_inParameterList)
	{
		m_symbolTable.define(identifier, m_varType, JackTokenizer::K_ARG);
		m_vmFs << "// arg: " << identifier <<
			" type: " << m_varType <<
			" index: " << m_symbolTable.indexOf(identifier) << std::endl;
	}
	else
	{
		int32_t kind = m_symbolTable.kindOf(identifier);
		if (kind == JackTokenizer::K_VAR)
		{
			int32_t index = m_symbolTable.indexOf(identifier);
			m_vmFs << "// var reference: " << identifier <<
				" index: " << index << std::endl;
			std::ostringstream os;
			os << direction << " local " << index;
			m_expressionStack.push_back(os.str());
			evaluateExpressionStack();
			printExpressionStack();
		}
		else if (kind == JackTokenizer::K_FIELD)
		{
			int32_t index = m_symbolTable.indexOf(identifier);
			m_vmFs << "// field reference: " << identifier <<
				" index: " << index << std::endl;
			std::ostringstream os;
			os << direction << " this " << index;
			m_expressionStack.push_back(os.str());
			evaluateExpressionStack();
			printExpressionStack();
		}
		else if (kind == JackTokenizer::K_ARG)
		{
			int32_t index = m_symbolTable.indexOf(identifier);
			m_vmFs << "// arg reference: " << identifier <<
				" index: " << index << std::endl;
			std::ostringstream os;
			os << direction << " argument " << index;
			m_expressionStack.push_back(os.str());
			evaluateExpressionStack();
			printExpressionStack();
		}
		else
		{
			m_identifier += identifier;
			m_vmFs << "// identifier reference: " << m_identifier << std::endl;
		}
	}
}

void
VmCompiler::beginCompileTerm()
{
}

void
VmCompiler::endCompileTerm()
{
}

void
VmCompiler::compileTerm(JackTokenizer::KEYWORD_TYPE keyword)
{
	if (keyword == JackTokenizer::K_INT)
	{
		m_subroutineVarType = m_varType = m_classVarType = "int";
	}
	else if (keyword == JackTokenizer::K_VOID)
	{
		m_subroutineVarType = m_varType = m_classVarType = "void";
	}
	else if (keyword == JackTokenizer::K_CLASS)
	{
		m_atClass = true;
	}
	else if (keyword == JackTokenizer::K_NULL)
	{
		m_expressionStack.push_back("push constant 0");
		evaluateExpressionStack();
		printExpressionStack();
	}
	else if (keyword == JackTokenizer::K_FALSE)
	{
		m_expressionStack.push_back("push constant 0");
		evaluateExpressionStack();
		printExpressionStack();
	}
	else if (keyword == JackTokenizer::K_TRUE)
	{
		std::ostringstream os;
		os << "push constant 1" << std::endl;
		os << "neg";
		m_expressionStack.push_back(os.str());
		evaluateExpressionStack();
		printExpressionStack();
	}

	if (m_atSubroutineType)
	{
		m_atSubroutineType = false;
		m_atSubroutineName = true;
	}
}

void
VmCompiler::compileTerm(char term)
{
	std::string sentinel1 = "endCompileExpressionList";
	std::string sentinel2 = "endCompileParameterList";

	if (term == '.')
	{
		m_identifier += term;
		m_vmFs << "// identifier reference: " << m_identifier << std::endl;
	}
	else if (term == '(')
	{
		std::string openParens;
		openParens.push_back(term);
		m_expressionStack.push_back(openParens);
		printExpressionStack();
	}
	else if (term == ')')
	{
		if (m_expressionStack.size() > 0 &&
			(m_expressionStack.back() == sentinel1 || m_expressionStack.back() == sentinel2))
		{
			/*
			 * Do nothing -- part of begin/endCompileExpressionList
			 */
			m_expressionStack.pop_back();
		}
		else
		{
			std::string closeParens;
			closeParens.push_back(term);
			m_expressionStack.push_back(closeParens);
			printExpressionStack();
			evaluateExpressionStack();
			printExpressionStack();
		}
	}
	else if (term == '+')
	{
		m_expressionStack.push_back("add");
		printExpressionStack();
	}
	else if (term == '-')
	{
		m_expressionStack.push_back("sub");
		printExpressionStack();
	}
	else if (term == '*')
	{
		m_expressionStack.push_back("call Math.multiply 2");
		printExpressionStack();
	}
	else if (term == '/')
	{
		m_expressionStack.push_back("call Math.divide 2");
		printExpressionStack();
	}
	else if (term == ',')
	{
		m_vmFs << "// ," << std::endl;
	}
	else if (term == '~')
	{
		m_expressionStack.push_back("not");
		evaluateExpressionStack();
		printExpressionStack();
	}
	else if (term == '=')
	{
		m_atLetLeftHandSide = false;
		m_vmFs << "// =" << std::endl;
	}
	else
	{
		m_vmFs << "// " << term << std::endl;
	}
}

void
VmCompiler::compileTerm(const std::string &term)
{
	if (m_atSubroutineType)
	{
		m_vmFs << "// subroutineType: " << term << std::endl;
		m_subroutineVarType = term;
		m_atSubroutineType = false;
		m_atSubroutineName = true;
	}
	else if (m_atSubroutineName)
	{
		switch (m_subroutineType)
		{
		case JackTokenizer::K_METHOD:
			m_vmFs << "// method: ";
			break;
		case JackTokenizer::K_FUNCTION:
			m_vmFs << "// function: ";
			break;
		case JackTokenizer::K_CONSTRUCTOR:
			m_vmFs << "// constructor: ";
			break;
		default:
			m_vmFs << "// ???: ";
			break;
		}
		m_vmFs << "type: " << m_subroutineVarType << ": " <<
			term << std::endl;
		m_methodName = term;
		m_atSubroutineName = false;
		m_atSubroutineStatements = true;
	}
	else if (m_inVarDec)
	{
		m_varType = term;
	}
	else if (m_inClassVarDec)
	{
		m_classVarType = term;
	}
	else
	{
		m_vmFs << "// term reference: " << term << std::endl;
		m_identifier += term;
	}
}

void
VmCompiler::compileStringConst(const std::string &value)
{
	std::ostringstream os;
	os << "push constant \"" << value << "\"";
	m_expressionStack.push_back(os.str());
	evaluateExpressionStack();
	printExpressionStack();
}

void
VmCompiler::compileIntConst(int32_t value)
{
	std::ostringstream os;
	os << "push constant " << value;
	m_expressionStack.push_back(os.str());
	evaluateExpressionStack();
	printExpressionStack();
}

void
VmCompiler::evaluateExpressionStack()
{
	if (m_expressionStack.back() == "not")
	{
		/*
		 * Replace top two elements <expr> <not> with single <expr not>.
		 */
		m_expressionStack.pop_back();
		std::string expression = m_expressionStack.back();
		m_expressionStack.pop_back();

		std::ostringstream os;
		os << expression << std::endl;
		os << "not";

		m_expressionStack.push_back(os.str());
	}
	else if (m_expressionStack.back() == "(")
	{
		/*
		 * Cannot evaluate until ')'.
		 */
	}
	else if (m_expressionStack.back() == ")")
	{
		/*
		 * Replace top three elements <(> <expr> <)>
		 * with single <expr>.
		 */
		if (m_expressionStack.size() >= 3)
		{
			m_expressionStack.pop_back();
			std::string expression = m_expressionStack.back();
			m_expressionStack.pop_back();
			m_expressionStack.pop_back();
			m_expressionStack.push_back(expression);
		}

		m_vmFs << "// ) ??" << std::endl;
		printExpressionStack();
#if 1
		evaluateExpressionStack();
		printExpressionStack();
#endif
	}
	else if (m_expressionStack.size() > 2)
	{
		/*
		 * Replace top three elements <expr1> <op> <expr2>
		 * with single <expr1 expr2 op>.
		 */
		std::string op = m_expressionStack.at(m_expressionStack.size() - 2);

		if (op == "add" || op == "sub" ||
			op == "call Math.multiply 2" || op == "call Math.divide 2")
		{
			std::string expression1 = m_expressionStack.at(m_expressionStack.size() - 3);
			std::string expression2 = m_expressionStack.back();

			m_expressionStack.pop_back();
			m_expressionStack.pop_back();
			m_expressionStack.pop_back();

			std::ostringstream os;
			os << expression1 << std::endl;
			os << expression2 << std::endl;
			os << op;

			m_expressionStack.push_back(os.str());
		}
	}
}

void
VmCompiler::printExpressionStack()
{
#if 0
	m_vmFs << "Stack:<";

	std::string separator = "";
	auto it = m_expressionStack.begin();
	while (it != m_expressionStack.end())
	{
		m_vmFs << separator << *it;
		separator = " | ";
		it++;
	}
	m_vmFs << ">" << std::endl;
#endif
}
