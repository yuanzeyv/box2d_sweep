#pragma once
#include <list>
template<typename ListType>
class AutomaticGenerator {
public:
	AutomaticGenerator(int allocStep = 100) :m_AllocStep(allocStep), m_AllocCount(0), m_IdleList() {}
	AutomaticGenerator::~AutomaticGenerator() {
		if (m_AllocCount > m_IdleList.size())
			printf("��ǰ��%s����%lld��δ������,������ڴ�й©,��������߼�\n\r", typeid(ListType).name(), m_AllocCount - m_IdleList.size());
		for (auto item = m_IdleList.begin(); item != m_IdleList.end(); item++)	delete* item;
		m_IdleList.erase(m_IdleList.begin(), m_IdleList.end()); //ȫ������
	}
	ListType* RequireObj() {
		if (m_IdleList.size() == 0)//�б���˵Ļ�
			GenerateIdelListTypes();//1000����ɫ  
		auto beginObj = m_IdleList.begin();//��ȡ����һ��
		ListType* listTypeObj = *beginObj;//��ȡ����һ����ֵ
		m_IdleList.erase(beginObj);//ɾ����һ��
		return listTypeObj;
	}
	void BackObj(ListType* obj) {//�黹һ��ָ��
		m_IdleList.push_back(obj);
	}
private:
	void GenerateIdelListTypes() {
		for (int i = 0; i < m_AllocStep; i++)
			m_IdleList.push_back(new ListType());//������ô������ж���
		m_AllocCount += m_AllocStep;
	}
private:
	int m_AllocStep;//���䲽��
	int m_AllocCount;//����ĸ���
	std::list<ListType*> m_IdleList;//��ǰ���еĸ���
};
 