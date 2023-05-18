#include "lexer.h"


TokenList Lexer::genTokens(FileReader& fScanner)
{
	TokenList res;

	int scanLine = 1;
	int scanCol = 0;

	int tokenLine = 1;
	int tokenCol = 1;

	char ch = ' ';
	std::string sem = "";

	LexState state = LexState::START;

	bool notEnd = fScanner.getNextChar(ch);
	scanCol++;

	while (notEnd)
	{

		if (ch == '\n' && (state == LexState::START || state == LexState::IN_COMMENT))
		{
			scanLine++;
			scanCol = 0;
		}
		
		switch (state)
		{
			case LexState::START:
			{

				if (isspace(ch))
				{
					// �ػ����
					notEnd = fScanner.getNextChar(ch);
					scanCol++;
					continue;
				}
				sem += ch;
				//���µ�ǰ��Ч����λ��
				tokenLine = scanLine;
				tokenCol = scanCol;
				//���벻ͬ״̬
				if (isalpha(ch))
				{
					state = LexState::IN_ID;
				}
				else if (isdigit(ch))
				{
					state = LexState::IN_NUM;
				}
				else if (ch == ':')
				{
					state = LexState::IN_ASSIGN;
				}
				else if (ch == '{')
				{
					state = LexState::IN_COMMENT;
				}
				else if (ch == '.')
				{
					state = LexState::IN_DOT;
				}
				else
				{
					//���ֽ�����ص����
					TokenType tp = getID(sem);
					if (tp != TokenType::NOT_ID)
					{
						res.appendToken(
							Token(
								tokenLine, tokenCol, sem, tp
							)
						);
						sem = "";
					}
					else
					{
						//���Ǳ����֣��ض��Ƿ�
						std::string err = "Line: " + std::to_string(tokenLine) + 
							" Column: " + std::to_string(tokenCol) +
							"ERROR: Expect a single delimiter word but get: " + sem + "\n";
						throw std::exception(err.c_str());
					}
				}
				//���۽�����һ��״̬�����ػ���㣬����ȡ��һ���ַ�
				notEnd = fScanner.getNextChar(ch);
				scanCol++;
				break;
			}
			case LexState::IN_ID:
			{
				if (isalnum(ch))
				{
					//ѭ����״̬��ȡ��һ���ַ�
					sem += ch;
					notEnd = fScanner.getNextChar(ch);
					if (!notEnd)
					{
						TokenType tp = getID(sem);
						res.appendToken(
							Token(
								tokenLine, tokenCol, sem,
								tp == TokenType::NOT_ID ? TokenType::IDENTIFIER : tp
							)
						);
					}
					scanCol++;
					continue;
				}
				else
				{
					TokenType tp = getID(sem);
					res.appendToken(
						Token(
							tokenLine, tokenCol, sem,
							tp == TokenType::NOT_ID ? TokenType::IDENTIFIER : tp
						)
					);
					// �ص���㣬����ȡ��һ���ַ�
					sem = "";
					state = LexState::START;
				}
				break;
			}
			case LexState::IN_NUM:
			{
				if (isdigit(ch))
				{
					//ѭ����״̬��ȡ��һ���ַ�
					sem += ch;
					notEnd = fScanner.getNextChar(ch);
					if (!notEnd)
					{
						res.appendToken(
							Token(
								tokenLine, tokenCol, sem, TokenType::INT
							)
						);
					}
					scanCol++;
					continue;
				}
				else
				{
					res.appendToken(
						Token(
							tokenLine, tokenCol, sem, TokenType::INT
						)
					);
					// �ص���㣬����ȡ��һ���ַ�
					sem = "";
					state = LexState::START;
				}
				break;
			}
			case LexState::IN_COMMENT:
			{	
				if (ch != '}')
				{
					// ѭ����״̬��ȡ��һ���ַ�
					notEnd = fScanner.getNextChar(ch);
					if (!notEnd)
					{
						std::cout << "ERROR: Unclosed comment region.\n";
					}
					scanCol++;
					continue;
				}
				// �ص���㣬ȡ��һ���ַ�
				sem = "";
				notEnd = fScanner.getNextChar(ch);
				scanCol++;
				state = LexState::START;
				break;
			}
			case LexState::IN_ASSIGN:
			{
				if (ch == '=')
				{
					sem += ch;
					res.appendToken(
						Token(
							tokenLine, tokenCol, sem, TokenType::ASSIGN
						)
					);
					// �ص���㣬ȡ��һ���ַ�
					sem = "";
					notEnd = fScanner.getNextChar(ch);
					scanCol++;
					state = LexState::START;
				}
				else
				{
					std::string err = "Line: " + std::to_string(tokenLine) +
						" Column: " + std::to_string(tokenCol) + 
						"ERROR: Invalid character: " + ch + std::string(" after : expect =\n");
					throw std::logic_error(err.c_str());
				}
				break;
			}
			case LexState::IN_DOT:
			{
				if (ch == '.')
				{
					sem += ch;
					res.appendToken(
						Token(
							tokenLine, tokenCol, sem, TokenType::DOT_DOT
						)
					);
					// �ص���㣬ȡ��һ���ַ�
					sem = "";
					notEnd = fScanner.getNextChar(ch);
					scanCol++;
					state = LexState::START;
				}
				else
				{
					res.appendToken(
						Token(
							tokenLine, tokenCol, sem, TokenType::DOT
						)
					);
					// �ص���㣬����ȡ��һ���ַ�
					sem = "";
					state = LexState::START;
				}
				break;
			}
		}
	}
	
	if (sem == ".")
	{
		res.appendToken(
			Token(
				tokenLine, tokenCol, sem, TokenType::DOT
			)
		);
	}
	else if (sem == ":")
	{
		res.appendToken(
			Token(
				tokenLine, tokenCol, sem, TokenType::COLON
			)
		);
	}
	
	res.appendToken(
		Token(tokenLine, tokenCol, "", TokenType::EOF_)
	);
	return res;
}