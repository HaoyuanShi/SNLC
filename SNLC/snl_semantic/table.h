#pragma once
#include "symTable.h"
#include"../snl_grammar/parser.h"
#include"../snl_common/ast.h"
#include <vector>
#include<map>
#include<string>

class Table {
	std::map<std::string,TypeIR*> typetabel;      //���ͱ�
	std::vector<std::vector<symTablePtr>> scope;		//�ֳ����
	unsigned int Level;					//�ӹ������
public:
	Table() : Level(0) {};
	~Table();
public:
	void Analyze(ASTNodeBase* currentP);
	void VarDecList(ASTNodeBase* currentP);
protected:
	TypeIR* TypeProcess(ASTNodeBase* currentP);
	TypeIR* arrayType(ASTDecNode*);
	TypeIR* nameType(ASTNodeBase* currentP);
	TypeIR* recordType(ASTNodeBase* currentP);
	void initialize(void);
	void TypeDecPart(ASTNodeBase* currentP);
	void VarDecList(ASTNodeBase* currentP);
	void ProcDecPart(ASTNodeBase* currentP);
public:
	void CreatTable();												//��һ�����ű�
	void DestroyTable();											//����һ�����ű�
	bool Enter(char* Id, AttributelR* AttribP, symTablePtr* Entry);	//�ǼǱ�ʶ�������Ե����ű�
	bool FindEntry(char* id, bool diraction, symTablePtr* Entry);				//�ű��в��ұ�ʶ��
	void PrintSymbTabl() {};														//��ӡ���ű�
private:
	bool SearchoneTable(char* id, int currentLevel, symTablePtr* Entry);			//�ű��в��ұ�ʶ��


};