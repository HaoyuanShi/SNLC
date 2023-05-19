#pragma once
#include <string>
#include <vector>
#include "token.h"

// AST�ڵ�����ö��
#define NODETYPE_TABLE()\
DEF_TYPE(PRO_K) \
DEF_TYPE(PHEAD_K) \
DEF_TYPE(TYPE_K) \
DEF_TYPE(VAR_K) \
DEF_TYPE(PROC_K) \
DEF_TYPE(STM_L_K) \
DEF_TYPE(DEC_K) \
DEF_TYPE(STMT_K) \
DEF_TYPE(EXP_K)

enum class ASTNodeKind
{
#define DEF_TYPE(v) v,
    NODETYPE_TABLE()
#undef DEF_TYPE
};

const char* ASTNodeKindName(ASTNodeKind);

// DEC_K���ͽڵ�ľ�������ö��
#define DECTYPE_TABLE()\
DEF_TYPE(ARRAY_K) \
DEF_TYPE(CHAR_K) \
DEF_TYPE(INTEGER_K) \
DEF_TYPE(RECORD_K) \
DEF_TYPE(ID_K) \
DEF_TYPE(PROC_DEC_K) \
DEF_TYPE(PROC_V_K)

enum class ASTDecKind
{
#define DEF_TYPE(v) v,
    DECTYPE_TABLE()
#undef DEF_TYPE
};

const char* DecKindName(ASTDecKind);

// STMT_K���ͽڵ�ľ�������ö��
#define STMTTYPE_TABLE()\
DEF_TYPE(IF_K) \
DEF_TYPE(WHILE_K) \
DEF_TYPE(ASSIGN_K) \
DEF_TYPE(READ_K) \
DEF_TYPE(WRITE_K) \
DEF_TYPE(CALL_K) \
DEF_TYPE(RETURN_K)

enum class ASTStmtKind
{
#define DEF_TYPE(v) v,
    STMTTYPE_TABLE()
#undef DEF_TYPE
};


const char* StmtKindName(ASTStmtKind);

// EXP_K���ͽڵ�ľ�������ö��
#define EXPTYPE_TABLE()\
DEF_TYPE(OP_K) \
DEF_TYPE(CONST_K) \
DEF_TYPE(ID_K)

enum class ASTEXPKind
{
#define DEF_TYPE(v) v,
    EXPTYPE_TABLE()
#undef DEF_TYPE
};

const char* ExpKindName(ASTEXPKind);


// �﷨�����ʽ�ڵ�������ö��
#define OPTYPE_TABLE()\
DEF_TYPE(LT) \
DEF_TYPE(EQ) \
DEF_TYPE(PLUS) \
DEF_TYPE(MINUS) \
DEF_TYPE(TIMES) \
DEF_TYPE(OVER) \
DEF_TYPE(BRACKET_OPEN) \
DEF_TYPE(BRACKET_CLOSE) \
DEF_TYPE(STACK_END)


enum class ASTOpType
{
#define DEF_TYPE(v) v,
    OPTYPE_TABLE()
#undef DEF_TYPE
};

const char* ExpOpName(ASTOpType);

int GetOpPriority(ASTOpType);


// �﷨�����ʽ�ڵ�ı�������ö��
#define VARTYPE_TABLE()\
DEF_TYPE(ID_V) \
DEF_TYPE(ARRAY_MEMB_V) \
DEF_TYPE(FIELD_MEMB_V)


enum class ASTVarType
{
#define DEF_TYPE(v) v,
    VARTYPE_TABLE()
#undef DEF_TYPE
};
const char* ExpVarTypeName(ASTVarType);

// �﷨�����ʽ�ڵ�ļ������
#define EXP_CHECK_TYPE_TABLE()\
DEF_TYPE(VOID) \
DEF_TYPE(INTEGER) \
DEF_TYPE(BOOLEAN)


enum class ASTEXPCheckType
{
#define DEF_TYPE(v) v,
    EXP_CHECK_TYPE_TABLE()
#undef DEF_TYPE
};


// �﷨�����̽ڵ�Ĳ������ͣ�ֵ�κͱ��
#define PARAM_TYPE_TABLE()\
DEF_TYPE(NOT_PARAM) \
DEF_TYPE(VAL_PARAM_TYPE) \
DEF_TYPE(VAR_PARAM_TYPE)


enum class ASTParamType
{
#define DEF_TYPE(v) v,
    PARAM_TYPE_TABLE()
#undef DEF_TYPE
};


//  ���ʽ������
typedef struct ASTExpAttr
{
    ASTOpType op;   // ����������
    int val;    // ���ֽڵ���Ч����¼��ֵ
    ASTVarType varType;     // ��������
    ASTEXPCheckType type;   // ���ʽ�������
}ASTExpAttr;

//  ���̲�������������
typedef struct ASTProcAttr
{
    ASTParamType paramType;     // ���̲�������
}ASTProcAttr;


//  ��������������
typedef struct ASTArrayAttr
{
    int low;
    int up;
    ASTDecKind itemType;    // �����Ա���ͣ�ֻ����CHAR��INTEGER
}ASTArrayAttr;


#define MAXCHILDREN 3
//  �﷨�����ڵ㣬�������ֽڵ㶼�е�����
typedef struct ASTNodeBase
{
    ASTNodeBase() :lineNum(0), sibling(nullptr) { std::memset(child, 0, sizeof(child)); }
    int lineNum;
    ASTNodeKind nodeKind;
    ASTNodeBase* sibling;
    ASTNodeBase* child[MAXCHILDREN];
    std::vector<std::string> names;     //��ʶ������
    std::vector<unsigned long int> tablePtrs;   //������ʶ�����ű����
    friend std::ostream& operator<<(std::ostream&, const ASTNodeBase&);
}ASTNodeBase;


//  �﷨�������ڵ㣺�����ڵ�
//  nodeKindӦΪDEC_K
typedef struct ASTDecNode
{
    ASTNodeBase nodeBase;
    ASTDecKind decKind;     //������������
    std::string typeName;   //����Ϊ���ͱ�ʶ����ʾʱ��Ч
    union
    {
        ASTArrayAttr arrayAttr;     //��������ʱ��Ч��decKindΪARRAY_K��
        ASTProcAttr procAttr;   //�������̲���ʱ��Ч
    }decAttr;

}ASTDecNode;


//  �﷨�������ڵ㣺���ڵ�
//  nodeKindӦΪSTMT_K
typedef struct ASTStmtNode
{
    ASTNodeBase nodeBase;
    ASTStmtKind stmtKind;       //�����������
}ASTStmtNode;


//  �﷨�������ڵ㣺���ʽ�ڵ�
//  nodeKindӦΪEXP_K
typedef struct ASTExpNode
{
    ASTNodeBase nodeBase;
    ASTEXPKind expKind;     //������ʽ����
    ASTExpAttr expAttr;
}ASTExpNode;


ASTNodeBase* GetASTLabelNode(ASTNodeKind);
ASTDecNode* GetASTDecNode(ASTDecKind);
ASTStmtNode* GetASTStmtNode(ASTStmtKind);
ASTExpNode* GetASTExpNode(ASTEXPKind);
ASTExpNode* GetASTExpOpNode(ASTOpType);
ASTDecNode* GetASTDecNode();
ASTStmtNode* GetASTStmtNode();
ASTExpNode* GetASTExpNode();
ASTExpNode* GetASTExpConstNode();
ASTExpNode* GetASTExpVarNode();
ASTOpType TokenType2OpType(TokenType);
