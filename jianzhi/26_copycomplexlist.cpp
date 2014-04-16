#include <iostream>
#include <memory>
#include <algorithm>
#include <vector>

using namespace std;

struct ComplexListNode
{
    int m_nValue;
    ComplexListNode * m_pNext;
    ComplexListNode * m_pSibling;
};

ComplexListNode * CreateNode(int nValue)
{
    ComplexListNode * p_Temp = new ComplexListNode();
    p_Temp->m_nValue = nValue;
    p_Temp->m_pNext = NULL;
    p_Temp->m_pSibling = NULL;

    return p_Temp;
}

void BuildNodes(ComplexListNode * pNode, ComplexListNode * pNext, ComplexListNode * pSibling)
{
    if (pNode != NULL) {
        pNode->m_pNext = pNext;
        pNode->m_pSibling = pSibling;
    }
}

void PrintList(ComplexListNode * pHead)
{
    ComplexListNode * pTemp = pHead;
    while (pTemp != NULL) {
        cout << "the value of this node is." << pTemp->m_nValue;
        if (pTemp->m_pSibling != NULL)
            cout << " the value of sibling node is " << pTemp->m_pSibling->m_nValue;
        else
            cout << " this node don't have sibling node." ;

        cout << endl;

        pTemp = pTemp->m_pNext;
    }
}

void CloneNodes(ComplexListNode * pHead)
{
    ComplexListNode * pTemp = pHead;
    while (pTemp != NULL) {
        ComplexListNode * pCloned = new ComplexListNode();
        pCloned->m_pNext = pTemp->m_pNext;
        pCloned->m_nValue = pTemp->m_nValue;
        pCloned->m_pSibling = NULL;

        pTemp->m_pNext = pCloned;

        pTemp = pCloned->m_pNext;
    }
}

void ConnectSiblingNodes(ComplexListNode * pHead)
{
    ComplexListNode * pTemp = pHead;
    while (pTemp != NULL) {
        ComplexListNode * pCloned = pTemp->m_pNext;
        if (pTemp->m_pSibling != NULL)
            pCloned->m_pSibling = pTemp->m_pSibling->m_pNext;

        pTemp = pCloned->m_pNext;
    }
}

ComplexListNode * ReconnectNodes(ComplexListNode * pHead)
{
    ComplexListNode * pTemp = pHead;
    ComplexListNode * pClonedHead = NULL;
    ComplexListNode * pClonedTemp = NULL;

    if (pTemp != NULL) {
        pClonedHead = pClonedTemp = pTemp->m_pNext;
        pTemp->m_pNext = pClonedHead->m_pNext;
        pTemp = pTemp->m_pNext;
    }

    while (pTemp != NULL) {
        pClonedTemp->m_pNext = pTemp->m_pNext;
        pClonedTemp = pClonedTemp->m_pNext;

        pTemp->m_pNext = pClonedTemp->m_pNext;
        pTemp = pTemp->m_pNext;
    }

    return pClonedHead;
}

ComplexListNode * Clone(ComplexListNode * pHead)
{
    CloneNodes(pHead);
    ConnectSiblingNodes(pHead);
    ReconnectNodes(pHead);
}

void test(string testname, ComplexListNode * pHead)
{
    cout << testname << " begins: " << endl;

    cout << "the original list is " << endl;
    PrintList(pHead);

    cout << endl;
    ComplexListNode * pClonedHead = Clone(pHead);
    cout << "the cloned list is " << endl;
    PrintList(pClonedHead);
}

//          -----------------
//         \|/              |
//  1-------2-------3-------4-------5
//  |       |      /|\             /|\
//  --------+--------               |
//          -------------------------
void Test1()
{
    ComplexListNode* pNode1 = CreateNode(1);
    ComplexListNode* pNode2 = CreateNode(2);
    ComplexListNode* pNode3 = CreateNode(3);
    ComplexListNode* pNode4 = CreateNode(4);
    ComplexListNode* pNode5 = CreateNode(5);

    BuildNodes(pNode1, pNode2, pNode3);
    BuildNodes(pNode2, pNode3, pNode5);
    BuildNodes(pNode3, pNode4, NULL);
    BuildNodes(pNode4, pNode5, pNode2);

    test("Test1", pNode1);
}

int main()
{
    Test1();
}
