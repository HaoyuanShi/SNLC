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

bool Table::Enter(char* Id, AttributelR* AttribP, symTablePtr *Entry) {
	/*
		����ʶ��������ʶ�����ԵǼ��ڷ��ű��У��Ǽǳɹ�������ֵΪ
		�棬Entryָ��Ǽǵı�ʶ���ڷ��ű��е�λ�ã��Ǽǲ��ɹ�������
		ֵΪ�٣�EntryֵΪ�ա�
	*/
	if (FindEntry(Id, true, *Entry)) {
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

	*Entry = item;
	return true;
}

bool Table::FindEntry(char* id, bool diraction, symTablePtr* Entry) {
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
	*Entry = nullptr;
	return false;
}

bool Table::SearchoneTable(char* id, int currentLevel, symTablePtr* Entry) {
	/*
		�ӱ�ͷ��ʼ�����ν��ڵ��еı�ʶ�����ֺ�id�Ƚ��Ƿ���ͬ��ֱ
		���ҵ��˱�ʶ���򵽴��β�����ҵ���������ֵ��EntryΪ��ʶ��
		�ڷ��ű��е�λ�ã����򣬷���ֵΪ�١�
	*/
	std::vector<symTablePtr> table = this->scope.at(currentLevel);

	for (int i = 0; i < table.size(); i++) {
		if (strcmp(table.at(i)->idname, id) == 0) {
			*Entry = table.at(i);
			return true;
		}
	}
	*Entry = nullptr;
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
	if (tmp->decKind == ASTDecKind::PROC_DEC_K)
	{
		//�����������Ĵ���
		AttributelR* item = new AttributelR;
		item->idtype = nullptr;
		item->kind = procKind;
		item->More.level = this->Level;
		item->More = 0;
		item->More.size = 1;
		item->More.param = nullptr;

		//һ�����̵Ĳ���������һ�����������
		if (this->scope.size() <= Level) {
			for (int i = tmp->nodeBase.child[0]->names.size(); i >=0 ; i--) {
				ParamTable* param = new struct ParamTable;
				param->entry = i;
				param->next = item->More.param;
				item->More.param = param;
			}
		}
		else {
			for (int i = tmp->nodeBase.child[0]->names.size(); i >= 0; i--) {
				ParamTable* param = new struct ParamTable;
				param->entry = i+this->scope.at(this->Level+1-1).size();
				param->next = item->More.param;
				item->More.param = param;
			}
		}
	}
}

void Table:: Analyze(ASTNodeBase* currentP) {
	/*
		��������ܳ��򣬶��﷨�����з�����
	*/
	switch (currentP->nodeKind)
	{

	case ASTNodeKind::PRO_K:
		this->CreatTable();
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
		this->Analyz(currentP->child[0]);
		break;
	case ASTNodeKind::STM_L_K:
		this->Analyze(currentP->child[0]);
		break;
	case ASTNodeKind::DEC_K:
		//�����������
		for (int i = 0; i < 3; ++i) {
			this->Analyze(currentP->child[i]);
		}
		this->Analyze(currentP->sibling);
		break;
	case ASTNodeKind::STMT_K:
		
		break;
	case ASTNodeKind::EXP_K:
		break;
	default:
		break;
	}


}