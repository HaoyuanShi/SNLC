#include "table.h"


Table::~Table() {


}
void Table::CreatTable() {
	/*
		������һ���µľֲ�����λʱ�����ñ��ӳ��򡣹����ǽ���һ����
		���ű�table,������1,ƫ�Ƴ�ʼ��Ϊ0��
	*/
	this->Level++;
	std::vector<symTablePtr> newitem;
	this->scope.push_back(newitem);
}

void Table::DestroyTable() {
	/*
		����һ�����ű�
		�ӷֳ�����е�����������ű���
	*/
	this->Access++;
	this->tables.push_back(this->scope.back());

	this->Level--;
	this->scope.pop_back();

}

bool Table::Enter(char* Id, AttributelR* AttribP, symTablePtr Entry) {
	/*
		����ʶ��������ʶ�����ԵǼ��ڷ��ű��У��Ǽǳɹ�������ֵΪ
		�棬Entryָ��Ǽǵı�ʶ���ڷ��ű��е�λ�ã��Ǽǲ��ɹ�������
		ֵΪ�٣�EntryֵΪ�ա�
	*/
	if (FindEntry(Id, true, Entry)) {
		//�����������
		std::cout << Id << " �ظ�����" << std::endl;
		return false;
	}

	symTablePtr item = new struct symTable;
	strcpy(item->idname, Id);
	item->next = nullptr;
	item->attrIR = *AttribP; //ǳ����
	

	std::vector<symTablePtr> table=this->scope.at(this->Level - 1);
	table.push_back(item);

	Entry = item;
	return true;
}

bool Table::FindEntry(char* id, bool diraction, symTablePtr Entry) {
	/*
		directionΪ������ײ����ң�Ϊ���򶥲�����
		����flag��ֵΪone����total,�����ڵ�ǰ���ű��в��ұ�ʶ����
		������scopeջ�е����з��ű��в��ұ�ʶ����
	*/
	if (diraction) {
		//Ϊ����ײ�����
		for (int level = this->Level-1; level >= 0; level--) {
			if (SearchoneTable(id, level, Entry)) return true;
		}
	}
	else {
		//Ϊ���򶥲�����
		for (int level = 0; level < this->Level - 1; level++) {
			if (SearchoneTable(id, level, Entry)) return true;
		}
	}
	Entry = nullptr;
	return false;
}

bool Table::SearchoneTable(char* id, int currentLevel, symTablePtr Entry) {
	/*
		�ӱ�ͷ��ʼ�����ν��ڵ��еı�ʶ�����ֺ�id�Ƚ��Ƿ���ͬ��ֱ
		���ҵ��˱�ʶ���򵽴��β�����ҵ���������ֵ��EntryΪ��ʶ��
		�ڷ��ű��е�λ�ã����򣬷���ֵΪ�١�
	*/
	std::vector<symTablePtr> table = this->scope.at(currentLevel);

	for (int i = 0; i < table.size(); i++) {
		if (strcmp(table.at(i)->idname, id) == 0) {
			Entry = table.at(i);
			return true;
		}
	}
	Entry = nullptr;
	return false;
}


void Table::initialize() {
	/*
		��ʼ���������ͣ��ַ����ͣ��������͵��ڲ���ʾ˵������������
		���;�Ϊ�������ͣ��ڲ���ʾ�̶���
	*/

	typeIR* item = new typeIR;
	item->kind = intTy;
	item->size = 1;
	this->typetabel["intTy"] = item;

	item = new typeIR;
	item->kind = charTy;
	item->size = 1;
	this->typetabel["charTy"] = item;

	item = new typeIR;
	item->kind = boolTy;
	item->size = 1;
	this->typetabel["boolTy"] = item;

}

