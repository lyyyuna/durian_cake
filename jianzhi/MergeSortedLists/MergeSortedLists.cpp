#include <iostream>
#include <string>
#include "List.h"

using namespace::std;

ListNode*
merge(ListNode* p_head1, ListNode* p_head2)
{
    if (p_head1 == NULL)
        return p_head2;
    else if (p_head2 == NULL)
        return p_head1;

    ListNode* p_newhead = NULL;

    if (p_head1->m_nValue < p_head2->m_nValue) {
        p_newhead = p_head1;
        p_newhead->m_pNext = merge(p_head1->m_pNext, p_head2);
    } else {
        p_newhead = p_head2;
        p_newhead->m_pNext = merge(p_head1, p_head2->m_pNext);
    }

    return p_newhead;
}

void
test(string testname, ListNode* p_head1, ListNode* p_head2)
{
    cout << testname << " begins: " << endl;

    cout << "the first list is: " << endl;
    print_list(p_head1);

    cout << "the second list is: " << endl;
    print_list(p_head2);

    cout << "the merged list is: " << endl;
    ListNode* p_node = merge(p_head1, p_head2);
    print_list(p_node);

    delete_list(p_node);

    cout << endl;

    return ;
}

void
test1()
{
    ListNode* p_node1 = create_listnode(1);
    ListNode* p_node3 = create_listnode(3);
    ListNode* p_node5 = create_listnode(5);

    connect_listnode(p_node1, p_node3);
    connect_listnode(p_node3, p_node5);

    ListNode* p_node2 = create_listnode(2);
    ListNode* p_node4 = create_listnode(4);
    ListNode* p_node6 = create_listnode(6);

    connect_listnode(p_node2, p_node4);
    connect_listnode(p_node4, p_node6);

    test("test1", p_node1, p_node2);
}

int
main()
{
    test1();

    return 0;
}
