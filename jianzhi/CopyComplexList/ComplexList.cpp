#include <iostream>
#include "ComplexList.h"

using namespace::std;

ComplexListNode* 
create_node(int value)
{
    ComplexListNode* p_node = new ComplexListNode();
    p_node->m_nvalue = value;
    p_node->m_pnext = NULL;
    p_node->m_psibling = NULL;

    return p_node;
}

void
build_nodes(ComplexListNode* p_node, ComplexListNode* p_next, ComplexListNode* psibling)
{
    if (p_node != NULL) {
        p_node->m_pnext = p_next;
        p_node->m_psibling = psibling;
    }
}

void
print_list(ComplexListNode* p_head)
{
    ComplexListNode* p_node = p_head;
    while (p_node != NULL) {
        cout << "the value of this node is: " << p_node->m_nvalue << endl;

        if (p_node->m_psibling != NULL)
            cout << "the value of sibling is: " << p_node->m_psibling->m_nvalue << endl;
        else 
            cout << "do not have a sibling." << endl;

        cout << endl;

        p_node = p_node->m_pnext;
    }
} 