TypeIR* Table::TypeProcess(ASTNodeBase* currentP) {
	/*
		���ͷ������������﷨���ĵ�ǰ�ڵ����ͣ����쵱ǰ���͵��ڲ�
		��ʾ���������ַ���ظ�Ptr�����ڲ���ʾ�ĵ�ַ��
	*/
	 ASTDecNode* tmp = (ASTDecNode*)&currentP;
	switch (tmp->decKind)
	{
	case ASTDecKind::ID_K:
		return this->nameType(currentP);
		break;
	case ASTDecKind::RECORD_K:
		return this->recordType(currentP);
		break;
	case ASTDecKind::ARRAY_K:
		return this->arrayType(tmp);
		break;
	case ASTDecKind::INTEGER_K:
		return this->typetabel.find("intTy")->second; break;
	case ASTDecKind::CHAR_K:
		return this->typetabel.find("intTy")->second; break;
	default:
		break;
	}

}

TypeIR* Table::arrayType(ASTDecNode* currentP) {
	/*
		�����������͵��ڲ���ʾ����ʱ�����±��Ƿ�Խ��
	*/
	if (currentP->decAttr.arrayAttr.up < currentP->decAttr.arrayAttr.low) {
		std::cout << "over boundary" << std:: endl;
		return nullptr;
	}

	typeIR* item = new typeIR;
	item->kind = arrayTy;
	item->size = currentP->decAttr.arrayAttr.up - currentP->decAttr.arrayAttr.low + 1;
	if (currentP->decAttr.arrayAttr.itemType == ASTDecKind::INTEGER_K) item->More.ArrayAttr.elemTy = this->typetabel["intTy"];
	else item->More.ArrayAttr.elemTy = this->typetabel["charTy"];
	//�±�����Ĭ��Ϊ����
	item->More.ArrayAttr.indexTy= this->typetabel["intTy"];
	//�����ڲ����ͱ���
	this->typetabel.insert(std::pair<std::string, TypeIR*>(currentP->nodeBase.names.back(), item));
	return item;
}


void Table::VarDecList(ASTNodeBase* currentP) {
	/*
		������������ʶ��idʱ����id�Ǽǵ����ű��У�����ظ��Զ��壻
		��������ʱ���������ڲ���ʾ��	
	*/
	const ASTDecNode* tmp = (const ASTDecNode*)&currentP;
	for (int i = 0; i < tmp->nodeBase.names.size(); ++i) {
		symTablePtr Entry=nullptr;
		char name[IDNAME_MAX_LEN];
		strcpy(name, tmp->nodeBase.names[i].c_str());
		if (this->FindEntry(name, true, Entry)) {
			std::cout << "re declare" << std::endl;
			return;
		}
	}

	currentP->tablePtrs.push_back(this->Access);//������ű����
	std::vector<symTablePtr> table = this->scope.at(this->Level - 1);
	if (tmp->decKind != ASTDecKind::ARRAY_K)
	{
		//�������������Ĵ���

		symTablePtr newsym = new struct symTable;
		newsym->attrIR.idtype = this->arrayType((ASTDecNode*)&currentP);
		newsym->attrIR.kind = varKind;
		newsym->attrIR.VarAttr.access = indir;
		newsym->attrIR.VarAttr.level = this->Level;
		newsym->attrIR.VarAttr.off = table.size();

		strcpy(newsym->idname, tmp->nodeBase.names.back().c_str());
		table.push_back(newsym);
	}
	else {
		//���������Ĵ���
		if (tmp->decKind == ASTDecKind::INTEGER_K) {
			for (int i = 0; i < tmp->nodeBase.names.size(); ++i) {
				AttributelR* item = new AttributelR;
				item->idtype = this->typetabel.find("intTy")->second;
				item->kind = varKind;

				symTablePtr Entry = nullptr;
				char name[IDNAME_MAX_LEN];
				strcpy(name, tmp->nodeBase.names[i].c_str());
				this->Enter(name, item, Entry);
			}
		}
		else {
			for (int i = 0; i < tmp->nodeBase.names.size(); ++i) {
				AttributelR* item = new AttributelR;
				item->idtype = this->typetabel.find("charTy")->second;
				item->kind = varKind;

				symTablePtr Entry = nullptr;
				char name[IDNAME_MAX_LEN];
				strcpy(name, tmp->nodeBase.names[i].c_str());
				this->Enter(name, item, Entry);
			}
		}
	}
}

