#include <iostream>
#include <stack>
#include "list.h"

using namespace::std;

void print_reversed_list_iter(ListNode* p_head)
{
    stack<ListNode*> nodes;

    ListNode* p_node = p_head;
    while (p_node != NULL) {
        nodes.push(p_node);
        p_node = p_node->m_pNext;
    }

    while (!nodes.empty()) {
        p_node = nodes.top();
        cout << p_node->m_nValue << " " << endl;
        nodes.pop();
    }
}

void
print_reversed_list_recur(ListNode* p_head)
{
    if (p_head != NULL) {
        if(p_head->m_pNext != NULL) {
            print_reversed_list_recur(p_head->m_pNext);
        }

        cout << p_head->m_nValue << " ";
    }
}

void
test(ListNode* p_head)
{
    print_list(p_head);
    print_reversed_list_iter(p_head);
    print_reversed_list_recur(p_head);
    cout << endl;
}


void
test1()
{
    cout << endl;
    cout << "test1 begins." << endl;

    ListNode* p_node1 = create_listnode(1);
    ListNode* p_node2 = create_listnode(2);
    ListNode* p_node3 = create_listnode(3);
    ListNode* p_node4 = create_listnode(4);
    ListNode* p_node5 = create_listnode(5);

    connect_listnode(p_node1, p_node2);
    connect_listnode(p_node2, p_node3);
    connect_listnode(p_node3, p_node4);
    connect_listnode(p_node4, p_node5);

    test(p_node1);

    delete_list(p_node1);
}


int
main()
{
    cout << "hello, world." << endl;

    test1();

    return 0;
}
