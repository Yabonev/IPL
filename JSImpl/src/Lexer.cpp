#include "Lexer.h"

class Tokenizer
{
public:
	Tokenizer(const char* code, const std::function<void()>& onError);
	IPLVector<Token> Tokenize();

private:
	Token NextToken();

	Token ProduceToken(TokenType type);
	Token ProduceToken(TokenType type, IPLString lexeme);
	double ParseNumber();
	IPLString ParseString();
	bool SkipWhiteSpaces();
	bool SkipNewLine();
	bool Match(const char c);

	std::function<void()> OnError;
	unsigned m_Line;
	unsigned m_Current;
	const char* m_Code;

	IPLUnorderedMap<IPLString, TokenType> m_KeyWordsTable;
};

IPLVector<Token> Tokenize(const char* code, const std::function<void()>& onError)
{
	Tokenizer tokenizer(code, onError);

	return tokenizer.Tokenize();
}

Tokenizer::Tokenizer(const char* code, const std::function<void()>& onError)
	: OnError(onError)
	, m_Line(0)
	, m_Current(0)
	, m_Code(code)
{
	m_KeyWordsTable["break"] = TokenType::Break;
	m_KeyWordsTable["case"] = TokenType::Case;
	m_KeyWordsTable["catch"] = TokenType::Catch;
	m_KeyWordsTable["class"] = TokenType::Class;
	m_KeyWordsTable["const"] = TokenType::Const;
	m_KeyWordsTable["continue"] = TokenType::Continue;
	m_KeyWordsTable["debugger"] = TokenType::Debugger;
	m_KeyWordsTable["default"] = TokenType::Default;
	m_KeyWordsTable["delete"] = TokenType::Delete;
	m_KeyWordsTable["do"] = TokenType::Do;
	m_KeyWordsTable["else"] = TokenType::Else;
	m_KeyWordsTable["export"] = TokenType::Export;
	m_KeyWordsTable["extends"] = TokenType::Extends;
	m_KeyWordsTable["finally"] = TokenType::Finally;
	m_KeyWordsTable["for"] = TokenType::For;
	m_KeyWordsTable["function"] = TokenType::Function;
	m_KeyWordsTable["if"] = TokenType::If;
	m_KeyWordsTable["import"] = TokenType::Import;
	m_KeyWordsTable["in"] = TokenType::In;
	m_KeyWordsTable["instanceof"] = TokenType::Instanceof;
	m_KeyWordsTable["new"] = TokenType::New;
	m_KeyWordsTable["return"] = TokenType::Return;
	m_KeyWordsTable["super"] = TokenType::Super;
	m_KeyWordsTable["switch"] = TokenType::Switch;
	m_KeyWordsTable["this"] = TokenType::This;
	m_KeyWordsTable["throw"] = TokenType::Throw;
	m_KeyWordsTable["try"] = TokenType::Try;
	m_KeyWordsTable["typeof"] = TokenType::Typeof;
	m_KeyWordsTable["var"] = TokenType::Var;
	m_KeyWordsTable["void"] = TokenType::Void;
	m_KeyWordsTable["while"] = TokenType::While;
	m_KeyWordsTable["with"] = TokenType::With;
	m_KeyWordsTable["yield"] = TokenType::Yield;
}

IPLVector<Token> Tokenizer::Tokenize()
{
	IPLVector<Token> result;
	do
	{
		result.push_back(NextToken());
	} while (result.back().Type != TokenType::Eof);

	return result;
}

bool Tokenizer::SkipWhiteSpaces()
{
	const auto old = m_Current;
	while (m_Code[m_Current] == ' ' || m_Code[m_Current] == '\t')
	{
		m_Current++;
	}
	return old != m_Current;
}

bool Tokenizer::SkipNewLine()
{
	const auto old = m_Current;
	while (m_Code[m_Current] == '\n')
	{
		m_Current++;
		m_Line++;
	}
	return old != m_Current;
}

bool Tokenizer::Match(const char c)
{
	if (c == m_Code[m_Current])
	{
		++m_Current;
		return true;
	}
	return false;
}

Token Tokenizer::ProduceToken(TokenType type)
{
	return { type, m_Line, "" };
}

Token Tokenizer::ProduceToken(TokenType type, IPLString lexeme)
{
	return{ type, m_Line, lexeme };
}

Token Tokenizer::NextToken()
{
	if (m_Code[m_Current] == '\0')
	{
		return Token{ TokenType::Eof, 0, "" };
	}

	while (SkipWhiteSpaces() || SkipNewLine());

	// Single Char
	switch (m_Code[m_Current])
	{
	case '(': m_Current++; return ProduceToken(TokenType::LeftParen);
	case ')': m_Current++; return ProduceToken(TokenType::RightParen);
	case '{': m_Current++; return ProduceToken(TokenType::LeftBrace);
	case '}': m_Current++; return ProduceToken(TokenType::RightBrace);
	case ',': m_Current++; return ProduceToken(TokenType::Comma);
	case '.': m_Current++; return ProduceToken(TokenType::Dot);
	case '-': m_Current++; return ProduceToken(TokenType::Minums);
	case '+': m_Current++; return ProduceToken(TokenType::Plus);
	case ';': m_Current++; return ProduceToken(TokenType::Semicolon);
	case '*': m_Current++; return ProduceToken(TokenType::Star);
	case '=': m_Current++; return Match('=') ? ProduceToken(TokenType::EqualEqual) : ProduceToken(TokenType::Equal);
	case '!': m_Current++; return Match('=') ? ProduceToken(TokenType::BangEqual) : ProduceToken(TokenType::Bang);
	case '>': m_Current++; return Match('=') ? ProduceToken(TokenType::GreaterEqual) : ProduceToken(TokenType::Greater);
	case '<': m_Current++; return Match('=') ? ProduceToken(TokenType::LessEqual) : ProduceToken(TokenType::Less);
	default:
		break;
	}
	// Number
	const char c = m_Code[m_Current];
	if (c >= '0' && c < '9')
	{
		size_t parsedBytes = 0;
		const double number = std::stod(m_Code + m_Current, &parsedBytes);
		m_Current += (unsigned)parsedBytes;
		return Token{ TokenType::Number, m_Line, "", number };
	}

	// String
	if (c == '"')
	{
		auto start = m_Current;
		m_Current++; // skip first '"'
		while (m_Code[m_Current] != '"')
		{
			++m_Current;
		}
		m_Current++; // skip second '"'
		return Token{ TokenType::String, m_Line, IPLString(m_Code + start, m_Code + m_Current) };
	}

	// Key words
	if (c >= 'a' && c <= 'z')
	{
		auto start = m_Current;
		m_Current++;
		while (m_Code[m_Current] >= 'a' && m_Code[m_Current] <= 'z')
		{
			++m_Current;
		}
		auto key = IPLString(m_Code + start, m_Code + m_Current);
		auto word = m_KeyWordsTable.find(key);
		if (word != m_KeyWordsTable.end())
		{
			return Token{ word->second, m_Line, key };
		}

	}
	return Token{ TokenType::Eof, m_Line, "" };
}