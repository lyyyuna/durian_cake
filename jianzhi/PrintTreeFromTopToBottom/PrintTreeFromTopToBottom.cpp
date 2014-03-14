#include <iostream>
#include <string>
#include <queue>
#include "BinaryTree.h"

using namespace::std;

void
bfs(BinaryTreeNode* p_root)
{
    if (p_root == NULL)   
        return ;

    queue<BinaryTreeNode*> queue_tree;

    queue_tree.push(p_root);

    while (!queue_tree.empty()) {
        BinaryTreeNode* p_node = queue_tree.front();
        queue_tree.pop();

        cout << p_node->m_nvalue << " ";

        if (p_node->m_pleft)
            queue_tree.push(p_node->m_pleft);
        if (p_node->m_pright)
            queue_tree.push(p_node->m_pright);
    }   
}

void
test(string testname, BinaryTreeNode* p_root)
{
    cout << testname << " begins: " << endl;

    bfs(p_root);

    cout << endl;
}

//            10
//         /      \
//        6        14
//       /\        /\
//      4  8     12  16
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

    delete_binarytree(p_node10);
}

void
test2()
{
    test("test2", NULL);
}

int
main()
{
    test1();
    test2();

    return 0;
}
