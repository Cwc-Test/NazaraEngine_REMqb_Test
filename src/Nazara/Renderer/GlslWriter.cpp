// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/GlslWriter.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	GlslWriter::GlslWriter() :
	m_currentState(nullptr)
	{
	}

	String GlslWriter::Generate(const ShaderAst::StatementPtr& node)
	{
		State state;
		m_currentState = &state;
		CallOnExit onExit([this]()
		{
			m_currentState = nullptr;
		});

		node->Register(*this);

		for (const auto& pair : state.m_variables)
		{
			Append(pair.first);
			Append(" ");
			Append(pair.second);
			AppendLine(";");
		}

		AppendLine();

		Function entryPoint;
		entryPoint.name = "main"; //< GLSL has only one entry point name possible
		entryPoint.node = node;
		entryPoint.retType = ShaderAst::ExpressionType::None;

		AppendFunction(entryPoint);

		return state.stream;
	}

	void GlslWriter::RegisterFunction(const String& name, ShaderAst::StatementPtr statement, std::initializer_list<ShaderAst::VariablePtr> parameters, ShaderAst::ExpressionType retType)
	{
		Function func;
		func.retType = retType;
		func.name = name;
		func.node = std::move(statement);
		func.parameters.assign(parameters);

		m_functions[name] = std::move(func);
	}

	void GlslWriter::RegisterVariable(const String& name, ShaderAst::ExpressionType type)
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		m_currentState->m_variables.insert(std::make_pair(type, name));
	}

	void GlslWriter::Write(const ShaderAst::NodePtr& node)
	{
		node->Visit(*this);
	}

	void GlslWriter::Write(const ShaderAst::AssignOp& node)
	{
		Write(node.variable);

		switch (node.op)
		{
			case ShaderAst::AssignType::Simple:
				Append(" = ");
				break;
		}

		Write(node.right);
	}

	void GlslWriter::Write(const ShaderAst::Branch& node)
	{
		bool first = true;
		for (const auto& statement : node.condStatements)
		{
			if (!first)
				Append("else ");

			Append("if (");
			Write(statement.condition);
			AppendLine(")");

			EnterScope();
				Write(statement.statement);
			LeaveScope();

			first = false;
		}

		if (node.elseStatement)
		{
			AppendLine("else");

			EnterScope();
				Write(node.elseStatement);
			LeaveScope();
		}
	}

	void GlslWriter::Write(const ShaderAst::BinaryOp& node)
	{
		Write(node.left);

		switch (node.op)
		{
			case ShaderAst::BinaryType::Add:
				Append(" + ");
				break;
			case ShaderAst::BinaryType::Substract:
				Append(" - ");
				break;
			case ShaderAst::BinaryType::Multiply:
				Append(" * ");
				break;
			case ShaderAst::BinaryType::Divide:
				Append(" / ");
				break;
			case ShaderAst::BinaryType::Equality:
				Append(" == ");
				break;
		}

		Write(node.right);
	}

	void GlslWriter::Write(const ShaderAst::Constant& node)
	{
		switch (node.exprType)
		{
			case ShaderAst::ExpressionType::Float1:
				Append(String::Format("%F", node.values.vec1));
				break;
		}
	}

	void GlslWriter::Write(const ShaderAst::ExpressionStatement& node)
	{
		Write(node.expression);
		Append(";");
	}

	void GlslWriter::Write(const ShaderAst::StatementBlock& node)
	{
		bool first = true;
		for (const ShaderAst::StatementPtr& statement : node.statements)
		{
			if (!first)
				AppendLine();

			Write(statement);

			first = false;
		}
	}

	void GlslWriter::Write(const ShaderAst::Variable& node)
	{
		Append(node.name);
	}

	void GlslWriter::Append(ShaderAst::ExpressionType type)
	{
		switch (type)
		{
			case ShaderAst::ExpressionType::Float1:
				Append("float");
				break;
			case ShaderAst::ExpressionType::Float2:
				Append("vec2");
				break;
			case ShaderAst::ExpressionType::Float3:
				Append("vec3");
				break;
			case ShaderAst::ExpressionType::Float4:
				Append("vec4");
				break;
			case ShaderAst::ExpressionType::None:
				Append("void");
				break;
		}
	}

	void GlslWriter::Append(const String& txt)
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		m_currentState->stream << txt;
	}

	void GlslWriter::AppendFunction(const Function& func)
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		Append(func.retType);

		m_currentState->stream << ' ';
		Append(func.name);

		m_currentState->stream << '(';
		for (std::size_t i = 0; i < func.parameters.size(); ++i)
		{
			if (i != 0)
				m_currentState->stream << ", ";

			Append(func.parameters[i]->type);
			m_currentState->stream << ' ';
			Append(func.parameters[i]->name);
		}
		m_currentState->stream << ")\n";

		EnterScope();
			Write(func.node);
		LeaveScope();
	}

	void GlslWriter::AppendLine(const String& txt)
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		m_currentState->stream << txt << '\n' << String(m_currentState->indentLevel, '\t');
	}

	void GlslWriter::EnterScope()
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		m_currentState->indentLevel++;
		AppendLine("{");
	}

	void GlslWriter::LeaveScope()
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		m_currentState->indentLevel--;
		AppendLine();
		AppendLine("}");
	}

}
