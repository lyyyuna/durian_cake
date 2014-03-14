#include <iostream>
#include <string>
#include <stack>
#include "BinaryTree.h"

using namespace::std;


void 
mirror_recursive(BinaryTreeNode* p_root)
{
    if ((p_root == NULL) || 
        (p_root->m_pleft == NULL && p_root->m_pright == NULL))
        return ;

    BinaryTreeNode* p_temp = p_root->m_pleft;
    p_root->m_pleft = p_root->m_pright;
    p_root->m_pright = p_temp;

    if (p_root->m_pleft != NULL)
        mirror_recursive(p_root->m_pleft);
    if (p_root->m_pright != NULL)
        mirror_recursive(p_root->m_pright);
}

void
mirror_iterative(BinaryTreeNode* p_root)
{
    if ((p_root == NULL) || 
        (p_root->m_pleft == NULL && p_root->m_pright == NULL))
        return ;

    stack<BinaryTreeNode*> stackBT;
    stackBT.push(p_root);

    while (!stackBT.empty()) {
        BinaryTreeNode* p_node = stackBT.top();
        stackBT.pop();


        BinaryTreeNode* p_temp = p_root->m_pleft;
        p_root->m_pleft = p_root->m_pright;
        p_root->m_pright = p_temp; 

        if (p_root->m_pleft != NULL)
            stackBT.push(p_root->m_pleft);

        if (p_root->m_pright != NULL)
            stackBT.push(p_root->m_pright);

    }
}

// ====================测试代码====================
// 测试完全二叉树：除了叶子节点，其他节点都有两个子节点
//            8
//        6      10
//       5 7    9  11
//
void 
test1()
{
    cout << "======test1======" << endl;
    BinaryTreeNode* p_node8 = create_binarytree_node(8);
    BinaryTreeNode* p_node6 = create_binarytree_node(6);
    BinaryTreeNode* p_node10 = create_binarytree_node(10);
    BinaryTreeNode* p_node5 = create_binarytree_node(5);
    BinaryTreeNode* p_node7 = create_binarytree_node(7);
    BinaryTreeNode* p_node9 = create_binarytree_node(9);
    BinaryTreeNode* p_node11 = create_binarytree_node(11);

    connect_binarytree_node(p_node8, p_node6, p_node10);
    connect_binarytree_node(p_node6, p_node5, p_node7);
    connect_binarytree_node(p_node10, p_node9, p_node11);

    print_binarytree(p_node8);

    cout << "test1: rescursive" <<  endl;
    mirror_recursive(p_node8);
    print_binarytree(p_node8);

    cout << "test1: iterative" << endl;
    mirror_recursive(p_node8);
    print_binarytree(p_node8);

    delete_binarytree(p_node8);

    cout << "test ends" << endl;
}

int
main()
{
    test1();

    return 0;
}