void Table::ProcDecPart(ASTNodeBase* currentP) {
	/*
		�ڵ�ǰ����ű�����д���̱�ʶ�������ԣ����²���ű�����д��
		�α�ʶ�������ԡ�
	*/
	const ASTDecNode* tmp = (const ASTDecNode*)&currentP;
	for (int i = 0; i < tmp->nodeBase.names.size(); ++i) {
		symTablePtr Entry=nullptr;
		char name[IDNAME_MAX_LEN];
		strcpy(name, tmp->nodeBase.names[i].c_str());
		if (this->FindEntry(name, true, Entry)) {
			std::cout << "re declare" << std::endl;
			return;
		}
	}

	currentP->tablePtrs.push_back(this->Access);//������ű����
	std::vector<symTablePtr> table = this->scope.at(this->Level - 1);
	//�����������Ĵ���
	AttributelR* item = new AttributelR;
	item->idtype = nullptr;
	item->kind = procKind;
	item->More.level = this->Level;
	item->More.size = 1;
	item->More.param = nullptr;

	symTablePtr newsym = new struct symTable;
	strcpy(newsym->idname, currentP->names.back().c_str());
	table.push_back(newsym);

	//�����ӳ���
	this->CreatTable();
	//���������
	item->More.param = this->ParaDecList(currentP->child[0]);

}

symTablePtr Table::HeadProcess(ASTNodeBase* currentP) {

	/*
		�ڵ�ǰ����ű�����д������ʶ�������ԣ����²���ű�����д��
		�α�ʶ�������ԡ����й��̵Ĵ�С�ʹ������Ժ���
	*/

	return nullptr;
}

ParamTable* Table::ParaDecList(ASTNodeBase* currentP){
	/*
		���µķ��ű��еǼ������βεı�������βα���ĵ�ַ����
		ʹparamָ��˵�ַ��
	*/
	if (currentP == nullptr) return nullptr;
	this->VarDecList(currentP);//����table

	ParamTable* param = new struct ParamTable;
	param->entry = this->scope.at(this->Level - 1).size()-1;
	param->next = ParaDecList(currentP->sibling);

	return param;
}

void Table::Body(ASTNodeBase* currentP) {
	/*
		SNL����ϵͳ��ִ���岿�ּ�Ϊ������У���ֻ�账��������в�
		�֡�
	*/
	ASTNodeBase* tmp = currentP;
	while (tmp != nullptr) {
		this->statement(tmp);
		tmp = tmp->sibling;
	}
}

void Table::statement(ASTNodeBase* currentP) {
	/*
		�����﷨���ڵ��е�kind���ж�Ӧ��ת�����ĸ�������ͺ�����
	*/
	ASTStmtNode* tmp = ( ASTStmtNode*)&currentP;
	switch (tmp->stmtKind)
	{
	case ASTStmtKind::ASSIGN_K:
		this->assignstatement(tmp);
		break;
	case ASTStmtKind::IF_K:
		this->ifstatment(tmp);
		break;
	case ASTStmtKind::WHILE_K:
		this->whilestatement(tmp);
		break;
	case ASTStmtKind::CALL_K :
		this->callstatement(tmp);
		break;
	default:
		break;
	}
}


