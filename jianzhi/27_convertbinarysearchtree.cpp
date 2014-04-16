#include <iostream>
#include <string>
#include <cstdio>

using namespace std;

struct BinaryTreeNode
{
    int                    m_nValue;
    BinaryTreeNode*        m_pLeft;
    BinaryTreeNode*        m_pRight;
};

BinaryTreeNode * CreateBinaryTreeNode(int value)
{
    BinaryTreeNode * pNode = new BinaryTreeNode();
    pNode->m_nValue = value;
    pNode->m_pLeft = NULL;
    pNode->m_pRight = NULL;

    return pNode;
}

void ConnectTreeNode(BinaryTreeNode * pParent, BinaryTreeNode * pLeft, BinaryTreeNode * pRight)
{
    if (pParent != NULL) {
        pParent->m_pLeft = pLeft;
        pParent->m_pRight = pRight;
    }
}

void PrintTreeNode(BinaryTreeNode *pNode)
{
    if (pNode != NULL) {
        cout << "the value of this node is: " << pNode->m_nValue;

        if (pNode->m_pLeft != NULL)
            cout << "the left child id " << pNode->m_pLeft->m_nValue;
        else
            cout << "the left child null " << endl;

        if (pNode->m_pRight != NULL)
            cout << "the right child is " << pNode->m_pRight->m_nValue;
        else
            cout << "the right child is null" << endl;
    }
    else {
        cout << "the node is null" << endl;
    }
}

void PrintTree(BinaryTreeNode * pHead)
{
    PrintTreeNode(pHead);

    if (pHead->m_pLeft != NULL)
        PrintTreeNode(pHead->m_pLeft);
    if (pHead->m_pRight != NULL)
        PrintTreeNode(pHead->m_pRight);
}

void ConvertNode(BinaryTreeNode * pNode, BinaryTreeNode ** pLastNodeInList)
{
    if (pNode == NULL)
        return ;

    BinaryTreeNode * pCurrent = pNode;

    if (pCurrent->m_pLeft != NULL)
        ConvertNode(pCurrent->m_pLeft, pLastNodeInList);

    pCurrent->m_pLeft = *pLastNodeInList;
    if (*pLastNodeInList != NULL)
        (*pLastNodeInList)->m_pRight = pCurrent;

    *pLastNodeInList = pCurrent;

    if (pCurrent->m_pRight != NULL)
        ConvertNode(pCurrent->m_pRight, pLastNodeInList);

}

BinaryTreeNode * Convert(BinaryTreeNode * pRoot)
{
    BinaryTreeNode * pLastNodeInList = NULL;
    ConvertNode(pRoot, &pLastNodeInList);

    BinaryTreeNode * pHead = pLastNodeInList;
    while (pHead != NULL && pHead->m_pLeft != NULL)
        pHead = pHead->m_pLeft;

    return pHead;
}

void PrintDoubleLinkedList(BinaryTreeNode* pHeadOfList)
{
    BinaryTreeNode* pNode = pHeadOfList;

    printf("The nodes from left to right are:\n");
    while(pNode != NULL)
    {
        printf("%d\t", pNode->m_nValue);

        if(pNode->m_pRight == NULL)
            break;
        pNode = pNode->m_pRight;
    }

    printf("\nThe nodes from right to left are:\n");
    while(pNode != NULL)
    {
        printf("%d\t", pNode->m_nValue);

        if(pNode->m_pLeft == NULL)
            break;
        pNode = pNode->m_pLeft;
    }

    printf("\n");
}

void Test(char* testName, BinaryTreeNode* pRootOfTree)
{
    if(testName != NULL)
        printf("%s begins:\n", testName);

    PrintTree(pRootOfTree);

    BinaryTreeNode* pHeadOfList = Convert(pRootOfTree);

    PrintDoubleLinkedList(pHeadOfList);
}

//            10
//         /      \
//        6        14
//       /\        /\
//      4  8     12  16
void Test1()
{
    BinaryTreeNode* pNode10 = CreateBinaryTreeNode(10);
    BinaryTreeNode* pNode6 = CreateBinaryTreeNode(6);
    BinaryTreeNode* pNode14 = CreateBinaryTreeNode(14);
    BinaryTreeNode* pNode4 = CreateBinaryTreeNode(4);
    BinaryTreeNode* pNode8 = CreateBinaryTreeNode(8);
    BinaryTreeNode* pNode12 = CreateBinaryTreeNode(12);
    BinaryTreeNode* pNode16 = CreateBinaryTreeNode(16);

    ConnectTreeNode(pNode10, pNode6, pNode14);
    ConnectTreeNode(pNode6, pNode4, pNode8);
    ConnectTreeNode(pNode14, pNode12, pNode16);

    Test("Test1", pNode10);

    //DestroyList(pNode4);
}

int main()
{
    Test1();

    return 0;
}
