#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

struct ListNode
{
    int         m_nValue;
    ListNode *  m_pNext;
};

ListNode * CreateListNode(int value)
{
    ListNode * pNode = new ListNode();
    pNode->m_nValue = value;
    pNode->m_pNext = NULL;

    return pNode;
}

void ConnectListNodes(ListNode * pCurrent, ListNode * pNext)
{
    pCurrent->m_pNext = pNext;
}

void PrintListNode(ListNode * pNode)
{
    if (pNode == NULL)
        cout << "the node is null" << endl;
    else
        cout << "the value of this node is " << pNode->m_nValue << endl;
}

void PrintList(ListNode * pHead)
{
    cout << endl << "begin to print list" << endl;
    ListNode * pNode = pHead;
    while (pNode != NULL) {
        cout << pNode->m_nValue << "   " << endl;
        pNode = pNode->m_pNext;
    }

    cout << "end to print list" << endl;
}

void AddToTail(ListNode ** pHead, int value)
{
    ListNode * pNew = new ListNode();
    pNew->m_nValue = value;
    pNew->m_pNext = NULL;

    if (*pHead == NULL)
        *pHead = pNew;
    else {
        ListNode * pNode = *pHead;
        while (pNode->m_pNext != NULL)
            pNode = pNode->m_pNext;

        pNode->m_pNext = pNew;
    }
}

void RemoveNode(ListNode ** pHead, int value)
{
    if (pHead == NULL || *pHead == NULL)
        return ;

    ListNode * pToBeDeleted = NULL;
    if ((*pHead)->m_nValue == value) {
        pToBeDeleted = *pHead;
        *pHead = (*pHead)->m_pNext;
    }
    else {
        ListNode * pNode = *pHead;
        while (pNode->m_pNext != NULL && pNode->m_pNext->m_nValue != value)
            pNode = pNode->m_pNext;

        if (pNode->m_pNext != NULL && pNode->m_pNext->m_nValue == value) {
            pToBeDeleted = pNode->m_pNext;
            pNode->m_pNext = pNode->m_pNext->m_pNext;
        }

    }

    if (pToBeDeleted != NULL) {
        delete pToBeDeleted;
        pToBeDeleted = NULL;
    }
}

size_t GetListLength(ListNode * pHead)
{
    size_t nLen = 0;
    ListNode * pNode = pHead;
    while (pNode != NULL) {
        ++ nLen;
        pNode = pNode->m_pNext;
    }

    return nLen;
}

ListNode * FindFirstCommonNode(ListNode * pHead1, ListNode * pHead2)
{
    size_t len1 = GetListLength(pHead1);
    size_t len2 = GetListLength(pHead2);
    int lenDif = len1 - len2;

    ListNode * pHeadLong = pHead1;
    ListNode * pHeadShort = pHead2;
    if (len1 < len2) {
        pHeadLong = pHead2;
        pHeadShort = pHead1;
        lenDif = len2 - len1;
    }

    for (int i = 0; i < lenDif; ++i)
        pHeadLong = pHeadLong->m_pNext;

    while ((pHeadLong != NULL) && (pHeadShort != NULL) && (pHeadLong != pHeadShort)) {
        pHeadLong = pHeadLong->m_pNext;
        pHeadShort = pHeadShort->m_pNext;
    }

    ListNode * pFirstCommonNode = pHeadLong;

    return pFirstCommonNode;

}

int main()
{
// 第一个公共结点在链表中间
// 1 - 2 - 3 \
//            6 - 7
//     4 - 5 /
    ListNode* pNode1 = CreateListNode(1);
    ListNode* pNode2 = CreateListNode(2);
    ListNode* pNode3 = CreateListNode(3);
    ListNode* pNode4 = CreateListNode(4);
    ListNode* pNode5 = CreateListNode(5);
    ListNode* pNode6 = CreateListNode(6);
    ListNode* pNode7 = CreateListNode(7);


    ConnectListNodes(pNode1, pNode2);
    ConnectListNodes(pNode2, pNode3);
    ConnectListNodes(pNode3, pNode6);
    ConnectListNodes(pNode4, pNode5);
    ConnectListNodes(pNode5, pNode6);
    ConnectListNodes(pNode6, pNode7);

    ListNode* pResult = FindFirstCommonNode(pNode1, pNode4);
    PrintListNode(pResult);

    return 0;
}