TypeIR* Table::arrayVar(ASTNodeBase* currentP) {
	/*
		���var:=var0[E]��var0�ǲ����������ͱ�����E�ǲ��Ǻ������
		�±��������ƥ�䡣
	*/
	symTablePtr Entry = nullptr;
	char name[IDNAME_MAX_LEN];
	strcpy(name, currentP->names[0].c_str());
	if (this->FindEntry(name, true, Entry)) {

		//�ж��Ƿ�Ϊ����
		if (Entry->attrIR.kind != arrayTy) {
			std::cout << currentP->names.back() << " id not array" << std::endl;
			return nullptr;
		}

		//�ж��±�����
		const ASTExpNode* exp = (const ASTExpNode*)&currentP->child[0];
		if (exp->expKind == ASTEXPKind::CONST_K&&Entry->attrIR.idtype->More.ArrayAttr.indexTy->kind!=intTy){
			std::cout << currentP->names.back() << " index type incorrect" << std::endl;
			return nullptr;
		}
		else if (exp->expKind == ASTEXPKind::ID_K) {
			symTablePtr indexEntry = nullptr;
			char name[IDNAME_MAX_LEN];
			strcpy(name, exp->nodeBase.names[0].c_str());
			if (this->FindEntry(name, true, indexEntry)) {
				std::cout << exp->nodeBase.names.back() << " not declare" << std::endl;
				return nullptr;
			}
			if (indexEntry->attrIR.kind != Entry->attrIR.kind) {
				std::cout << currentP->names.back() << " index type incorrect" << std::endl;
				return nullptr;
			}
		}
	}
	else {
		//��������δ����
		std::cout << currentP->names.back() << " not declare" << std::endl;
		return nullptr;
	}
	
	return Entry->attrIR.idtype;
}

TypeIR* Table::Expr(ASTNodeBase* currentP) {
	/*
		���ʽ����������ص��Ǽ��������������������ԣ�����ʽ��
		���͡����в���Ekind������ʾʵ���Ǳ�λ���ֵ�Ρ�
	*/
	const ASTExpNode* exp = (const ASTExpNode*)&currentP;

	if (exp->expKind == ASTEXPKind::OP_K) {
		TypeIR* exlL = this->Expr(currentP->child[0]);
		TypeIR* exlR = this->Expr(currentP->child[1]);

		if (exlL->kind != arrayTy && exlR->kind != arrayTy) {
			if (exlL->kind != exlR->kind) {
				std::cout << "different type can not op" << std::endl;
				exit(1);
			}
		}
		else if (exlL->kind == arrayTy) {
			if (exlR->kind == arrayTy) {
				if (exlL->More.ArrayAttr.elemTy->kind != exlR->More.ArrayAttr.elemTy->kind) {
					std::cout << "different type can not op" << std::endl;
					exit(1);
				}
			}
			else if (exlL->More.ArrayAttr.elemTy->kind != exlR->kind) {
				std::cout << "different type can not op" << std::endl;
				exit(1);
			}
			
		}
		else {
			if (exlR->More.ArrayAttr.elemTy->kind != exlL->kind) {
				std::cout << "different type can not op" << std::endl;
				exit(1);
			}
		}
		//����ʽ��������  ##tipsbool���Ͳ�������δ����
		if (exp->expAttr.op == ASTOpType::EQ || exp->expAttr.op==ASTOpType::LT) {
			
			TypeIR* typeir = new TypeIR;
			typeir->kind = boolTy;
			return typeir;
		}
		else if (exp->expAttr.op == ASTOpType::MINUS || exp->expAttr.op == ASTOpType::PLUS
			|| exp->expAttr.op == ASTOpType::PLUS || exp->expAttr.op == ASTOpType::OVER) {
			
			TypeIR* typeir = new TypeIR;
			if (exlL->kind == arrayTy) typeir->kind = exlL->More.ArrayAttr.elemTy->kind;
			else typeir->kind = exlL->kind;
			return exlL;
		}
	}
	else if(exp->expKind == ASTEXPKind::ID_K){

		symTablePtr ptr = nullptr;
		char name[IDNAME_MAX_LEN];
		strcpy(name, exp->nodeBase.names[0].c_str());
		this->FindEntry(name, true, ptr);
		
		return ptr->attrIR.idtype;
	}
	else {
		//Ĭ��const�������γ���
		TypeIR* typeir = new TypeIR;
		typeir->kind = intTy;
		return typeir;
	}
	
}

