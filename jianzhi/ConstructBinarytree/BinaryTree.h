#ifndef BINARY_TREE
#define BINARY_TREE

struct BinaryTreeNode
{
    int                 m_nvalue;
    BinaryTreeNode*     m_pleft;
    BinaryTreeNode*     m_pright;
};

extern BinaryTreeNode* create_binarytree_node(int value);
extern void connect_binarytree_node(BinaryTreeNode*, BinaryTreeNode*, BinaryTreeNode*);
extern void print_binarytree_node(BinaryTreeNode* p_node);
extern void print_binarytree(BinaryTreeNode*);
extern void delete_binarytree(BinaryTreeNode*);

#endif
