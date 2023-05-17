#include "parser.h"

Parser::Parser()
{
	initPredictTable();
	symbolStack.push(GetNonTerminal(NonTerminalType::PROGRAM));
	root = new ASTNodeBase;
	root->nodeKind = ASTNodeKind::PRO_K;
	ASTStack.push(&root->child[2]);
	ASTStack.push(&root->child[1]);
	ASTStack.push(&root->child[0]);
	currentP = root;
}

void Parser::initPredictTable()
{

}

void Parser::pushSymbolStack(std::vector<SymbolStackItem> vec)
{
	for (auto item : vec)
	{
		symbolStack.push(item);
	}
}
void Parser::storeTokenSem(TokenType t)
{
	symbolStack.push(GetTerminal(t));
	currentP->names.push_back(currentT.sem);
}
void Parser::linkStackTop(ASTNodeBase* p)
{
	if (ASTStack.empty())
	{
		throw std::exception("ERROR: AST Stack empty.");
	}
	ASTNodeBase** ptr = ASTStack.top();
	ASTStack.pop();
	*ptr = p;
}
/*
 ��ջ��ΪProgramHead,��ǰToken�Ĵʷ���ϢΪPROGRAMʱ��
ѡ���������ʽ���������ͷ��������ʽ�Ҳ�ѹ�����ջ�����ɳ���
ͷ�ڵ㣬���﷨��ջ�����Ե���Ԫ�ؽ��и�ֵ��ʹ���﷨�����ڵ��
���ӽڵ�ָ�����ͷ�ڵ㡣
*/
void Parser::process2()
{
	pushSymbolStack({ GetNonTerminal(NonTerminalType::PROGRAM_NAME),
					GetTerminal(TokenType::PROGRAM)
					});
	
	currentP = GetASTLabelNode(ASTNodeKind::PHEAD_K);
	linkStackTop(currentP);
}

/*
�����������IDѹ�����ջ��������ǰ��ʶ����������Ϣ(��ʶ����)
д�����ͷ�ڵ㣬��ʶ��������1��
*/
void Parser::process3()
{
	storeTokenSem(TokenType::IDENTIFIER);
}


