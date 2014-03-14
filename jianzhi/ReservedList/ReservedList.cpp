#include <iostream>
#include "List.h"

using namespace::std;

ListNode*
reserved_list(ListNode* p_head)
{
    if (p_head == NULL)
        return NULL;

    ListNode* p_rear = NULL;
    ListNode* p_node = p_head;
    ListNode* p_prev = NULL;

    while (p_node != NULL) {
        ListNode* p_next = p_node->m_pNext;

        if (p_next == NULL)
            p_rear = p_node;

        p_node->m_pNext = p_prev;
        p_prev = p_node;
        p_node = p_next;
    }

    return p_rear;
}

void
test(ListNode* p_head)
{
    cout << "the orgi list is: " << endl;
    print_list(p_head);

    ListNode* p_rear = reserved_list(p_head);

    cout << "the reserved list is: " << endl;
    print_list(p_rear);

    delete_list(p_rear);
}

/*
 * many nodes
 *
 */
void
test1()
{
    ListNode* p_node1 = create_listnode(1);
    ListNode* p_node2 = create_listnode(2);
    ListNode* p_node3 = create_listnode(3);
    ListNode* p_node4 = create_listnode(4);
    ListNode* p_node5 = create_listnode(4);

    connect_listnode(p_node1, p_node2);
    connect_listnode(p_node2, p_node3);
    connect_listnode(p_node3, p_node4);
    connect_listnode(p_node4, p_node5);

    test(p_node1);

    cout << endl;

}

/*
 * only one node
 */
void
test2()
{
    ListNode* p_node1 = create_listnode(1);
    
    test(p_node1);

    cout << endl;
}

/*
 * NULL
 */
void
test3()
{
    test(NULL);

    cout << endl;
}

int
main()
{
    test1();
    test2();
    test3();

    return 0;
}