void Table::assignstatement(ASTStmtNode* currentP) {
	/*
		��ֵ��������������ص��Ǽ�鸳ֵ�����˷��������������ԡ�
		������߷�const ���������ͬ�����һ��λconst��һ������Ϊchar,�������ͼ��elem���Ͳ���Ϊchar
	*/
	symTablePtr Exp1 = nullptr,Exp2 = nullptr;
	const ASTExpNode* tmp1 = (const ASTExpNode*)&currentP->nodeBase.child[0];
	const ASTExpNode* tmp2 = (const ASTExpNode*)&currentP->nodeBase.child[1];
	char name1[IDNAME_MAX_LEN], name2[IDNAME_MAX_LEN];
	strcpy(name1, currentP->nodeBase.child[0]->names[0].c_str());
	strcpy(name2, currentP->nodeBase.child[0]->names[1].c_str());

	if (this->FindEntry(name1, true, Exp1) && this->FindEntry(name2, true, Exp2)) {

		//�������array��Ҫ����array EXP
		if (Exp1->attrIR.kind == arrayTy) this->arrayType(( ASTDecNode*)&currentP->nodeBase.child[0]);
		if (Exp2->attrIR.kind == arrayTy) this->arrayType(( ASTDecNode*)&currentP->nodeBase.child[1]);

		//��������������
		if (tmp1->expKind != ASTEXPKind::CONST_K && tmp2->expKind != ASTEXPKind::CONST_K) {
			if (Exp1->attrIR.idtype->kind != Exp2->attrIR.idtype->kind) {
				std::cout << "type unfit" << std::endl;
				return;
			}
		}
		else if (tmp1->expKind == ASTEXPKind::CONST_K && tmp2->expKind == ASTEXPKind::CONST_K) {
			std::cout << "const cant assign" << std::endl;
			return;
		}
		else {
			if (tmp1->expKind == ASTEXPKind::CONST_K) {
				
				if (Exp2->attrIR.idtype->kind == arrayTy&&Exp2->attrIR.idtype->More.ArrayAttr.elemTy->kind==charTy) {
					std::cout << "array elemtype unfit" << std::endl;
					return;
				}
				else if (Exp2->attrIR.idtype->kind == charTy) {
					std::cout << "type unfit" << std::endl;
					return;
				}
			}
			else {
				if (Exp1->attrIR.idtype->kind == charTy) {
					std::cout << "type unfit" << std::endl;
					return;
				}
			}
		}
	}
}

void Table::ifstatment(ASTStmtNode* currentP) {
	/*
		����������ʽ�Ƿ�Ϊbool���ͣ�����then������в��ֺ�else
		������в��֡�
	*/
	
	TypeIR* item = this->Expr(currentP->nodeBase.child[0]);
	if (item->kind != boolTy) {
		std::cout << "if condition stmt not bool" << std::endl;
		return;
	}

	//����then��else����
	for (int i = 1; i < 3; i++) this->Body(currentP->nodeBase.child[i]);
	
}


void Table::whilestatement(ASTStmtNode* currentP) {
	/*
		����������ʽ�Ƿ�Ϊbool���ͣ�����������в��֡�
	*/

	TypeIR* item = this->Expr(currentP->nodeBase.child[0]);
	if (item->kind != boolTy) {
		std::cout << "while condition stmt not bool" << std::endl;
		return;
	}

	//����whileѭ����
	this->Body(currentP->nodeBase.child[1]);

}

void Table::callstatement(ASTStmtNode* currentP) {
	/*
		����������������������ȼ����ű�����������е�Param��
		��(�βη��ű����ַ��),����������βκ�ʵ��֮��Ķ�Ӧ��ϵ
		�Ƿ���ȷ��
	*/

	const ASTExpNode* proc = (const ASTExpNode*)&currentP->nodeBase.child[0];
	const ASTExpNode* param = (const ASTExpNode*)&currentP->nodeBase.child[1];

	//��ѯ�������Ƿ����
	symTablePtr procEntry = nullptr;
	char name[IDNAME_MAX_LEN];
	strcpy(name, proc->nodeBase.names[0].c_str());
	if (!this->FindEntry(name, true,procEntry)) {
		std::cout << "procedure " << proc->nodeBase.names.back() << " not exist" << std::endl;
		return;
	}

	//��ѯ�����Ƿ���� ������ƥ��
	this->paramstatemnt(procEntry->attrIR.More.param, procEntry->attrIR.More.level, currentP->nodeBase.child[1]);
	
	return;
}

