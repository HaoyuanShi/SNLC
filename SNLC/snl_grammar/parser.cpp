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
void Parser::initOpStack()
{
	while (!operandStack.empty())
	{
		delete operandStack.top();
		operandStack.pop();
	}
	while (!operatorStack.empty())
	{
		delete operatorStack.top();
		operatorStack.pop();
	}
	operatorStack.push(GetASTExpOpNode(ASTOpType::STACK_END));
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
���������Ͳ��ָ�ֵΪ��¼����RecordK,�ñ���saveP���浱ǰָ��
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
		GetNonTerminal(NonTerminalType::ID_MORE)
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
		GetNonTerminal(NonTerminalType::VAR_ID_MORE)
					});
	storeTokenSem(TokenType::IDENTIFIER);
}
/*
���ɺ���������־�ڵ�
*/
void Parser::process40()
{
	symbolStack.push(GetNonTerminal(NonTerminalType::PROC_DEC));
	currentP = GetASTLabelNode(ASTNodeKind::PROC_DEC_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->child[0]);
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
		//GetNonTerminal(NonTerminalType::BASE_TYPE),
		//GetTerminal(TokenType::COLON),
		GetTerminal(TokenType::BRACKET_CLOSE),
		GetNonTerminal(NonTerminalType::PARAM_LIST),
		GetTerminal(TokenType::BRACKET_OPEN),
		GetNonTerminal(NonTerminalType::PROC_NAME),
		GetTerminal(TokenType::PROCEDURE)
					});
	currentP = (ASTNodeBase*)GetASTDecNode(ASTDecKind::PROC_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
	ASTStack.push(&currentP->child[2]);
	ASTStack.push(&currentP->child[1]);
	ASTStack.push(&currentP->child[0]);
	saveP = (ASTDecNode*)currentP;
}
/*
* ����û�б�Ĳ�����˵���������ββ��ִ�����ϣ��������һ����
���������ֵܽڵ㣻������saveFuncP�ָ���ǰ�ڵ�Ϊ���������ڵ㣬
*/
void Parser::process48()
{
	ASTStack.pop();
	currentP = (ASTNodeBase*)saveP;

}
/*
�����β��������ò���ʽ˵������Ϊֵ�Ρ��﷨�������֣�����һ
���������ͽڵ㣬�ڽڵ�Ĳ������Ͳ���д���βΣ����﷨��ջ����
��ָ��ĵ�ַ����ָ��ָ����������ͽڵ㣬���ǵ�һ�������ڵ㣬
����ProcK���͵ĵ�һ���ӽ��ָ��ǰ��㣻������Ϊ��һ����
���������ֵܽ����֣������˽ڵ���ֵܽڵ�ѹ���﷨��ջ���Ա�
������һ���β�������

*/
void Parser::process50()
{
	pushSymbolStack({
		GetNonTerminal(NonTerminalType::FORM_LIST),
		GetNonTerminal(NonTerminalType::TYPE_DEF)
					});
	currentP = (ASTNodeBase*)GetASTDecNode(ASTDecKind::ID_K);
	((ASTDecNode*)currentP)->decAttr.procAttr.paramType = ASTParamType::VAL_PARAM_TYPE;
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
}
/*
�����β��������������ʽ˵������Ϊ��Ρ��﷨�������֣�����
һ���������ͽڵ㣬�ڽڵ�Ĳ������Ͳ���д�ϱ�Σ����﷨��ջ��
�õ�ָ��ĵ�ַ����ָ��ָ����������ͽڵ㣬���ǵ�һ�������ڵ㣬
����FuncK���͵ĵ�һ���ӽ��ָ��ǰ��㣬������Ϊ��һ����
���������ֵܽ����֡������˽ڵ���ֵܽڵ�ѹ���﷨��ջ���Ա�
������һ���β�������
*/
void Parser::process51()
{
	pushSymbolStack({
		GetNonTerminal(NonTerminalType::FORM_LIST),
		GetNonTerminal(NonTerminalType::TYPE_DEF),
		GetTerminal(TokenType::VAR)
					});
	currentP = (ASTNodeBase*)GetASTDecNode(ASTDecKind::ID_K);
	((ASTDecNode*)currentP)->decAttr.procAttr.paramType = ASTParamType::VAR_PARAM_TYPE;
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
}
/*
�Ҳ�������ջ��������ʶ��������д�뵱ǰ�﷨���ڵ��У��ڵ��б�
ʶ������idnum��1��
*/
void Parser::process52()
{
	pushSymbolStack({
		GetNonTerminal(NonTerminalType::FID_MORE),
					});
	storeTokenSem(TokenType::IDENTIFIER);
}
/*
����������У�����ʽ�Ҳ�ѹ�����ջ���﷨�����֣��ȵ��﷨��ջ��
ɾ��ջ���Ķ���ָ�룬�Ա�֤һ����child[2]ָ��������нڵ㣻
����������б�־�ڵ㣬�˽ڵ��µ����нڵ㶼�����ڵ㡣���﷨
��ջ����ָ��ָ��˱�־�ڵ㣬ʹ�ýڵ���Ϊ���ڵ����ͷ�ڵ��
���һ�����ӽڵ���֡�����ǰ�ڵ�ĵ�һ�����ӽڵ�ѹ���﷨��ջ��
�Դ��Ժ���

*/
void Parser::process57()
{
	pushSymbolStack({
			GetTerminal(TokenType::END),
			GetNonTerminal(NonTerminalType::STM_LIST),
			GetTerminal(TokenType::BEGIN)
					});
	ASTStack.pop();
	currentP = GetASTLabelNode(ASTNodeKind::STM_L_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->child[0]);
}
/*
��һ��������ͽڵ㣬��������ֵ��Ϊ�������IfK,���﷨��ջ��
����ָ��ĵ�ַ������Ӧָ��ָ����������ڵ㣬��ѹ��if����
�ֵܽڵ�ָ���ַ�Դ�����һ����䡣
*/
void Parser::process61()
{
	symbolStack.push(GetNonTerminal(NonTerminalType::CONDITIONAL_STM));
	currentP = (ASTNodeBase*)GetASTStmtNode(ASTStmtKind::IF_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
}
/*
����һ��������ͽڵ㣬��������ֵ��Ϊѭ�����WhileK,���﷨��
ջ�е���ָ��ĵ�ַ������Ӧָ��ָ���ѭ�����ڵ㣬��ѹ��while
�����ֵܽڵ�ָ���ַ�Դ�����һ����䡣

*/
void Parser::process62()
{
	symbolStack.push(GetNonTerminal(NonTerminalType::LOOP_STM));
	currentP = (ASTNodeBase*)GetASTStmtNode(ASTStmtKind::WHILE_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
}
/*
����һ��������ͽڵ㣬��������ֵ��Ϊ�������ReadK,���﷨��
ջ�е���ָ��ĵ�ַ������Ӧָ��ָ����������ڵ㣬��ѹ��read
�����ֵܽڵ�ָ���ַ�Դ�����һ����䡣
*/
void Parser::process63()
{
	symbolStack.push(GetNonTerminal(NonTerminalType::INPUT_STM));
	currentP = (ASTNodeBase*)GetASTStmtNode(ASTStmtKind::READ_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
}
/*
����һ��������ͽڵ㣬��������ֵ��Ϊ������WriteK,���﷨��
ջ�е���ָ��ĵ�ַ������Ӧָ��ָ���������ڵ㣬��ѹ��write
�����ֵܽڵ�ָ���ַ�Դ�����һ����䡣
*/
void Parser::process64()
{
	symbolStack.push(GetNonTerminal(NonTerminalType::OUTPUT_STM));
	currentP = (ASTNodeBase*)GetASTStmtNode(ASTStmtKind::WRITE_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
}
/*
����һ��������ͽڵ㣬��������ֵ��Ϊ�������ReturnK,���﷨��
ջ�е���ָ��ĵ�ַ������Ӧָ��ָ��˷������ڵ㣬��ѹ��return
�����ֵܽڵ�ָ���ַ�Դ�����һ����䡣
*/
void Parser::process65()
{
	symbolStack.push(GetNonTerminal(NonTerminalType::RETURN_STM));
	currentP = (ASTNodeBase*)GetASTStmtNode(ASTStmtKind::RETURN_K);
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
}
/*
����һ��������ͽڵ㣬���ڸ�ֵ���͹��̵�����䶼�Ա�ʶ����
ʼ�������ڲ���ȷ�������ľ������ͣ�����һ���������ʽ�ڵ㣬
��¼��ֵ�󲿣���ʹ���Ϊ�����ڵ�ĵ�һ�����ӽڵ㣻���﷨��
ջ�е���ָ��ĵ�ַ������Ӧָ��ָ��˷������ڵ㣬��ѹ�����
����ֵܽڵ�ָ���ַ�Դ�����һ����䡣
*/
void Parser::process66()
{
	pushSymbolStack({
			GetNonTerminal(NonTerminalType::ASS_CALL),
					});
	currentP = (ASTNodeBase*)GetASTStmtNode();
	currentP->child[0] = (ASTNodeBase*)GetASTExpNode(ASTEXPKind::ID_K);
	((ASTExpNode*)currentP->child[0])->expAttr.varType = ASTVarType::ID_V;
	auto tmp = currentP;
	currentP = currentP->child[0];
	storeTokenSem(TokenType::IDENTIFIER);
	currentP = tmp;
	linkStackTop(currentP);
	ASTStack.push(&currentP->sibling);
}
/*
����ʽ�Ҳ����ռ��������ջ�����ľ�������ֵȷ��Ϊ��ֵ��䣬
��AssignKд�뵱ǰ����ڵ��С�
*/
void Parser::process67()
{
	pushSymbolStack({
			GetNonTerminal(NonTerminalType::ASSIGNMENT_REST),
					});
	((ASTStmtNode*)currentP)->stmtKind = ASTStmtKind::ASSIGN_K;

}
/*
��ʽ�Ҳ����ռ��������ջ�������ľ�������ȷ��Ϊ��־��������
���ľ�������ֵȷ��Ϊ���̵�����䣬��CallKд�뵱ǰ����ڵ�
�С�
*/
void Parser::process68()
{
	pushSymbolStack({
			GetNonTerminal(NonTerminalType::CALL_STM_REST)
					});
	((ASTStmtNode*)currentP)->stmtKind = ASTStmtKind::CALL_K;
	((ASTExpNode*)currentP->child[0])->expAttr.varType = ASTVarType::ID_V;
}
/*

��ֵ���ڵ�ĸ�ֵ�Ҳ��ӽڵ�ָ��ѹջ���ı䵱ǰ�ڵ�ָ��Ϊ��ֵ
���ӽڵ㡣ѹ������ջ�ױ�־��
*/
void Parser::process69()
{
	ASTStack.push(&currentP->child[1]);
	currentP = currentP->child[0];
	initOpStack();
}
/*

if������������֣��������ʽ���֣�then���ֺ�else���֣��ֱ���
�������ӽڵ�ָ����Ӧ�Ĳ��֡��ʽ��������ӽڵ㰴���������﷨��
ջ���Դ��Ժ�ֵ��
*/
void Parser::process70()
{
	pushSymbolStack({
			GetTerminal(TokenType::FI),
			GetNonTerminal(NonTerminalType::STM_LIST),
			GetTerminal(TokenType::ELSE),
			GetNonTerminal(NonTerminalType::STM_LIST),
			GetTerminal(TokenType::THEN),
			GetNonTerminal(NonTerminalType::REL_EXP),
			GetTerminal(TokenType::IF)
					});
	ASTStack.push(&currentP->child[2]);
	ASTStack.push(&currentP->child[1]);
	ASTStack.push(&currentP->child[0]);
}

/*
while����������֣��������ʽ��ѭ���塣�ֱ����������ӽڵ�ָ��
ָ�򡣽��������ӽڵ�ָ���ַ���մ�����������ѹ���﷨��ջ��
*/
void Parser::process71()
{
	pushSymbolStack({
			GetTerminal(TokenType::END_WHILE),
			GetNonTerminal(NonTerminalType::STM_LIST),
			GetTerminal(TokenType::DO),
			GetNonTerminal(NonTerminalType::REL_EXP),
			GetTerminal(TokenType::WHILE)
					});
	ASTStack.push(&currentP->child[1]);
	ASTStack.push(&currentP->child[0]);
}
/*
������ڵ�ĵ�һ�����ӽڵ�Ӧָ����ʽ�����ĸ��ڵ㡣���Ƚ�
������ĵ�һ�����ӽڵ�ָ���ַѹ�﷨��ջ�����⣬Ҫ������
ʽ����Ӧ���ȳ�ʼ�����ʽ������Ҫ�Ĳ�����ջ��ѹ��һ�������
�����������������ȼ���͡�������ջ�Ͳ�����ջ�д�ŵĶ���ָ��
���ڵ�ָ�룬����ɱ��ʽ�����ڵ�����ӡ�����������һ����"END"
��Ϊ���ݵĲ������ڵ㣬�ٽ���ָ��ѹ�������ջ��ʵ�ֳ�ʼ�����ܡ�

*/
void Parser::process74()
{
	pushSymbolStack({
			GetTerminal(TokenType::WRITE),
			GetTerminal(TokenType::BRACKET_OPEN),
			GetNonTerminal(NonTerminalType::EXP),
			GetTerminal(TokenType::BRACKET_CLOSE)
					});
	ASTStack.push(&currentP->child[0]);
	initOpStack();
}
/*
������������䣬����ѹ�뺯���������ĵ�һ(����)�����ӽڵ㡣
*/
void Parser::process76()
{
	ASTStack.push(&currentP->child[1]);
}
/*
������������ʵ���Ǳ��ʽ����Ҫ������ʽ�����ȳ�ʼ������
��ջ������һ����������ȼ���͵Ĳ�������
*/
void Parser::process78()
{
	pushSymbolStack({
			GetNonTerminal(NonTerminalType::ACT_PARAM_MORE),
			GetNonTerminal(NonTerminalType::EXP)
					});
	initOpStack();
}
/*
���б��ʵ�Σ�����ʽ�Ҳ������ջ��ѹ��ǰʵ�νڵ���ֵܽ�
�����﷨��ջ��
*/
void Parser::process80()
{
	pushSymbolStack({
			GetNonTerminal(NonTerminalType::ACT_PARAM_LIST),
			GetTerminal(TokenType::COMMA)
					});
	ASTStack.push(&currentP->sibling);
}
/*
�����������ʽ����if��while����н��õ��������ʽ���������
���ʽ�Ĳ�����ջ���г�ʼ����ѹ��һ�����ȼ���͵Ĳ��������ͽ�
���ָ�롣���⣬������getExpResult��ֵΪ�٣�һ����ʽ������
��process840ʱ�����ʽ����������������ʽ�Ĳ�����ջ�Ͳ�����
ջ���õ����ʽ���ֵ����ṹ������������У������ڹ�ϵ���ʽ��
��ϵ������󲿵ı��ʽ����������������ʽ��δ�����꣬����ջ��
�������Ҳ��ı��ʽʱ���Ž�����

*/
void Parser::process81()
{
	pushSymbolStack({
			GetNonTerminal(NonTerminalType::OTHER_REL_E),
			GetNonTerminal(NonTerminalType::EXP)
					});
	initOpStack();
	getExpResault = false;
}
/*
����һ�����������ͱ��ʽ�ڵ㣬��¼�����ϵ����������ݡ��Ƚ�
ջ������������������������ȼ�����ջ�������������ȼ����ڻ���
���ڵ�ǰ��������Ӳ�����ջ����һ����������ָ��t,�Ӳ�����ջ��
�������������ڵ�ָ�룬�ֱ���Ϊt���������ӽڵ㣬�ٽ��������
��t�Ľڵ�ָ��ѹ�������ջ����Ϊ��һ�����������������������
ǰ������ָ��ѹ�������ջ������ֱ�ӽ���ǰָ��ѹ�������ջ��
���⣬ѡ�����������ʽ��˵����Ϊ��ϵ�����������ı��ʽExp,
�Ѿ������꣬Ҫ�����ҷ����Ĵ�����Ϊ�ҷ�����Exp����������
��ϵ���ʽҲ�������������ں���process840�н������õ����ʽ��
����ʶgetExpResultΪ�档
*/
void Parser::process82()
{
	pushSymbolStack({
			GetNonTerminal(NonTerminalType::EXP),
			GetNonTerminal(NonTerminalType::CMP_OP)
					});

	currentP = (ASTNodeBase*)GetASTExpOpNode(TokenType2OpType(currentT.type));
	while (GetOpPriority(operatorStack.top()->expAttr.op)
		   >= GetOpPriority(((ASTExpNode*)currentP)->expAttr.op)
		   )
	{
		auto t = operatorStack.top();
		operatorStack.pop();
		auto rNum = operandStack.top();
		operandStack.pop();
		auto lNum = operandStack.top();
		operandStack.pop();
		t->nodeBase.child[0] = (ASTNodeBase*)lNum;
		t->nodeBase.child[1] = (ASTNodeBase*)rNum;
		operandStack.push(t);
	}
	operatorStack.push((ASTExpNode*)currentP);
	getExpResault = true;
}
/*
����write,return�������������)��Ϊ�������ʽ�Ľ��������ұ��
ʽ�������ֿ�����(���ʽ),�������������ţ���Ҫ���������
�����ǰ��������Ϊ������),����expflag��Ϊ0,˵��ǰ����
��֮��Ե�������(,��ʱ���������Ǳ��ʽ��һ���֣�ѭ��������
��һ��������ջ���ݣ���������������ջ���ݣ��Բ������ڵ������
���ӽڵ㸳ֵ�������ڵ�ָ�룬��������Ϊ��������������������
������ָ��ѹ�������ջ����Ϊ��һ����������������������ѭ����
ֱ��������ջ��Ϊ(,����(,����expflag��1,˵����������ʽ
�е�һ�����ţ�������
���򣬵�ǰ�������ʱ�ʶ��exp�Ĵ���������жϣ����
getExpResultΪ�����getExpResult2Ϊ�棬�����ǰ���ʽ�����
����ѭ����������һ��������ջ���ݣ���������������ջ���ݣ�����
�����������ҷ��������ѽ��ָ��ѹ�������ջ��ֱ������������ջ
�ױ�ʶEND,����ջ�ױ�ʶ�����ʽ����������ϣ����ڵ��ڲ�����
ջ�У��������������﷨��ջ�е���Ӧָ����ʽ�������ڵ��ָ��
��ַ����ֵָ��ָ����ʽ���ڵ㡣
���У�getExpResult2ר���������Ա�������������Ա���ʽ��
�����ʱ����Ҫ��������������ڵ������������Ա���ʽ�ĸ����
��Ϊ�����Ա�����ڵ�Ķ��ӽڵ㡣

*/
void Parser::process84()
{
	if (currentT.type == TokenType::BRACKET_CLOSE && expFlag == 0)
	{
		//���ʽ����
		if (getExpResault || getExpResault2)
		{
			while (operatorStack.top()->expAttr.op != ASTOpType::STACK_END)
			{
				auto t = operatorStack.top();
				operatorStack.pop();
				auto rNum = operandStack.top();
				operandStack.pop();
				auto lNum = operandStack.top();
				operandStack.pop();
				t->nodeBase.child[0] = (ASTNodeBase*)lNum;
				t->nodeBase.child[1] = (ASTNodeBase*)rNum;
				operandStack.push(t);
			}
			delete operatorStack.top();
			operatorStack.pop();
			currentP = (ASTNodeBase*)operandStack.top();
			operandStack.pop();
			linkStackTop(currentP);
			getExpResault2 = false;
		}
	}
	else
	{
		//�����ű��ʽ
		while (operatorStack.top()->expAttr.op != ASTOpType::BRACKET_OPEN)
		{
			auto t = operatorStack.top();
			operatorStack.pop();
			auto rNum = operandStack.top();
			operandStack.pop();
			auto lNum = operandStack.top();
			operandStack.pop();
			t->nodeBase.child[0] = (ASTNodeBase*)lNum;
			t->nodeBase.child[1] = (ASTNodeBase*)rNum;
			operandStack.push(t);
		}
		delete operatorStack.top();
		operatorStack.pop();
		expFlag--;

	}
}
/*
�����ӷ������������һ�����ʽ�ڵ㣬���������ǲ��������ͣ���
¼����ӷ�����������ݡ�����ǰ������ջ�����ݣ���ջ����������
���ȼ����ڻ���ڵ�ǰ��������Ӳ�����ջ����һ����������ָ��t,
�Ӳ�����ջ���������������ڵ�ָ�룬�ֱ���Ϊt���������ӽڵ㣬
�ٽ����������t�Ľڵ�ָ��ѹ�������ջ����Ϊ��һ������������
�����������ǰ������ָ��ѹ�������ջ������ֱ�ӽ���ǰָ��ѹ
�������ջ��

*/
void Parser::process85()
{
	pushSymbolStack({
			GetNonTerminal(NonTerminalType::EXP),
			GetNonTerminal(NonTerminalType::ADD_OP)
					});
	currentP = (ASTNodeBase*)GetASTExpOpNode(TokenType2OpType(currentT.type));
	while (GetOpPriority(operatorStack.top()->expAttr.op)
		   >= GetOpPriority(((ASTExpNode*)currentP)->expAttr.op)
		   )
	{
		auto t = operatorStack.top();
		operatorStack.pop();
		auto rNum = operandStack.top();
		operandStack.pop();
		auto lNum = operandStack.top();
		operandStack.pop();
		t->nodeBase.child[0] = (ASTNodeBase*)lNum;
		t->nodeBase.child[1] = (ASTNodeBase*)rNum;
		operandStack.push(t);
	}
	operatorStack.push((ASTExpNode*)currentP);
}
/*
�����˷������������һ�����ʽ�ڵ㣬���������ǲ��������ͣ���
¼����˷�����������ݡ�����ǰ������ջ�����ݣ�����*, /˵��ջ
�������������ȼ����ڵ�ǰ��������Ӳ�����ջ����һ����������ָ
��t, �Ӳ�����ջ���������������ڵ�ָ�룬�ֱ���Ϊt���������ӽ�
�㣬�ٽ����������t�Ľڵ�ָ��ѹ�������ջ����Ϊ��һ��������
���������������ǰ������ָ��ѹ�������ջ����֤���ʽ�ļ�����
�����ϡ�����ֱ�ӽ���ǰָ��ѹ�������ջ��
*/
void Parser::process88()
{
	pushSymbolStack({
			GetNonTerminal(NonTerminalType::MULTI_OP),
			GetNonTerminal(NonTerminalType::TERM)
					});
	currentP = (ASTNodeBase*)GetASTExpOpNode(TokenType2OpType(currentT.type));
	while (GetOpPriority(operatorStack.top()->expAttr.op)
		   >= GetOpPriority(((ASTExpNode*)currentP)->expAttr.op)
		   )
	{
		auto t = operatorStack.top();
		operatorStack.pop();
		auto rNum = operandStack.top();
		operandStack.pop();
		auto lNum = operandStack.top();
		operandStack.pop();
		t->nodeBase.child[0] = (ASTNodeBase*)lNum;
		t->nodeBase.child[1] = (ASTNodeBase*)rNum;
		operandStack.push(t);
	}
	operatorStack.push((ASTExpNode*)currentP);
}
/*
���ʽ������Ϊ�����ŵı��ʽ����ǰ����Ϊ�����ţ�������ҲҪ��
������ջ�����ԣ�����һ�����������ʽ�ڵ㣬���ݸ�ֵΪ������(,
ָ��ѹ�������ջ�У�����expflag��1,������expflag��ô���
(δ��ԡ�
*/
void Parser::process89()
{
	pushSymbolStack({
			GetTerminal(TokenType::BRACKET_CLOSE),
			GetNonTerminal(NonTerminalType::EXP),
			GetTerminal(TokenType::BRACKET_OPEN)
					});
	currentP = (ASTNodeBase*)GetASTExpOpNode(ASTOpType::BRACKET_OPEN);
	operatorStack.push((ASTExpNode*)currentP);
	expFlag++;
}
/*
����һ�����������ʽ�ڵ㣬��������Ϊ�������ʽ����������ֵд
��ڵ��У���������������ڵ�ָ��ѹ�������ջ��
*/
void Parser::process90()
{
	pushSymbolStack({
			GetTerminal(TokenType::INTEGER),
					});
	currentP = (ASTNodeBase*)GetASTExpConstNode();
	((ASTExpNode*)currentP)->expAttr.val = std::stoi(currentT.sem);
	operandStack.push((ASTExpNode*)currentP);
}
/*
����һ�������ı��ʽ�ڵ㣬����־�����ּ���ڵ��У������˽ڵ�
ѹ�������ջ��
*/
void Parser::process92()
{
	pushSymbolStack({
			GetNonTerminal(NonTerminalType::VARI_MORE)
					});
	currentP = (ASTNodeBase*)GetASTExpVarNode();
	storeTokenSem(TokenType::IDENTIFIER);
	operandStack.push((ASTExpNode*)currentP);
}
/*
�������ʽ�ľ��������Ǳ�־�������������������Ϣ���뵱ǰ����
���ʽ�ڵ㡣
*/
void Parser::process93()
{
	((ASTExpNode*)currentP)->expAttr.varType = ASTVarType::ID_V;
}
/*
��ʽ�Ҳ������ջ�������ľ�����������Ϊ�����Ա������ѹ��ָ
�������Ա���ʽ�Ķ��ӽڵ�ָ�룬ѹ�����������ȼ�������ջ��
��־��
*/
void Parser::process94()
{
	pushSymbolStack({
			GetTerminal(TokenType::SQUARE_BRACKET_OPEN),
			GetNonTerminal(NonTerminalType::EXP),
			GetTerminal(TokenType::SQUARE_BRACKET_CLOSE)
					});
	((ASTExpNode*)currentP)->expAttr.varType = ASTVarType::ARRAY_MEMB_V;
	ASTStack.push(&currentP->child[0]);
	initOpStack();
	getExpResault2 = true;
}
/*
����ʽ�Ҳ������ջ����ǰ�����ڵ�ľ�����������Ϊ���Ա���ͱ�
����ѹ�����Ա�����ڵ�Ķ��ӽڵ㣬�Դ������Ա���ʽ��
*/
void Parser::process95()
{
	pushSymbolStack({
			GetNonTerminal(NonTerminalType::FIELD_VAR),
			GetTerminal(TokenType::DOT)
					});
	((ASTExpNode*)currentP)->expAttr.varType = ASTVarType::FIELD_MEMB_V;
	ASTStack.push(&currentP->child[0]);
}
/*
����һ�������ı��ʽ�ڵ㣻����־��������ڵ��У����﷨��ջ��
�õ�������Ķ��ӽڵ�ָ�룬ָ������Ա�ڵ㡣
*/
void Parser::process96()
{
	pushSymbolStack({
			GetNonTerminal(NonTerminalType::FIELD_VAR_MORE)
					});
	currentP = (ASTNodeBase*)GetASTExpVarNode();
	storeTokenSem(TokenType::IDENTIFIER);
	linkStackTop(currentP);
}
/*
˵���������ʽ�ľ��������Ǳ�־�������������������Ϣ���뵱ǰ
�������ʽ�ڵ㡣
*/
void Parser::process97()
{
	((ASTExpNode*)currentP)->expAttr.varType = ASTVarType::ID_V;
}




































































