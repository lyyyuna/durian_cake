#include <iostream>
#include <stdlib.h>
#include "List.h"

using namespace::std;

ListNode * create_listnode(int value)
{
    ListNode *pNode = new ListNode();
    pNode->m_nValue = value;
    pNode->m_pNext = NULL;
    
    return pNode;
}

void
connect_listnode(ListNode *p_current, ListNode *p_next)
{
    if (p_current == NULL) {
        cout << "current node is null, cannot connect" << endl;
        exit(1);
    }

    p_current->m_pNext = p_next;
}

void
print_listnode(ListNode *p_node)
{
    if (p_node == NULL) {
        cout << "the node is null, cannot print" << endl;
    } else {
        cout << "the key in node is " << p_node->m_nValue << endl;
    }
}

void
print_list(ListNode * p_head)
{
    cout << "print list starts" << endl;
    
    ListNode * p_node = p_head;
    while (p_node != NULL) {
        cout << p_node->m_nValue << "   ";
        p_node = p_node->m_pNext;
    }

    cout << "print list ends" << endl;
}

void
delete_list(ListNode *p_head)
{
    ListNode *p_node = p_head;
    while (p_node != NULL) {
        p_head = p_head->m_pNext;
        delete p_node;
        p_node = p_head;
    }
}

void
insert_node(ListNode** p_head, int value)
{
    ListNode* p_new = new ListNode();
    p_new->m_nValue = value;
    p_new->m_pNext = NULL;

    if (*p_head == NULL) {
        *p_head = p_new;
    } else {
        ListNode* p_node = *p_head;
        while (p_node->m_pNext != NULL)
            p_node = p_node->m_pNext;

        p_node->m_pNext = p_new;
    }
}

void
delete_node(ListNode** p_head, int value)
{
    if (p_head == NULL || *p_head == NULL)
        return ;

    ListNode* node_delete = NULL;
    /* the delete node is the head */
    if ((*p_head)->m_nValue == value) {
        node_delete = *p_head;
        *p_head = (*p_head)->m_pNext;
    } else {
        ListNode* p_node = *p_head;

        while (p_node->m_pNext != NULL && p_node->m_pNext->m_nValue != value)
            p_node = p_node->m_pNext;

        if (p_node->m_pNext != NULL && p_node->m_pNext->m_nValue == value) {
            node_delete = p_node->m_pNext;
            p_node->m_pNext = p_node->m_pNext->m_pNext;
        }
    }

    if (node_delete != NULL) {
        delete node_delete;
        node_delete = NULL;
    }
}
