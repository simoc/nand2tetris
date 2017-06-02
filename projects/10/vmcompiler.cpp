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

	m_labelCounter = 0;

	m_atClass = false;
	m_atSubroutineType = false;
	m_atSubroutineName = false;
	m_atSubroutineStatements = false;
	m_atLetLeftHandSide = false;
	m_atIfStatement = false;
	m_atWhileStatement = false;
	m_inVarDec = false;
	m_inClassVarDec = false;
	m_inParameterList = false;
	m_inReturnStatement = false;
	m_hasReturnExpression = false;

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
	size_t dotIndex = functionName.find_first_of('.');
	if (dotIndex == std::string::npos)
	{
		/*
		 * This is a method call on current object.
		 * Pass 'this' on stack.
		 */
		os << "push pointer 0" << std::endl;
		argCount++;

		functionName = m_className + "." +  functionName;
	}
	else
	{
		int kind = m_symbolTable.kindOf(functionName.substr(0, dotIndex));
		if (kind != -1)
		{
			int index = m_symbolTable.indexOf(functionName.substr(0, dotIndex));
			std::string typeOf = m_symbolTable.typeOf(functionName.substr(0, dotIndex));
			functionName = typeOf + functionName.substr(dotIndex);

			if (kind == JackTokenizer::K_VAR)
				os << "push local " << index << std::endl;
			else if (kind == JackTokenizer::K_ARG)
				os << "push arg " << index << std::endl;
			else if (kind == JackTokenizer::K_FIELD)
				os << "push this " << index << std::endl;
			else if (kind == JackTokenizer::K_STATIC)
				os << "push static " << index << std::endl;
			argCount++;
		}
	}
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
	m_inVarDec = true;
}

void
VmCompiler::endCompileVarDec()
{
	m_inVarDec = false;
}

void
VmCompiler::beginCompileStatements()
{
	if (m_atSubroutineStatements)
	{
		int32_t varCount = m_symbolTable.varCount(JackTokenizer::K_VAR);
		m_vmFs << "function ";
		if (m_subroutineType == JackTokenizer::K_METHOD)
		{
			/*
			 * 'this' object pointer will also be passed on stack.
			 */
			varCount++;
		}

		m_vmFs << m_className << "." << m_functionName <<
			" " << varCount << std::endl;

		if (m_subroutineType == JackTokenizer::K_METHOD)
		{
			/*
			 * Set 'this' pointer.
			 */
			m_vmFs << "push argument 0" << std::endl;
			m_vmFs << "pop pointer 0" << std::endl;
		}
		else if (m_subroutineType == JackTokenizer::K_CONSTRUCTOR)
		{
			/*
			 * Defining a constructor.
			 * Allocate enough memory to hold all fields of new object
			 * and store address in 'this'.
			 */
			int32_t byteCount =
				m_symbolTable.varCount(JackTokenizer::K_FIELD) * 2;
			m_vmFs << "push constant " << byteCount << std::endl;
			m_vmFs << "call Memory.alloc 1" << std::endl;
			m_vmFs << "pop pointer 0" << std::endl;
		}
		m_atSubroutineStatements = false;
	}
	else if (m_atWhileStatement)
	{
		m_vmFs << m_expressionStack.back() << std::endl;
		m_vmFs << "not" << std::endl;
		m_vmFs << "if-goto endloop" << m_loopStack.back() << std::endl;

		m_atWhileStatement = false;
	}
	else if (m_atIfStatement)
	{
		m_vmFs << m_expressionStack.back() << std::endl;
		m_vmFs << "not" << std::endl;
		m_vmFs << "if-goto else" << m_ifStack.back() << std::endl;

		m_atIfStatement = false;
	}
}

void
VmCompiler::endCompileStatements()
{
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

	/*
	 * Discard unwanted return value, as described on page 235.
	 */
	m_vmFs << "pop temp 0" << std::endl;
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

	m_labelCounter++;
	m_vmFs << "label loop" << m_labelCounter << std::endl;

	m_loopStack.push_back(m_labelCounter);
	m_atWhileStatement = true;
}

void
VmCompiler::endCompileWhile()
{
	m_vmFs << "// end while" << std::endl;

	m_vmFs << "goto loop" << m_loopStack.back() << std::endl;
	m_vmFs << "label endloop" << m_loopStack.back() << std::endl;
	m_loopStack.pop_back();
}

void
VmCompiler::beginCompileReturnStatement()
{
	m_vmFs << "// beginCompileReturnStatement" << std::endl;
	m_inReturnStatement = true;
	m_hasReturnExpression = false;
}

void
VmCompiler::endCompileReturnStatement()
{
	m_inReturnStatement = false;
	if (m_hasReturnExpression)
	{
		m_vmFs << m_expressionStack.back() << std::endl;
		m_expressionStack.pop_back();
	}
	else
	{
		m_vmFs << "push constant 0" << std::endl;
	}
	m_vmFs << "return" << std::endl;
}

void
VmCompiler::beginCompileIf()
{
	m_vmFs << "// if" << std::endl;
	m_labelCounter++;
	m_ifStack.push_back(m_labelCounter);
	m_atIfStatement = true;
}

