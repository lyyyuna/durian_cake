#include <iostream>
#include <vector>
#include <string>
#include "BinaryTree.h"

using namespace::std;

void dfs(BinaryTreeNode*, int, vector<int>&, int&);

void
find_path(BinaryTreeNode* p_root, int expectedsum)
{
    if (p_root == NULL)
        return ;

    vector<int> path;
    int cur_sum = 0;
    dfs(p_root, expectedsum, path, cur_sum);
}

void
dfs(BinaryTreeNode* p_root, int expectedsum, vector<int>& path, int& sum)
{
    sum += p_root->m_nvalue;
    path.push_back(p_root->m_nvalue);

    if (p_root->m_pleft == NULL && p_root->m_pright == NULL
                    && sum == expectedsum) {
        cout << "the path is " << endl;
        for (vector<int>::iterator iter = path.begin(); iter != path.end();
                        ++iter) {
            cout << *iter << " ";
        }
        cout << endl;
    }

    if (p_root->m_pleft != NULL)
        dfs(p_root->m_pleft, expectedsum, path, sum);
    if (p_root->m_pright != NULL)
        dfs(p_root->m_pright, expectedsum, path, sum);

    sum -= p_root->m_nvalue;
    path.pop_back();
}   

void
test(string testname, BinaryTreeNode* p_root, int expectedsum)
{
    cout << testname << " begins: " << endl;
    find_path(p_root, expectedsum);

    cout << endl;
}

//            10
//         /      \
//        5        12
//       / \
//      4   7     
// 有两条路径上的结点和为22
void
test1()
{
    BinaryTreeNode* p_node10 = create_binarytree_node(10);
    BinaryTreeNode* p_node5 = create_binarytree_node(5);
    BinaryTreeNode* p_node12 = create_binarytree_node(12);
    BinaryTreeNode* p_node4 = create_binarytree_node(4);
    BinaryTreeNode* p_node7 = create_binarytree_node(7);

    connect_binarytree_node(p_node10, p_node5, p_node12);
    connect_binarytree_node(p_node5, p_node4, p_node7);

    cout << "the paths: " << endl;
    test("test1", p_node10, 22);

    delete_binarytree(p_node10);
}

void
test2()
{
    test("test6", NULL, 0);
}

int main()
{
    test1();
    test2();

    return 0;
}
