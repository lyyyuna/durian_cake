#include <iostream>
#include <string>
#include "BinaryTree.h"

using namespace::std;

void convert_dfs(BinaryTreeNode* p_node, BinaryTreeNode** p_lastnode);

BinaryTreeNode*
convert(BinaryTreeNode* p_root)
{
    BinaryTreeNode* p_lastnode = NULL;
    convert_dfs(p_root, &p_lastnode);

    BinaryTreeNode* p_head = p_lastnode;
    while (p_head != NULL && p_head->m_pleft != NULL) {
        p_head = p_head->m_pleft;
    }

    return p_head;
}

void 
convert_dfs(BinaryTreeNode* p_node, BinaryTreeNode** p_lastnode)
{
    if (p_node == NULL)
        return ;

    BinaryTreeNode* p_cur = p_node;

    if (p_node->m_pleft != NULL)
        convert_dfs(p_cur->m_pleft, p_lastnode);

    p_node->m_pleft = *p_lastnode;
    // wrong!!! if the *p_lastnode is NULL
    // p_lastnode->m_pright = p_node;
    if ((*p_lastnode) != NULL)
        (*p_lastnode)->m_pright = p_cur;

    *p_lastnode = p_cur;

    if (p_node->m_pright != NULL)
        convert_dfs(p_cur->m_pright, p_lastnode);

}

void
print_double_list(BinaryTreeNode* p_head)
{
    BinaryTreeNode* p_node = p_head;

    cout << "the nodes from left to right: " << endl;
    while (p_node != NULL) {
        cout << p_node->m_nvalue << " ";
        if (p_node->m_pright == NULL)
            break;
        p_node = p_node->m_pright;
    }

    cout << "the nodes from right to left: " << endl;
    while (p_node != NULL) {
        cout << p_node->m_nvalue << " ";
        if (p_node->m_pleft == NULL)
            break;
        p_node = p_node->m_pleft;
    }

    cout << endl;
}

void
test(string testname, BinaryTreeNode* p_root)
{
    cout << testname << " begins: " << endl;

    BinaryTreeNode* p_head = convert(p_root);
    print_double_list(p_head);
}

//            10
//         /      \
//        6        14
//       /\        /\
//      4  8     12  16
//
void
test1()
{
    BinaryTreeNode* p_node10 = create_binarytree_node(10);
    BinaryTreeNode* p_node6 = create_binarytree_node(6);
    BinaryTreeNode* p_node14 = create_binarytree_node(14);
    BinaryTreeNode* p_node4 = create_binarytree_node(4);
    BinaryTreeNode* p_node8 = create_binarytree_node(8);
    BinaryTreeNode* p_node12 = create_binarytree_node(12);
    BinaryTreeNode* p_node16 = create_binarytree_node(16);

    connect_binarytree_node(p_node10, p_node6, p_node14);
    connect_binarytree_node(p_node6, p_node4, p_node8);
    connect_binarytree_node(p_node14, p_node12, p_node16);

    test("test1", p_node10);
}

int 
main()
{
    test1();

    return 0;
}