void Table::paramstatemnt(ParamTable* paramItem,int level,ASTNodeBase* currentP) {
	/*
		��ѯ�����Ƿ���� ������ƥ��
	*/
	if (currentP == nullptr) {
		if (paramItem != nullptr)std::cout << "param not emough" << std::endl;
		return;
	}

	const ASTExpNode* param = (const ASTExpNode*)&currentP;
	symTablePtr paramEntry = nullptr;
	char name[IDNAME_MAX_LEN];
	strcpy(name, param->nodeBase.names[0].c_str());
	if (paramItem!=nullptr &&this->FindEntry(name, true, paramEntry)) {
		if (this->scope.at(level).at(paramItem->entry)->attrIR.kind != paramEntry->attrIR.kind) {
			std::cout <<  " param  not same" << std::endl;
		}
	}
	else {
		if (paramItem != nullptr)	std::cout << " param  not declare" << std::endl;
		else std::cout << "too many param  " << std::endl;
	}

	this->paramstatemnt(paramItem->next, level, currentP->sibling);
}


void Table::writestatemen(ASTNodeBase* currentP) {
	/*
		���Ҫ����ı��������������Ƿ�Ϊ������
	*/
}

void Table::readstatemen(ASTNodeBase* currentP) {
	/*
		����д����еı��ʽ�Ƿ�Ϸ���
	*/

}

void Table:: Analyze(ASTNodeBase* currentP) {
	/*
		��������ܳ��򣬶��﷨�����з�����
	*/
	switch (currentP->nodeKind)
	{

	case ASTNodeKind::PRO_K:
		for (int i = 0; i < 3; ++i) {
			this->Analyze(currentP->child[i]);
		}
		break;
	case ASTNodeKind::PHEAD_K:
		break;
	case ASTNodeKind::TYPE_K:
		//�����ӽڵ���������ֵܽ��ΪVAR_K
		this->Analyze(currentP->child[0]);
		this->Analyze(currentP->sibling);
		break;
	case ASTNodeKind::VAR_K:
		//��������������ӽڵ�ΪDEC_K
		this->Analyze(currentP->child[0]);
		this->Analyze(currentP->sibling);
		break;
	case ASTNodeKind::PROC_K:
		//�����������
		this->Analyze(currentP->child[0]);
		break;
	case ASTNodeKind::STM_L_K:
		this->Body(currentP->child[0]);
		break;
	case ASTNodeKind::DEC_K:
		if (((const ASTDecNode*)&currentP)->decKind == ASTDecKind::PROC_DEC_K) {
			//�������
			this->ProcDecPart(currentP);
			for (int i = 1; i < 3; i++) {
				this->Analyze(currentP->child[i]);
			}
			this->Analyze(currentP->sibling);
			//�ӹ��̻���
			this->DestroyTable();
		}
		else{
			//�����������
			for (int i = 0; i < 3; ++i) {
				this->Analyze(currentP->child[i]);
			}
			this->Analyze(currentP->sibling);
		}
		break;
	default:
		break;
	}
}

void Table::PrintSymbTabl() {


	for (int i = 0; i < this->tables.size(); i++) {
		std::cout << std::endl << "=================================" << std::endl;
		std::vector<symTablePtr> table = this->tables.at(i);
		for (int j = 0; j < table.size(); i++) {
			std::cout << std::endl << "��������������������������������" << std::endl;
			std::cout << "|" << table.at(j)->idname << "\t" << praseIdKind(table.at(j)->attrIR.kind) << "\t" << praseTypeKind(table.at(j)->attrIR.idtype->kind);
		}
		std::cout << std::endl << "��������������������������������" << std::endl;
	}
}