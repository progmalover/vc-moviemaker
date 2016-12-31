#pragma once

#include <string>
#include <list>
#include <map>

class CSubject;

class CObserver
{
public:
	CObserver(){}
	virtual ~CObserver(){}

private:
	virtual void Update(void *pData) = 0;
	friend class CSubject;
};

typedef std::list <CObserver *> observer_list;
class CSubject
{
public:
	CSubject(){}
	virtual ~CSubject(){}

	virtual void Attach(CObserver *pObserver)
	{
		m_list.push_back(pObserver);
	}
	virtual void Detach(CObserver *pObserver)
	{
		m_list.remove(pObserver);
	}
	virtual void Notify(void *pData)
	{
		for (observer_list::iterator it = m_list.begin(); it != m_list.end(); it++)
			(*it)->Update(pData);
	}

private:
	observer_list m_list;
};


typedef std::map<unsigned short, CSubject*> subject_map;

class CSubjectManager
{
public:
    CSubjectManager(){}
    virtual ~CSubjectManager()
    {
        for (subject_map::iterator it = m_map.begin(); it != m_map.end(); it++)
        {
            delete (*it).second;
        }

        m_map.clear();
    }

    virtual void Register(unsigned short name, CSubject *subject)
    {
        m_map[name] = subject;
    }

    virtual void UnRegister(unsigned short name)
    {
        m_map.erase(name);
    }

    virtual CSubject *GetSubject(unsigned short name)
    {
        subject_map::iterator it = m_map.find(name);
        if (it != m_map.end())
            return (*it).second;
        else
            return NULL;
    }

    static CSubjectManager *Instance()
    {
	    if (!m_pInstance)
		    m_pInstance = new CSubjectManager;
	    return m_pInstance;
    }
    static void ReleaseInstance()
    {
	    delete m_pInstance;
	    m_pInstance = NULL;
    }

private:
    subject_map m_map;

    static CSubjectManager *m_pInstance;
};

#define DECLARE_OBSERVER(ContainerClass, Subject)								\
virtual void Update##Subject(void *pData);										\
class CObserver##Subject : public CObserver										\
{																				\
public:																			\
	CObserver##Subject()														\
	{																			\
		CSubjectManager::Instance()->GetSubject(Subject)->Attach(this);		\
	}																			\
private:																		\
	virtual void Update(void *pData)											\
	{																			\
		METHOD_PROLOGUE(ContainerClass, CObserver##Subject);					\
		pThis->Update##Subject(pData);											\
	}																			\
}m_xCObserver##Subject;

#define IMPLEMENT_OBSERVER(ContainerClass, Subject)								\
	void ContainerClass::Update##Subject(void *pData)

