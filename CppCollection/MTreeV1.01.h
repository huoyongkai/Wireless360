
/**
 * @file
 * @brief m-tree class designed for implementation of IniParser
 * @version 1.00
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com)
 * @date Oct 9, 2014-Oct 9, 2014
 * @copyright None.
*/

#ifndef _MTREE_H_
#define _MTREE_H_
#include "Headers.h"
template<class T>
struct MNode
{
    static const int MIN_ASCII=33;
    static const int MAX_ASCII=127;
    string m_key;
    T m_val;
    int m_childInd;;
    Vec<MNode <T>*> m_children;//[MAX_ASCII-MIN_ASCII+1];
    MNode();
    MNode<T>* Child(const char& _key, MNode**& _refchild);
};

template<class Mt>
class MTree
{
    MNode<Mt>* m_root;
    int m_childCounter;
    void Free(MNode<Mt>* _proot);
    void Out(MNode<Mt>* _proot, Array< string >& _strs, string& _tmpkey);
public:
    MTree();
    ~MTree();
    int NumofChildren();
    bool Exist(const string& _key,MNode<Mt>*& _pnode);
    MNode<Mt>* RepInsert(const string& _key, const Mt& _val);
    void Out(Array<string>& _strs);
};


template <class T>
MNode<T>::MNode()
{
    m_children.set_size(MAX_ASCII-MIN_ASCII+1);
    m_children.zeros();
    m_childInd=-1;
    m_key="";
}

template <class T>
MNode<T>* MNode<T>::Child(const char& _val, MNode**& _refchild)
{
//     if(_refchild!=NULL)
    _refchild=&(m_children[_val-MIN_ASCII]);
    return m_children[_val-MIN_ASCII];
}

template <class Mt>
MTree<Mt>::MTree()
{
    m_childCounter=0;
    m_root=new MNode<Mt>();
}

template <class Mt>
MTree<Mt>::~MTree()
{
    Free(m_root);
}

template <class Mt>
void MTree<Mt>::Free(MNode<Mt>* _proot)
{
    for(int i=0; i<_proot->m_children.length(); i++)
    {
        if(_proot->m_children[i])
            Free(_proot->m_children[i]);
    }
    delete _proot;
}

template <class Mt>
bool MTree<Mt>::Exist(const string& _key, MNode< Mt >*& _pnode)
{
    MNode<Mt>* pcurr=m_root;
    MNode<Mt>** useless=0;
    int ind=0;
    while(ind<_key.length())
    {
        MNode<Mt>* temp=pcurr->Child(_key[ind],useless);
        if(temp==NULL)
	{
	    _pnode=0;
            return false;
	}
        pcurr=temp;
        ind++;
    }
    _pnode=pcurr->m_childInd>=0?pcurr:NULL;
    return _pnode!=NULL;
}

template <class Mt>
MNode<Mt>* MTree<Mt>::RepInsert(const string& _key, const Mt& _val)
{
    MNode<Mt>* pcurr=m_root;
    int ind=0;
    while(ind<_key.length())
    {
        MNode<Mt>** temp;
        pcurr->Child(_key[ind],temp);
        if(*temp==NULL)
        {
            *temp=new MNode<Mt>();
            (*temp)->m_key=_key[ind];
        }
        pcurr=*temp;
        ind++;
    }
    pcurr->m_val=_val;
    if(pcurr->m_childInd<0)
        pcurr->m_childInd=m_childCounter++;
    return pcurr;
}

template <class Mt>
void MTree<Mt>::Out(MNode<Mt>* _proot, Array< string >& _strs, string& _tmpkey)
{
    if(_proot==NULL)
        return;
    string key=_tmpkey+_proot->m_key;
    if(_proot->m_childInd>=0)
        _strs(_proot->m_childInd)= key;

    for(int i=0; i<_proot->m_children.length(); i++)
    {
        if(_proot->m_children[i])
            Out(_proot->m_children[i],_strs,key);        
    }
}

template <class Mt>
void MTree<Mt>::Out(Array< string >& _strs)
{
    _strs.set_size(m_childCounter);
//     cout<<"number="<<m_childCounter<<endl;
    string key="";
    Out(m_root,_strs,key);
}

template <class Mt>
int MTree<Mt>::NumofChildren()
{
    return m_childCounter;
}
#endif // _MTREE_H_
