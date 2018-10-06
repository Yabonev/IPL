#include "Lexer.h"
#include "Parser.h"
#include <iostream>
#define LOG(msg) std::cout << msg << std::endl
#define EXECUTE_TEST(test) std::cout << #test << " "; test();

#define CHECK(cond) cond ? LOG("PASS") : LOG("FAIL")

// Lexer Tests
void TestLess()
{
	auto tokens = Tokenize("<");
	CHECK(tokens.size() == 2 && tokens[0].Type == TokenType::Less);
}

void TestNumber()
{
	auto tokens = Tokenize("213434.24");
	CHECK(tokens.size() == 2 && tokens[0].Type == TokenType::Number && tokens[0].Number == 213434.24);
}

void TestString()
{
	auto tokens = Tokenize("\"alabala\"");
	CHECK(tokens.size() == 2 && tokens[0].Type == TokenType::String && tokens[0].Lexeme == "\"alabala\"");
}

void TestKeyWord()
{
	auto tokens = Tokenize("for");
	CHECK(tokens.size() == 2 && tokens[0].Type == TokenType::For);
}

void TestVariableDeclaration()
{
	auto tokens = Tokenize("var pesho = 10");
	CHECK(tokens.size() == 5 && tokens[0].Type == TokenType::Var
		&& tokens[1].Type == TokenType::Identifier
		&& tokens[2].Type == TokenType::Equal
		&& tokens[3].Type == TokenType::Number
		&& tokens[4].Type == TokenType::Eof);
}
// Parser Tests
void TestParseUnaryExpr()
{
	// TODO make actual test :D
	auto expr = Parse(Tokenize("function pesho() { var a = 0; return a; }"));
}

int main()
{
	EXECUTE_TEST(TestLess);
	EXECUTE_TEST(TestNumber);
	EXECUTE_TEST(TestString);
	EXECUTE_TEST(TestKeyWord);
	EXECUTE_TEST(TestVariableDeclaration);
	EXECUTE_TEST(TestParseUnaryExpr);
#if defined(_WIN32)
	std::system("pause");
#endif
	return 0;
}
