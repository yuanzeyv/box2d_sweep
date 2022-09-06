#pragma once
#include <list>
template<typename ListType>
class AutomaticGenerator {
public:
	AutomaticGenerator(int allocStep = 100) :m_AllocStep(allocStep), m_AllocCount(0), m_IdleList() {}
	AutomaticGenerator::~AutomaticGenerator() {
		if (m_AllocCount > m_IdleList.size())
			printf("当前有%s对象%lld个未被回收,将造成内存泄漏,请检查代码逻辑\n\r", typeid(ListType).name(), m_AllocCount - m_IdleList.size());
		for (auto item = m_IdleList.begin(); item != m_IdleList.end(); item++)	delete* item;
		m_IdleList.erase(m_IdleList.begin(), m_IdleList.end()); //全部清理
	}
	ListType* RequireObj() {
		if (m_IdleList.size() == 0)//列表空了的话
			GenerateIdelListTypes();//1000个角色  
		auto beginObj = m_IdleList.begin();//获取到第一个
		ListType* listTypeObj = *beginObj;//获取到第一个的值
		m_IdleList.erase(beginObj);//删除第一个
		return listTypeObj;
	}
	void BackObj(ListType* obj) {//归还一个指针
		m_IdleList.push_back(obj);
	}
private:
	void GenerateIdelListTypes() {
		for (int i = 0; i < m_AllocStep; i++)
			m_IdleList.push_back(new ListType());//创建这么多个空闲对象
		m_AllocCount += m_AllocStep;
	}
private:
	int m_AllocStep;//分配步长
	int m_AllocCount;//分配的个数
	std::list<ListType*> m_IdleList;//当前空闲的个数
};
 