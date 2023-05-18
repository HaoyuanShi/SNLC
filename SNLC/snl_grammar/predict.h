#pragma once
#include <string>
#include <vector>
#include <functional>
#include "../snl_common/token.h"


// ��ֹ�������ֹ��
#define SYMBOLTYPE_TABLE()\
DEF_TYPE(TERMINAL) \
DEF_TYPE(NON_TERMINAL)

enum class SymbolType
{
#define DEF_TYPE(v) v,
    SYMBOLTYPE_TABLE()
#undef DEF_TYPE
};

const char* SymbolTypeName(SymbolType);


#define NONTERMINAL_TABLE()\
DEF_TYPE(PROGRAM) \
DEF_TYPE(PROGRAM_HEAD) \
DEF_TYPE(PROGRAM_NAME) \
DEF_TYPE(DECLARE_PART) \
DEF_TYPE(TYPE_DECPART) \
DEF_TYPE(TYPE_DEC) \
DEF_TYPE(TYPE_DECLATRATION) \
DEF_TYPE(TYPE_DEC_LIST) \
DEF_TYPE(TYPE_DEC_MORE) \
DEF_TYPE(TYPE_ID) \
DEF_TYPE(TYPE_DEF) \
DEF_TYPE(BASE_TYPE) \
DEF_TYPE(STRUCT_TYPE) \
DEF_TYPE(ARRAY_TYPE) \
DEF_TYPE(LOW) \
DEF_TYPE(TOP) \
DEF_TYPE(REC_TYPE) \
DEF_TYPE(FIELD_DEC_LIST) \
DEF_TYPE(FIELD_DEC_MORE) \
DEF_TYPE(ID_LIST) \
DEF_TYPE(ID_MORE) \
DEF_TYPE(VAR_DEC_PART) \
DEF_TYPE(VAR_DEC) \
DEF_TYPE(VAR_DECLARATION) \
DEF_TYPE(VAR_DEC_LIST) \
DEF_TYPE(VAR_DEC_MORE) \
DEF_TYPE(VAR_ID_LIST) \
DEF_TYPE(VAR_ID_MORE) \
DEF_TYPE(PROC_DECPART) \
DEF_TYPE(PROC_DEC) \
DEF_TYPE(PROC_DEC_MORE) \
DEF_TYPE(PROC_NAME) \
DEF_TYPE(PARAM_LIST) \
DEF_TYPE(PARAM_DEC_LIST) \
DEF_TYPE(PARAM_MORE) \
DEF_TYPE(PARAM) \
DEF_TYPE(FORM_LIST) \
DEF_TYPE(FID_MORE) \
DEF_TYPE(PROC_DEC_PART) \
DEF_TYPE(PROC_BODY) \
DEF_TYPE(PROGRAM_BODY) \
DEF_TYPE(STM_LIST) \
DEF_TYPE(STM_MORE) \
DEF_TYPE(STM) \
DEF_TYPE(ASS_CALL) \
DEF_TYPE(ASSIGNMENT_REST) \
DEF_TYPE(CONDITIONAL_STM) \
DEF_TYPE(LOOP_STM) \
DEF_TYPE(INPUT_STM) \
DEF_TYPE(IN_VAR) \
DEF_TYPE(OUTPUT_STM) \
DEF_TYPE(RETURN_STM) \
DEF_TYPE(CALL_STM_REST) \
DEF_TYPE(ACT_PARAM_LIST) \
DEF_TYPE(ACT_PARAM_MORE) \
DEF_TYPE(REL_EXP) \
DEF_TYPE(OTHER_REL_E) \
DEF_TYPE(EXP) \
DEF_TYPE(OTHER_TERM) \
DEF_TYPE(TERM) \
DEF_TYPE(OTHER_FACTOR) \
DEF_TYPE(FACTOR) \
DEF_TYPE(VARIABLE) \
DEF_TYPE(VARI_MORE) \
DEF_TYPE(FIELD_VAR) \
DEF_TYPE(FIELD_VAR_MORE) \
DEF_TYPE(CMP_OP) \
DEF_TYPE(ADD_OP) \
DEF_TYPE(MULTI_OP)

enum class NonTerminalType
{
#define DEF_TYPE(v) v,
    NONTERMINAL_TABLE()
#undef DEF_TYPE
};

const char* NonTerminalTypeName(NonTerminalType);


typedef std::pair<NonTerminalType, TokenType> PredictTableKey;

std::pair<PredictTableKey, std::function<void()>> GetPredictItem(PredictTableKey, std::function<void()>);

typedef struct SymbolStackItem
{
    SymbolType symbolType;
    union 
    {
        NonTerminalType nonTerminalType;    // ����ֹ����Ч
        TokenType tokenType;    // ��ֹ����Ч
    }symbolName;
}SymbolStackItem;

SymbolStackItem GetTerminal(TokenType);
SymbolStackItem GetNonTerminal(NonTerminalType);


