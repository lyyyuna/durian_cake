#include <iostream>
#include <string>
#include "BinaryTree.h"

using namespace::std;

bool dfs(BinaryTreeNode*, BinaryTreeNode*);

bool
has_subtree(BinaryTreeNode* p_root1, BinaryTreeNode* p_root2)
{
    bool res = false;

    if (p_root1 != NULL && p_root2 != NULL) {
        if (p_root1->m_nvalue == p_root2->m_nvalue) {
            res = dfs(p_root1, p_root2);
        }
        if (!res)
            res = has_subtree(p_root1->m_pleft, p_root2);
        if (!res)
            res = has_subtree(p_root1->m_pright, p_root2);
    }

    return res;
}   

bool
dfs(BinaryTreeNode* p_root1, BinaryTreeNode* p_root2)
{
    if (p_root2 == NULL)
        return true;
    if (p_root1 == NULL)
        return false;

    if (p_root1->m_nvalue != p_root2->m_nvalue)
        return false;

    return dfs(p_root1->m_pleft, p_root2->m_pleft) &&
            dfs(p_root1->m_pright, p_root2->m_pright);
}

void
test(string testname, BinaryTreeNode* p_root1, BinaryTreeNode* p_root2, bool expected)
{
    if (has_subtree(p_root1, p_root2) == expected)
        cout << testname << " passed." << endl;
    else
        cout << testname << " failed." << endl;
}

//
//         8
//       /   \
//      8      7
//     /  \
//    9    2
//        /  \
//       4    7
//
//         8
//        /  \
//       9    2
void
test1()
{
    BinaryTreeNode* p_nodea1 = create_binarytree_node(8);
    BinaryTreeNode* p_nodea2 = create_binarytree_node(8);
    BinaryTreeNode* p_nodea3 = create_binarytree_node(7);
    BinaryTreeNode* p_nodea4 = create_binarytree_node(9);
    BinaryTreeNode* p_nodea5 = create_binarytree_node(2);
    BinaryTreeNode* p_nodea6 = create_binarytree_node(4);
    BinaryTreeNode* p_nodea7 = create_binarytree_node(7);

    connect_binarytree_node(p_nodea1, p_nodea2, p_nodea3);
    connect_binarytree_node(p_nodea2, p_nodea4, p_nodea5);
    connect_binarytree_node(p_nodea5, p_nodea6, p_nodea7);

    BinaryTreeNode* p_nodeb1 = create_binarytree_node(8);
    BinaryTreeNode* p_nodeb2 = create_binarytree_node(9);
    BinaryTreeNode* p_nodeb3 = create_binarytree_node(2);

    connect_binarytree_node(p_nodeb1, p_nodeb2, p_nodeb3);

    test("test1", p_nodea1, p_nodeb1, true);

    delete_binarytree(p_nodea1);
    delete_binarytree(p_nodeb1);
}

//
//         8
//       /   \
//      8      7
//     /  \
//    9    2
//        /  \
//       4    7
//
//         8
//        /  \
//       2    2
void
test2()
{
    BinaryTreeNode* p_nodea1 = create_binarytree_node(8);
    BinaryTreeNode* p_nodea2 = create_binarytree_node(8);
    BinaryTreeNode* p_nodea3 = create_binarytree_node(7);
    BinaryTreeNode* p_nodea4 = create_binarytree_node(9);
    BinaryTreeNode* p_nodea5 = create_binarytree_node(2);
    BinaryTreeNode* p_nodea6 = create_binarytree_node(4);
    BinaryTreeNode* p_nodea7 = create_binarytree_node(7);

    connect_binarytree_node(p_nodea1, p_nodea2, p_nodea3);
    connect_binarytree_node(p_nodea2, p_nodea4, p_nodea5);
    connect_binarytree_node(p_nodea5, p_nodea6, p_nodea7);

    BinaryTreeNode* p_nodeb1 = create_binarytree_node(8);
    BinaryTreeNode* p_nodeb2 = create_binarytree_node(2);
    BinaryTreeNode* p_nodeb3 = create_binarytree_node(2);

    connect_binarytree_node(p_nodeb1, p_nodeb2, p_nodeb3);

    test("test1", p_nodea1, p_nodeb1, true);

    delete_binarytree(p_nodea1);
    delete_binarytree(p_nodeb1);
}

int
main()
{
    test1();
    test2();

    return 0;
}
