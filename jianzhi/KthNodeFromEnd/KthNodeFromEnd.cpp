#include <iostream>
#include "List.h"

using namespace::std;

ListNode* 
find_kth(ListNode* p_head, int k)
{
    if (p_head == NULL || k <= 0)
        return NULL;

    ListNode* p1 = p_head;
    ListNode* p2 = NULL;

    for (int i = 0; i < k-1; ++i)
        if (p1->m_pNext != NULL)
            p1 = p1->m_pNext;
        else
            return NULL;
    
    p2 = p_head;
    while (p1->m_pNext != NULL) {
        p1 = p1->m_pNext;
        p2 = p2->m_pNext;
    }

    return p2;
}

void 
test1()
{
    cout << "======test1 starts:=======" << endl;
    ListNode* p_node1 = create_listnode(1);
    ListNode* p_node2 = create_listnode(2);
    ListNode* p_node3 = create_listnode(3);
    ListNode* p_node4 = create_listnode(4);
    ListNode* p_node5 = create_listnode(5);

    connect_listnode(p_node1, p_node2);
    connect_listnode(p_node2, p_node3);
    connect_listnode(p_node3, p_node4);
    connect_listnode(p_node4, p_node5);

    ListNode* p_node = find_kth(p_node1, 2);
    print_listnode(p_node);

    p_node = find_kth(p_node1, 5);
    print_listnode(p_node);

    p_node = find_kth(p_node1, 0);
    print_listnode(p_node);

    p_node = find_kth(NULL, 100);
    print_listnode(p_node);

    // return 0;

}

int
main()
{
    test1();

    return 0;
}
