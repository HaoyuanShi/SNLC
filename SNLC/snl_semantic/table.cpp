#include "table.h"


Table::~Table() {


}
Table::CreatTable() {
	/*
		������һ���µľֲ�����λʱ�����ñ��ӳ��򡣹����ǽ���һ����
		���ű�table,������1,ƫ�Ƴ�ʼ��Ϊ0��
	*/
	this->Level++;
	this->scope.push_back(new std::vector<symTablePtr>);
}

Table::DestroyTable() {
	/*
		����һ�����ű�
	*/
	this->Level--;
	//���ٷ��ű�
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
		std::cout << Id << " �ظ�����" << endl;
		return false;
	}

	symTablePtr item = new struct symtabe;
	strcpy(item->idname, Id);
	item->next = nullptr;
	item->attrIR = AttribP; //ǳ����
	

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
		for (int level = this->scope.size() - 1; level >= 0; level--) {
			if (SearchoneTable(id, level, Entry)) return true;
		}
	}
	else {
		//Ϊ���򶥲�����
		for (int level = 0; level < this->scope.size(); level++) {
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
	const ASTDecNode* tmp = (const ASTDecNode*)&currentP;
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
		std::cout << "over boundary" << endl;
		return nullptr;
	}

	typeIR* item = new typeIR;
	item->kind = arrayTy;
	item->size = currentP->decAttr.arrayAttr.up - currentP->decAttr.arrayAttr.low + 1;
	if (currentP->decAttr.arrayAttr.itemType == ASTDecKind::INTEGER_K) item->More.ArrayAttr.elemTy = this->typetabel.find("intTy")->second;
	else item->More.ArrayAttr.elemTy = this->typetabel.find("charTy")->second;
	//�±�����Ĭ��Ϊ����
	item->More.ArrayAttr.indexTy= this->typetabel.find("intTy")->second;
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
		if (this->FindEntry(tmp->nodeBase.names.at(i), true, symTablePtr * Entry)) {
			std::cout << "re declare" << endl;
			return;
		}
	}

	std::vector<symTablePtr> table = this->scope.back();

	if (tmp->decKind != ASTDecKind::ARRAY_K)
	{
		//�������������Ĵ���

		AttributelR* item = new AttributelR;
		item->idtype = this->ProcDecPart(currentP);
		item->kind = arrayTy;
		item->VarAttr.access = indir;
		item->VarAttr.level = this->Level;
		item->VarAttr.off = table.size();

		symTablePtr newsym = new struct symTable;
		strcpy(newsym->idname, tmp->nodeBase.names.back());
		newsym->attrIR = item;
		table.push_back(newsym);
	}
	else {
		//���������Ĵ���
		if (tmp->decKind == ASTDecKind::INTEGER_K) {
			for (int i = 0; i < tmp->nodeBase.names.size(); ++i) {
				AttributelR* item = new AttributelR;
				item->idtype = this->typetabel.find("intTy")->second;
				item->kind = varKind;
				this->Enter(tmp->nodeBase.names.at(i), item, symTablePtr * Entry);
			}
		}
		else {
			for (int i = 0; i < tmp->nodeBase.names.size(); ++i) {
				AttributelR* item = new AttributelR;
				item->idtype = this->typetabel.find("charTy")->second;
				item->kind = varKind;
				this->Enter(tmp->nodeBase.names.at(i), item, symTablePtr * Entry);
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
		if (this->FindEntry(tmp->nodeBase.names.at(i), true, symTablePtr * Entry)) {
			std::cout << "re declare" << endl;
			return;
		}
	}

	std::vector<symTablePtr> table = this->scope.back();
	//�����������Ĵ���
	AttributelR* item = new AttributelR;
	item->idtype = nullptr;
	item->kind = procKind;
	item->More.level = this->Level;
	item->More = 0;
	item->More.size = 1;
	item->More.param = nullptr;

	symTablePtr newsym = new struct symTable;
	strcpy(newsym->idname, currentP->names.back());
	table.push_back(newsym);

	//�����ӳ���
	this->CreatTable();
	//���������
	item->More.param=this->ParaDecList(currentP->child[0])

}

symTablePtr Table::HeadProcess(ASTNodeBase* currentP) {

	/*
		�ڵ�ǰ����ű�����д������ʶ�������ԣ����²���ű�����д��
		�α�ʶ�������ԡ����й��̵Ĵ�С�ʹ������Ժ���
	*/


}

ParamTable* Table::ParaDecList(ASTNodeBase* currentP){
	/*
		���µķ��ű��еǼ������βεı�������βα���ĵ�ַ����
		ʹparamָ��˵�ַ��
	*/
	if (currentP == nullptr) return nullptr;
	this->VarDecList(currentP);//����table

	ParamTable* param = new struct ParamTable;
	param->entry = this->scope.back().size()-1;
	param->next = ParaDecList(currentP->sibling);

	return param;
}

void Table::Body(ASTNodeBase* currentP) {
	/*
		SNL����ϵͳ��ִ���岿�ּ�Ϊ������У���ֻ�账��������в�
		�֡�
	*/
	const ASTStmtNode* tmp = (const ASTStmtNode*)&currentP;
	while (tmp != nullptr) {
		this->statement(tmp);
		tmp = tmp->nodeBase.sibling;
	}
}

void Table::statement(ASTStmtNode* currentP) {
	/*
		�����﷨���ڵ��е�kind���ж�Ӧ��ת�����ĸ�������ͺ�����
	*/
	switch (currentP->stmtKind)
	{
	case ASTStmtKind::ASSIGN_K:
		this->assignstatement(currentP);
		break;
	case ASTStmtKind::IF_K:
		this->ifstatment(currentP);
		break;
	case ASTStmtKind::WHILE_K:
		this->whilestatement(currentP);
		break;
	case ASTStmtKind::CALL_K :
		this->callstatement(currentP);
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
	symTablePtr Entry;
	if (this->FindEntry(currentP->names.back(), true, Entry)) {

		//�ж��Ƿ�Ϊ����
		if (Entry->attrIR.kind != arrayTy) {
			std::cout << currentP->names.back() << " id not array" << std::endl;
			return nullptr;
		}

		//�ж��±�����
		const ASTExpNode* exp = (const ASTExpNode*)&currentP->child[0];
		if (exp->expKind == ASTEXPKind::CONST_K&&Entry->attrIR.idtype->More.ArrayAttr.indexTy->kind!=intTy){
			std::coutstd::cout << currentP->names.back() << " index type incorrect" << std::endl;
			return nullptr;
		}
		else if (exp->expKind == ASTEXPKind::ID_K) {
			symTablePtr indexEntry;
			if (this->FindEntry(exp->nodeBase.names.back(), true, indexEntry)) {
				std::coutstd::cout << exp->nodeBase.names.back() << " not declare" << std::endl;
				return nullptr;
			}
			if (indexEntry->attrIR.kind != Entry->attrIR.kind) {
				std::coutstd::cout << currentP->names.back() << " index type incorrect" << std::endl;
				return nullptr;
			}
		}
	}
	else {
		//��������δ����
		std::coutstd::cout << currentP->names.back() << " not declare" << std::endl;
		return nullptr;
	}
	
	return Entry->attrIR;
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
		else if (exlL.kind == arrayTy) {
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

		symTablePtr ptr;
		this->FindEntry(exp->nodeBase.names.back(), true, ptr);
		
		return ptr->attrIR;
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
	symTablePtr Exp1,Exp2;
	const ASTExpNode* tmp1 = (const ASTExpNode*)&currentP->nodeBase.child[0];
	const ASTExpNode* tmp2 = (const ASTExpNode*)&currentP->nodeBase.child[1];
	if (this->FindEntry(currentP->nodeBase.child[0]->names.back(), true, Exp1) && this->FindEntry(currentP->nodeBase.child[1]->names.back(), true, Exp2)) {

		//�������array��Ҫ����array EXP
		if (Exp1->attrIR.kind == arrayTy) this->arrayType(currentP->nodeBase.child[0]);
		if (ExpR->attrIR.kind == arrayTy) this->arrayType(currentP->nodeBase.child[1]);

		//��������������
		if (tmp1->expKind != ASTEXPKind::CONST_K && tmp2->expKind != ASTEXPKind::CONST_K) {
			if (Exp1->attrIR.idtype->kind != Exp2->attrIR.idtype->kind) {
				std::cout << "type unfit" << endl;
				return;
			}
		}
		else if (tmp1->expKind == ASTEXPKind::CONST_K && tmp2->expKind == ASTEXPKind::CONST_K) {
			std::cout << "const cant assign" << endl;
			return;
		}
		else {
			if (tmp1->expKind == ASTEXPKind::CONST_K) {
				
				if (Exp2->attrIR.idtype->kind == arrayTy&&Exp2->attrIR.idtype->More.ArrayAttr.elemTy->kind==charTy) {
					std::cout << "array elemtype unfit" << endl;
					return;
				}
				else if (Exp2->attrIR.idtype->kind == charTy) {
					std::cout << "type unfit" << endl;
					return;
				}
			}
			else {
				if (Exp1->attrIR.idtype->kind == charTy) {
					std::cout << "type unfit" << endl;
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
	symTablePtr procEntry;
	if (!this->FindEntry(proc->nodeBase.names.back(), true,procEntry)) {
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
	symTablePtr paramEntry;
	if (paramItem!=nullptr &&this->FindEntry(param->nodeBase.names.back(), true, paramEntry)) {
		if (this->scope.at(level).at(paramItem->entry)->attrIR.kind != paramEntry->attrIR.kind) {
			std::cout <<  " param  not same" << std::endl;
		}
	}
	else {
		if (paramItem != nullptr)	std::cout << " param  not declare" << std::endl;
		else std::cout << "too many param  " << std::endl;
	}

	this->paramstatemnt(paramItem->next,level,currentP->sibling)
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
		const ASTDecNode* tmp = (const ASTDecNode*)&currentP;
		if (tmp->decKind == ASTDecKind::PROC_DEC_K) {
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