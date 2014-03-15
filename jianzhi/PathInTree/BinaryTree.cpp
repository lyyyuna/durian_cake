#include <iostream>
#include "BinaryTree.h"

using namespace::std;

BinaryTreeNode* 
create_binarytree_node(int value)
{
    BinaryTreeNode* p_new = new BinaryTreeNode();
    p_new->m_nvalue = value;
    p_new->m_pleft = NULL;
    p_new->m_pright = NULL;

    return p_new;
}

void
connect_binarytree_node(BinaryTreeNode* parent, BinaryTreeNode* p_left, BinaryTreeNode* p_right)
{
    if (parent != NULL) {
        parent->m_pleft = p_left;
        parent->m_pright = p_right;
    }
}

void
print_binarytree_node(BinaryTreeNode* p_node)
{
    if (p_node != NULL) {
        cout << "value of this node is: " 
                << p_node->m_nvalue << endl;
        
        if (p_node->m_pleft != NULL) {
            cout << "the left is node is: "
                   << p_node->m_pleft->m_nvalue << endl;
        }

        if (p_node->m_pright != NULL) {
            cout << "the right node is: "
                    << p_node->m_pright->m_nvalue << endl;
        }
    } else {
        cout << "this node is null.\n" << endl;
    }

    cout << endl;
}

void
print_binarytree(BinaryTreeNode* p_root)
{
    print_binarytree_node(p_root);

    if (p_root != NULL) {
        if (p_root->m_pleft != NULL)
            print_binarytree(p_root->m_pleft);
        if (p_root->m_pright != NULL)
            print_binarytree(p_root->m_pright);                
    }
}   

void delete_binarytree(BinaryTreeNode* p_root)
{
    if (p_root != NULL) {
        BinaryTreeNode* p_left = p_root->m_pleft;
        BinaryTreeNode* p_right = p_root->m_pright;

        delete p_root;
        p_root = NULL;

        delete_binarytree(p_left);
        delete_binarytree(p_right);
    }
}


