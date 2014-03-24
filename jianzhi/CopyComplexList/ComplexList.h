#ifndef COMPLEX_LIST
#define COMPLEX_LIST

struct ComplexListNode
{
    int             m_nvalue;
    ComplexListNode*    m_pnext;
    ComplexListNode*    m_psibling;
};

ComplexListNode* create_node(int value);
void build_nodes(ComplexListNode* p_node, ComplexListNode* p_next, ComplexListNode* psibling);
void print_list(ComplexListNode* p_head);


#endif
