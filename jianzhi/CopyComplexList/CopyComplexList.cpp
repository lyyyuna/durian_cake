#include <iostream>
#include <string>
#include "ComplexList.h"

using namespace::std;

void
clone_nodes(ComplexListNode* p_head)
{
    ComplexListNode* p_node = p_head;

    // insert the cloned node into p and p->next
    while (p_node != NULL) {
        ComplexListNode* p_cloned = new ComplexListNode();
        p_cloned->m_nvalue = p_node->m_nvalue;
        p_cloned->m_pnext = p_node->m_pnext;
        p_cloned->m_psibling = p_node->m_psibling;

        p_node->m_pnext = p_cloned;
        p_node = p_cloned->m_pnext;
    }
}

void
connect_siblingnodes(ComplexListNode* p_head)
{
    ComplexListNode* p_node = p_head;
    while (p_node != NULL) {
        ComplexListNode* p_cloned = p_node->m_pnext;
        if (p_node->m_psibling != NULL) 
            p_cloned->m_psibling = p_node->m_psibling->m_pnext;

        p_node = p_cloned->m_pnext;
    }
}

ComplexListNode*
reconnect_nodes(ComplexListNode* p_head)
{
    ComplexListNode* p_node = p_head;
    ComplexListNode* p_new_head = NULL;
    ComplexListNode* p_cloned_node = NULL;

    if (p_node != NULL) {
        p_new_head = p_node->m_pnext;
        p_cloned_node = p_new_head;

        p_node->m_pnext = p_node->m_pnext->m_pnext;
        p_node = p_node->m_pnext;
    }

    while (p_node != NULL) {
        p_cloned_node->m_pnext = p_node->m_pnext;
        p_cloned_node = p_cloned_node->m_pnext;

        p_node->m_pnext = p_cloned_node->m_pnext;
        p_node = p_node->m_pnext;
    }

    return p_cloned_node;
}   

ComplexListNode* 
clone(ComplexListNode* p_head)
{
    if (p_head == NULL)
        return NULL;

    clone_nodes(p_head);
    connect_siblingnodes(p_head);
    return reconnect_nodes(p_head);
}

void
test(string testname, ComplexListNode* p_head)
{
    cout << testname << " begins: " << endl;

    cout << "the orginal list is: " << endl;
    print_list(p_head);

    ComplexListNode* p_cloned = clone(p_head);
    cout << endl;
    cout << "the cloned list is: " << endl;
    print_list(p_head);
}

//          -----------------
//         \|/              |
//  1-------2-------3-------4-------5
//  |       |      /|\             /|\
//  --------+--------               |
//          -------------------------
void 
test1()
{
    ComplexListNode* p_node1 = create_node(1);
    ComplexListNode* p_node2 = create_node(2);
    ComplexListNode* p_node3 = create_node(3);
    ComplexListNode* p_node4 = create_node(4);
    ComplexListNode* p_node5 = create_node(5);

    build_nodes(p_node1, p_node2, p_node3);
    build_nodes(p_node2, p_node3, p_node5);
    build_nodes(p_node3, p_node4, NULL);
    build_nodes(p_node4, p_node5, p_node2);

    test("test1", p_node1);
}

int
main()
{
    test1();

    return 0;
}

