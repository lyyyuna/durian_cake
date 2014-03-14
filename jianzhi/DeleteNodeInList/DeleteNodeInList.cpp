#include <iostream>
#include "List.h"

using namespace::std;

void
delete_node(ListNode** p_head, ListNode* node_delete)
{
    if (p_head == NULL || node_delete == NULL || *p_head == NULL)
        return;

    if (node_delete->m_pNext != NULL) {
        ListNode* p_new = node_delete->m_pNext;
        node_delete->m_nValue = p_new->m_nValue;
        node_delete->m_pNext = p_new->m_pNext;

        delete p_new;
        p_new = NULL;
    } else if (node_delete == *p_head) {
        delete node_delete;
        node_delete = NULL;
        *p_head = NULL;
    } else {
        ListNode* p_node = *p_head;
        while (p_node->m_pNext != node_delete) {
            p_node = p_node->m_pNext;
        }

        p_node->m_pNext = NULL;
        delete node_delete;
        node_delete = NULL;
    }
}

void
test(ListNode* p_head, ListNode* p_node)
{
    cout << "the original list is: " << endl;
    print_list(p_head);

    cout << "the node to ne deleted is: " << endl;
    print_listnode(p_node);

    delete_node(&p_head, p_node);

    cout << "the result is: " << endl;
    print_list(p_head);
}   

/* the deleted node is in the middle */
void
test1()
{
    ListNode* p_node1 = create_listnode(1);
    ListNode* p_node2 = create_listnode(2);
    ListNode* p_node3 = create_listnode(3);
    ListNode* p_node4 = create_listnode(4);
    ListNode* p_node5 = create_listnode(5);

    connect_listnode(p_node1, p_node2);
    connect_listnode(p_node2, p_node3);
    connect_listnode(p_node3, p_node4);
    connect_listnode(p_node4, p_node5);

    test(p_node1, p_node3);

    delete_list(p_node1);
}

/* many nodes, delete head */
void
test2()
{
    ListNode* p_node1 = create_listnode(1);
    ListNode* p_node2 = create_listnode(2);
    ListNode* p_node3 = create_listnode(3);
    ListNode* p_node4 = create_listnode(4);
    ListNode* p_node5 = create_listnode(5);

    connect_listnode(p_node1, p_node2);
    connect_listnode(p_node2, p_node3);
    connect_listnode(p_node3, p_node4);
    connect_listnode(p_node4, p_node5);

    test(p_node1, p_node1);

    delete_list(p_node1);
}

// many nodes, delete rear
void
test3()
{
    ListNode* p_node1 = create_listnode(1);
    ListNode* p_node2 = create_listnode(2);
    ListNode* p_node3 = create_listnode(3);
    ListNode* p_node4 = create_listnode(4);
    ListNode* p_node5 = create_listnode(5);

    connect_listnode(p_node1, p_node2);
    connect_listnode(p_node2, p_node3);
    connect_listnode(p_node3, p_node4);
    connect_listnode(p_node4, p_node5);

    test(p_node1, p_node5);

    delete_list(p_node1);
}

void
test4()
{
    ListNode* p_node1 = create_listnode(1);

    test(p_node1, p_node1);
}

void
test5()
{
    test(NULL, NULL);
}

int
main()
{
    test1();
    test2();
    test3();
    test4();
    test5();

    return 0;
}