/*
������������������ʽ�Ҳ�ѹ�����ջ���﷨�����֣�������������
��־�ڵ㣬���﷨��ջ���õ�ָ��ĵ�ַ����ָ��ָ��������ڵ㣬
ʹ�ô˽ڵ���Ϊ���ڵ�Ķ��ӽڵ���֡���ǰ���������ڵ���ֵܽ�
��Ӧ��ָ�����������ʶ�ڵ㣬���������ڵ��������нڵ㣬����
�ڵ���Ӧָ�����������ڵ㣬�ʽ���ǰ�ڵ���ֵܽڵ�͵�һ����
�ӽڵ�ѹ���﷨��ջ���Դ��Ժ���
*/
void Parser::process7()
{
	pushSymbolStack({ GetNonTerminal(NonTerminalType::TYPE_DEC_LIST),
					GetTerminal(TokenType::TYPE)
					});
	currentP = GetASTLabelNode(ASTNodeKind::TYPE_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
	ASTStack.push(&currentP->child[0]);
}
/*
* <TypeDecList>:=TypeId=TypeDef;TypeDecMore
* �������������������﷨�������֣�����һ���������ͽڵ㣬��
�ڴ�����κ�������Ϣ�����﷨��ջ���õ�ָ��ĵ�ַ����ָ��ָ��
���������ͽڵ㣬���ǵ�һ�������ڵ㣬����Type���͵��ӽڵ�ָ��
��ǰ�ڵ㣬������Ϊ��һ�������������ֵܽڵ���֡������˽ڵ�
���ֵܽڵ�ѹ���﷨��ջ���Ա㴦����һ������������
*/
void Parser::process8()
{
	pushSymbolStack({ 
		GetNonTerminal(NonTerminalType::TYPE_DEC_MORE),
		GetTerminal(TokenType::SEMICOLON),
		GetNonTerminal(NonTerminalType::TYPE_DEF),
		GetTerminal(TokenType::EQUAL),
		GetNonTerminal(NonTerminalType::TYPE_ID)
					});
	currentP = (ASTNodeBase*)GetASTDecNode();
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
}
/*
û��������������������ʱ�﷨��ջ����ŵ������һ������������
����ֵܽڵ㣬������������Ͳ��ֵ��﷨���ڵ����ɡ�
*/
void Parser::process9()
{
	ASTStack.pop();
}

/*
���������ڵ�ı�ʶ�������Ͳ��֡��������Ϳ��������ͺ��ַ��ͣ�
�����ñ���temp��¼�ڵ�����д��ʶ��������Ϣ�Ĳ��ֵĵ�ַ������
��Ĳ���ʽ�����temp������ݽ��и�ֵ������������Ͳ��ֵ���
д��
*/
void Parser::process12()
{
	symbolStack.push(GetNonTerminal(NonTerminalType::BASE_TYPE));
	tempDecKindPtr = &((ASTDecNode*)currentP)->decKind;
}

/*
���������ͱ�ʶ�������������������������ͱ�ʶ�������ģ��ڵ�ǰ
�ڵ�洢�˱�ʶ�������֣��ڵ��ϱ�ʶ���ĸ�����1��
*/
void Parser::process14()
{
	*tempDecKindPtr = ASTDecKind::ID_K;
	storeTokenSem(TokenType::IDENTIFIER);
}
/*
���������������ͣ���temp�����ַ�����ݸ�ֵ����������Ϣ������
���ڵ㡣
*/
void Parser::process15()
{
	*tempDecKindPtr = ASTDecKind::INTEGER_K;
	symbolStack.push(GetTerminal(TokenType::INTEGER));
}

/*
�������������ַ��ͣ���temp�����ַ�����ݸ�ֵ�����ַ�����Ϣ��
�������ڵ㡣
*/
void Parser::process16()
{
	*tempDecKindPtr = ASTDecKind::CHAR_K;
	symbolStack.push(GetTerminal(TokenType::CHAR));
}

/*
������������Ϣ��ֵΪ��������ArrayK,����temp��¼��д�����
�����Ͳ��ֵĵ�ַ���Ժ��temp��ַ�����ݸ�ֵ��������˶������
������Ϣ����д��
*/
void Parser::process19()
{
	pushSymbolStack({
		GetNonTerminal(NonTerminalType::BASE_TYPE),
		GetTerminal(TokenType::OF),
		GetTerminal(TokenType::SQUARE_BRACKET_CLOSE),
		GetNonTerminal(NonTerminalType::TOP),
		GetTerminal(TokenType::DOT_DOT),
		GetNonTerminal(NonTerminalType::LOW),
		GetTerminal(TokenType::SQUARE_BRACKET_OPEN),
		GetTerminal(TokenType::ARRAY)
					});
	*tempDecKindPtr = ASTDecKind::ARRAY_K;
	tempDecKindPtr = &((ASTDecNode*)currentP)->decAttr.arrayAttr.itemType;
}

/*
�Ҳ��ռ��������ջ������������ֵд���������������ڵ���½硣
*/
void Parser::process20()
{
	symbolStack.push(GetTerminal(TokenType::INT));
	((ASTDecNode*)currentP)->decAttr.arrayAttr.low = std::stoi(currentT.sem);
}

/*
�Ҳ��ռ��������ջ������������ֵд���������������ڵ���Ͻ硣
*/
void Parser::process21()
{
	symbolStack.push(GetTerminal(TokenType::INT));
	((ASTDecNode*)currentP)->decAttr.arrayAttr.up = std::stoi(currentT.sem);
}


/*
�������Ͳ��ָ�ֵΪ��¼����RecordK,�ñ���saveP���浱ǰָ��
��¼���������ڵ��ָ�룬�Ա㴦�����¼�ĸ������Ժ��ܹ��ص���
¼���ͽڵ㴦��û����ɵ���Ϣ����ѹ��ָ���¼�ĵ�һ�����ָ��
���﷨��ջ���ں����ָ�븳ֵ��
*/
void Parser::process22()
{
	pushSymbolStack({
		GetTerminal(TokenType::END),
		GetNonTerminal(NonTerminalType::FIELD_DEC_LIST),
		GetTerminal(TokenType::RECORD)
					});
	*tempDecKindPtr = ASTDecKind::RECORD_K;
	saveP = (ASTDecNode*)currentP;
	ASTStack.push(&currentP->child[0]);
}
/*
���ɼ�¼���͵�һ���򣬽ڵ�Ϊ�������͵Ľڵ㣬������κ���Ϣ��
�������ڻ����ͣ���temp��¼��д������Ϣ�ĳ�Ա��ַ���Դ��Ժ���
д���������ͻ����ַ����͡����﷨��ջ����ָ��ָ������ڵ㡣��
�ǵ�һ��������record���͵��ӽڵ�ָ��ǰ�ڵ㣻��������һ��
��¼���������ֵܽڵ㡣���ѹ��ָ���¼������һ�����ָ�룬
�Դ�������
*/
void Parser::process23()
{
	pushSymbolStack({
		GetNonTerminal(NonTerminalType::FIELD_DEC_MORE),
		GetTerminal(TokenType::SEMICOLON),
		GetNonTerminal(NonTerminalType::ID_LIST),
		GetNonTerminal(NonTerminalType::BASE_TYPE),
					});
	currentP = (ASTNodeBase*)GetASTDecNode();
	linkStackTop(currentP);
	tempDecKindPtr = &((ASTDecNode*)currentP)->decKind;
	ASTStack.push(&currentP->sibling);
}

/*
* ���ɼ�¼���͵�һ���򣬽ڵ�Ϊ�����ڵ㣬�������������ͣ������
�κ���Ϣ�����﷨��ջ����ָ��ָ������ڵ㣬���ǵ�һ��������record
���͵��ӽ��ָ��ǰ��㣬��������һ����¼���������ֵܽ�㣬
���ѹ��ָ���¼������һ�����ָ�룬�Դ�������
*/
void Parser::process24()
{
	pushSymbolStack({
		GetNonTerminal(NonTerminalType::FIELD_DEC_MORE),
		GetTerminal(TokenType::SEMICOLON),
		GetNonTerminal(NonTerminalType::ID_LIST),
		GetNonTerminal(NonTerminalType::ARRAY_TYPE),
					});
	currentP = (ASTNodeBase*)GetASTDecNode(ASTDecKind::ARRAY_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
}
/*
* û�м�¼���͵���һ�����ˣ�����ջ����������һ������ֵܽڵ㣬
��ʾ��¼����ȫ�������ꣻ������saveP�ָ���ǰ��¼���ͽڵ��ָ
�롣
*/
void Parser::process25()
{
	ASTStack.pop();
	currentP = (ASTNodeBase*)saveP;
}
/*
�Ҳ������ջ��������ǰ��ʶ�������֣�����ڵ��У���ʶ��������1��
*/
void Parser::process27()
{
	pushSymbolStack({
		GetNonTerminal(NonTerminalType::ID_MORE),
		GetTerminal(TokenType::IDENTIFIER)
					});
	storeTokenSem(TokenType::IDENTIFIER);
}

/*
�����������������ʽ�Ҳ�ѹ�����ջ���﷨�����֣����ɱ�������
��־�ڵ㣬���﷨��ջ���õ�ָ��ĵ�ַ����ָ��ָ��������ڵ㣬
ʹ�ô˽ڵ���Ϊ���ڵ�Ķ��ӽڵ�������ͱ�ʶ�ڵ���ֵܽڵ��
�֡���ǰ���������ڵ���ֵܽڵ�Ӧ��ָ���������ڵ���������
�ڵ㣬���ӽڵ���Ӧָ�����������ڵ㣬�ʽ���ǰ�ڵ���ֵܽڵ�
�͵�һ�����ӽڵ�ѹ���﷨��ջ���Դ��Ժ���
*/
void Parser::process32()
{
	pushSymbolStack({
		GetNonTerminal(NonTerminalType::VAR_DEC_LIST),
		GetTerminal(TokenType::VAR)
					});
	currentP = GetASTLabelNode(ASTNodeKind::VAR_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
	ASTStack.push(&currentP->child[0]);
}
/*
�����ı����������﷨�������֣�����һ���������ͽڵ㣬��
�ڴ�����κ�������Ϣ�����﷨��ջ���õ�ָ��ĵ�ַ����ָ��ָ��
���������ͽڵ㣬���ǵ�һ�������ڵ㣬����VarK���͵��ӽڵ�ָ��
��ǰ�ڵ㣬������Ϊ��һ�������������ֵܽڵ���֡������˽ڵ�
���ֵܽڵ�ѹ���﷨��ջ���Ա㴦����һ������������
*/
void Parser::process33()
{
	pushSymbolStack({
		GetNonTerminal(NonTerminalType::VAR_DEC_MORE),
		GetTerminal(TokenType::SEMICOLON),
		GetNonTerminal(NonTerminalType::VAR_ID_LIST),
		GetNonTerminal(NonTerminalType::TYPE_DEF)
					});
	currentP = (ASTNodeBase*)GetASTDecNode();
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
}
/*
����������������ɸ���ʶ��������ǰ��ʶ�������֣�����ڵ��У�
��ʶ��������1��
*/
void Parser::process36()
{
	pushSymbolStack({
		GetNonTerminal(NonTerminalType::VAR_ID_MORE),
		GetTerminal(TokenType::IDENTIFIER)
					});
	storeTokenSem(TokenType::IDENTIFIER);
}
/*
���������������ʽ�Ҳ�ѹ�����ջ���﷨�����֣����ɹ���ͷ��
��ProcK,���﷨��ջ���õ�ָ��ĵ�ַ����ָ��ָ��������ڵ㣬
ʹ�ô˽ڵ���Ϊ���ڵ�Ķ��ӽڵ�������ͱ�ʶ�ڵ㣬���߱�����ʶ
�ڵ���ֵܽڵ���֡���ǰ���������ڵ���ֵܽڵ�ָ����һ������
�����ڵ��������нڵ㣬���������ӽڵ�ָ������岿�֣��ڶ���
�ӽڵ�ָ������е��������֣���û�����������ָ��Ϊ�գ���һ��
�ӽڵ�ָ�������ββ��֡��ʽ���ǰ�ڵ���ֵܽڵ���������ӽ�
��ѹ���﷨��ջ���Դ��Ժ���
*/
void Parser::process41()
{
	pushSymbolStack({
		GetNonTerminal(NonTerminalType::PROC_DEC_MORE),
		GetNonTerminal(NonTerminalType::PROC_BODY),
		GetNonTerminal(NonTerminalType::PROC_DEC_PART),
		GetTerminal(TokenType::SEMICOLON),
		GetNonTerminal(NonTerminalType::BASE_TYPE),
		GetTerminal(TokenType::COLON),
		GetTerminal(TokenType::BRACKET_CLOSE),
		GetNonTerminal(NonTerminalType::PARAM_LIST),
		GetTerminal(TokenType::BRACKET_OPEN),
		GetNonTerminal(NonTerminalType::PROC_NAME),
		GetTerminal(TokenType::PROCEDURE)
					});
	currentP = GetASTLabelNode(ASTNodeKind::PRO_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
	ASTStack.push(&currentP->child[2]);
	ASTStack.push(&currentP->child[1]);
	ASTStack.push(&currentP->child[0]);
}






























