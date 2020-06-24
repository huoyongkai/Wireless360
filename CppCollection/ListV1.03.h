
/**
 * @file
 * @brief List
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @version 1.01
 * @date Jun 15, 2003-Jan 23, 2013
 * @copyright None.
 * @note V1.01 add comments
*/
#ifndef _LIST_HYK_H_
#define _LIST_HYK_H_

//! the data node
template<class T>
struct Node
{
    //! data definition
    T data;
    //! pointer to the next node
    Node *next;
    //! constructor
    Node():next(0)
    {
    }
    //! constructor
    Node (const T& _data ):next(0)
    {
        data=_data;
    }
};

//! definition of template List class
template<class t>
class List
{
    //! the head node--storing invalid data
    Node<t> *head,*tail;
    //! temprary data node pointer for exchanging
    Node<t> *ptempNode;
public:
    //! constructor
    List();
    //! destructor
    ~List();
    /**
     * @brief add a not according to the data
     * @param _data the data to add
     * @return true if success
     */
    Node<t>* add (const t& _data);
    Node<t>* addtail (const t& _data);
    //! get the pointer of the fist node--the next node of the header
    inline Node<t>* first();
    //! get the next node
    inline Node<t>* next();
    //! get the head pointer
    inline Node<t>* gethead() const; //inline to run faster
    //! free all the space of the list
    void free();
    //! add a new node to the head
    Node<t>* add ( Node<t>* _pnode );
};

template<class t>
List<t>::List()
{
    head=new Node<t>;

    head->next=NULL;
    tail=head;
    ptempNode=NULL;
    //tail=NULL;
}

template<class t>
List<t>::~List()
{
    Node<t> *tmp;
    while ( head!=NULL )
    {
        tmp=head;
        head=head->next;
        delete tmp;
    }
}

template<class t>
inline Node<t> *List<t>::gethead() const
{
    return head;
}

template<class t>
Node<t>* List<t>::add ( const t& _data ) //donot use unless single data in node
{
    Node<t> *n=new Node<t> ( _data );
    if ( n!=NULL )
    {
        n->next=head->next;
        head->next=n;
        if(head==tail)
            tail=n;
        return n;
    }
    return 0;
}

template<class t>
Node<t>* List<t>::addtail ( const t& _data ) //donot use unless single data in node
{
    Node<t> *n=new Node<t> ( _data );
    if ( n!=NULL )
    {
        tail->next=n;
        tail=n;
        tail->next=NULL;
        return tail;
    }
    return 0;
}

template<class t>
Node<t>* List<t>::add ( Node< t >* _pnode ) //donot use unless single data in node
{
    if ( _pnode!=NULL )
    {
        _pnode->next=head->next;
        head->next=_pnode;
        if(head==tail)
            tail=_pnode;
        return _pnode;
    }
    return 0;
}

template<class t>
inline Node<t>* List<t>::first()
{
    return ptempNode=head->next;
}

template<class t>
inline Node<t>* List<t>::next()
{
    if ( ptempNode )
        return ptempNode=ptempNode->next;
    return NULL;
}

template<class t>
void  List<t>::free()
{
    Node<t> *p=head->next;
    Node<t> *temp;
    while ( p!=NULL )
    {
        temp=p;
        p=p->next;
        delete temp;
    }
    head->next=NULL;
    ptempNode=NULL;
}

typedef Node<double> Dnode;
typedef Node<int> Inode;
typedef List<double> Dlist;
typedef List<int> Ilist;
typedef List<string> Slist;
#endif
