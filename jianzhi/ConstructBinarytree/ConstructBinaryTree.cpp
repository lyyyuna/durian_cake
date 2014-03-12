#include <iostream>
#include <exception>
#include <string>
#include "BinaryTree.h"

using namespace::std;

BinaryTreeNode*
construct(int* start_preorder, int* end_preorder,
        int *start_inorder, int* end_inorder)
{
    int root_value = start_preorder[0];
    BinaryTreeNode* p_root = new BinaryTreeNode();
    p_root->m_nvalue = root_value;
    p_root->m_pright = p_root->m_pleft = NULL;

    if (start_preorder == end_preorder) {
        if (start_inorder == end_inorder && *start_preorder == *start_inorder)
            return p_root;
        else
            throw exception();
    }

    // find the value in inorder
    int* root_inorder = start_inorder ;
    while (root_inorder <= end_inorder && *root_inorder != root_value)
        root_inorder ++;

    if (root_inorder == end_inorder && *root_inorder != root_value)
        throw exception();

    int left_len = root_inorder - start_inorder;
    int* left_preorder_end = start_preorder + left_len;

    if (left_len > 0) {
        /* left tree */
        p_root->m_pleft = construct(start_preorder+1, left_preorder_end,
                        start_inorder, root_inorder-1);
    }
    if (left_len < end_preorder-start_preorder) {
        /* right tree */
        p_root->m_pright = construct(left_preorder_end+1, end_preorder,
                        root_inorder+1, end_inorder);
    }

    return p_root;

}

BinaryTreeNode* 
construct_tree(int* preorder, int* inorder, int len)
{
    if (preorder == NULL || inorder == NULL || len <= 0)
        return NULL;
    return construct(preorder, preorder+len-1, inorder, inorder+len-1);
}

void
test(string testname, int* preorder, int* inorder, int len)
{
    if (!testname.empty())
        cout << testname << " begins: " << endl;

    cout << "the preorder sequence is: " << endl;
    for (int i = 0; i < len; i ++) {
        cout << preorder[i];
    }
    cout << endl;

    cout << "the inorder sequence is: " << endl;
    for (int i = 0; i < len; i ++) {
        cout << inorder[i];
    }
    cout << endl;

    try {
        BinaryTreeNode* p_root = construct_tree(preorder, inorder, len);
        print_binarytree(p_root);

        delete_binarytree(p_root);
    } catch (exception& e){
        cout << "invalid input" << endl;
    }
}

/*
 *           1
 *        /     \
 *       2       3  
 *      /       /  \
 *     4       5    6
 *      \           /
 *       7         8
 */
void 
test1()
{
    const int len = 8;
    int preorder[] = {1, 2, 4, 7, 3, 5, 6, 8};
    int inorder[] = {4, 7, 2, 1, 5, 3, 8, 6};

    test("test 1", preorder, inorder, len);
}

/*
 *                  1
 *                 /
 *                2
 *               /
 *              3 
 *             /
 *            4
 *           /
 *          5
 */
void
test2()
{
    const int len = 5;
    int preorder[] = {1, 2, 3, 4, 5};
    int inorder[] = {5, 4, 3, 2, 1};

    test("test2", preorder, inorder, len);
}

/*
 * only 1 node
 *
 */
void
test3()
{
    const int len = 1;
    int preorder[] = {1};
    int inorder[] = {1};

    test("test3", preorder, inorder, len);
}

/*
 *              1
 *             /   \
 *            2    3
 *           / \  /  \
 *          4  5  6  7
 */
void
test4()
{
    const int len = 7;
    int preorder[] = {1, 2, 4, 5, 3, 6, 7};
    int inorder[] = {4, 2, 5, 1, 6, 3, 7};

    test("test4", preorder, inorder, len);
}

/*
 *  NULL
 */
void
test5()
{
    test("test5", NULL, NULL, 0);
}

/*
 *
 * the 2 order is not compatible
 *
 *
 */
void 
test6()
{
    const int len = 7;
    int preorder[] = {1, 2, 4, 5, 3, 6, 7};
    int inorder[] = {4, 2, 8, 1, 6, 3, 7};

    test("test6: for unmatched input", preorder, inorder, len);
}


int
main()
{
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();

    return 0;        
}