void
VmCompiler::endCompileIf()
{
	m_vmFs << "// end if" << std::endl;

	/*
	 * Add a label to jump to if there was no 'else' clause.
	 */
	if (m_elseStack.size() == 0)
		m_vmFs << "label else" << m_ifStack.back() << std::endl;
	else if (m_elseStack.back() != m_ifStack.back())
		m_vmFs << "label else" << m_ifStack.back() << std::endl;
	else
		m_elseStack.pop_back();

	m_vmFs << "label endif" << m_ifStack.back() << std::endl;
	m_ifStack.pop_back();
}

void
VmCompiler::beginCompileExpression()
{
	m_expressionStack.push_back("beginCompileExpression");
	printExpressionStack();

	if (m_inReturnStatement)
		m_hasReturnExpression = true;
}

void
VmCompiler::endCompileExpression()
{
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
		m_functionName = identifier;
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
			std::ostringstream os;
			os << direction << " local " << index;
			m_expressionStack.push_back(os.str());
			evaluateExpressionStack();
			printExpressionStack();
		}
		else if (kind == JackTokenizer::K_FIELD)
		{
			int32_t index = m_symbolTable.indexOf(identifier);
			std::ostringstream os;
			os << direction << " this " << index;
			m_expressionStack.push_back(os.str());
			evaluateExpressionStack();
			printExpressionStack();
		}
		else if (kind == JackTokenizer::K_ARG)
		{
			int32_t index = m_symbolTable.indexOf(identifier);
			if (m_subroutineType == JackTokenizer::K_METHOD)
			{
				/*
				 * 'this' object reference is passed on stack
				 * before method arguments.
				 */
				index++;
			}
			std::ostringstream os;
			os << direction << " argument " << index;
			m_expressionStack.push_back(os.str());
			evaluateExpressionStack();
			printExpressionStack();
		}
		else
		{
			m_identifier += identifier;
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
	else if (keyword == JackTokenizer::K_ELSE)
	{
		m_vmFs << "goto endif" << m_ifStack.back() << std::endl;
		m_vmFs << "label else" << m_ifStack.back() << std::endl;
		m_elseStack.push_back(m_ifStack.back());
	}
	else if (keyword == JackTokenizer::K_THIS)
	{
		m_expressionStack.push_back("push pointer 0");
		evaluateExpressionStack();
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
	else if (term == '&')
	{
		m_expressionStack.push_back("and");
		printExpressionStack();
	}
	else if (term == '|')
	{
		m_expressionStack.push_back("or");
		printExpressionStack();
	}
	else if (term == ',')
	{
		m_vmFs << "// ," << std::endl;
	}
	else if (term == '~')
	{
		m_expressionStack.push_back("not");
		printExpressionStack();
		evaluateExpressionStack();
		printExpressionStack();
	}
	else if (term == '=')
	{
		if (!m_atLetLeftHandSide)
			m_expressionStack.push_back("eq");

		m_atLetLeftHandSide = false;
		m_vmFs << "// =" << std::endl;
	}
	else if (term == '>')
	{
		m_expressionStack.push_back("gt");
	}
	else if (term == '<')
	{
		m_expressionStack.push_back("lt");
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
		m_functionName = term;
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
		m_identifier += term;
	}
}

void
VmCompiler::compileStringConst(const std::string &value)
{
	std::ostringstream os;

	m_vmFs << "// \"" << value << "\"" << std::endl;

	/*
	 * Allocate memory for string.
	 * Leave pointer to String on stack and add each character to string.
	 */
	os << "push constant " << value.size() << std::endl;
	os << "call String.new 1" << std::endl;

	for (unsigned int i = 0; i < value.size(); i++)
	{
		os << "push constant " << int32_t(value.at(i)) << std::endl;
		os << "call String.appendChar 2";
		if (i < value.size() - 1)
			os << std::endl;
	}
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
	if (m_expressionStack.size() > 1 &&
		m_expressionStack.at(m_expressionStack.size() - 2) == "not")
	{
		/*
		 * Replace top two elements <not> <expr> with single <expr not>.
		 */
		std::string expression = m_expressionStack.back();
		m_expressionStack.pop_back();
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
		std::string op = m_expressionStack.at(m_expressionStack.size() - 2);

		if (op == "sub" &&
			m_expressionStack.at(m_expressionStack.size() - 3) == "beginCompileExpression")
		{
			/*
			 * We now know that we have a negative number.
			 * Replace top two elements <sub> <expr2>
			 * with single <expr neg>.
			 */
			std::string expression = m_expressionStack.back();
			m_expressionStack.pop_back();
			m_expressionStack.pop_back();

			std::ostringstream os;
			os << expression << std::endl;
			os << "neg";
			m_expressionStack.push_back(os.str());
		}
		else if (op == "add" || op == "sub" ||
			op == "and" || op == "or" ||
			op == "lt" || op == "eq" || op == "gt" ||
			op == "call Math.multiply 2" || op == "call Math.divide 2")
		{
			/*
			 * Replace top three elements <expr1> <op> <expr2>
			 * with single <expr1 expr2 op>.
			 */
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
