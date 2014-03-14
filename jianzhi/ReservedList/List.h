#ifndef LIST_H
#define LIST_H

struct ListNode
{
    int         m_nValue;
    ListNode    *m_pNext;
};

extern ListNode *create_listnode(int value);
extern void connect_listnode(ListNode* p_cur, ListNode* p_next);
extern void print_listnode(ListNode* p_node);
extern void print_list(ListNode* p_head);
extern void delete_list(ListNode* p_head);
extern void insert_node(ListNode** p_head, int value);
extern void delete_node(ListNode** p_head, int value);

#endif
