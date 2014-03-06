#include <iostream>
#include <string>

using namespace::std;


class CMyString
{
public:
    CMyString(char *pData = NULL);
    CMyString(const CMyString &str);
    ~CMyString();

    CMyString & operator = (const CMyString &str);

    void print();

private:
    char *m_pData;
};

CMyString::CMyString(char *pData)
{
    if (pData == NULL) {
        m_pData = new char[1];
        m_pData[0] = '\0';
    } else {
        int length = strlen(pData);
        m_pData = new char[length + 1]; // for the last '\0'
        strcpy(m_pData, pData);
    }
}

CMyString::CMyString(const CMyString &str)
{
    int length = strlen(str.m_pData);
    m_pData = new char[length+1];
    strcpy(m_pData, str.m_pData);
}

CMyString::~CMyString()
{
    delete[] m_pData;
}
/*
CMyString & CMyString::operator = (const CMyString &str)
{
    if (this == &str)
        return *this;
        
    delete[] m_pData;
    m_pData = NULL;
    m_pData = new char[strlen(str.m_pData) + 1];
    strcpy(m_pData, str.m_pData);

    return *this;
}*/

CMyString & CMyString::operator = (const CMyString &str)
{
    if (this != &str) {
        CMyString strSwap(str);

        char *pTemp = strSwap.m_pData;
        strSwap.m_pData = m_pData;
        m_pData = pTemp;
    }

    return *this;
}

void CMyString::print()
{
    cout<<m_pData;
}

void Test1()
{
    cout<<"test 1 begins"<<endl;

    char *text = "hello, world";

    CMyString str1(text);
    CMyString str2;
    str2 = str1;

    cout << "the expected result is " << text << endl;

    cout << "the actual result is ";
    str2.print();
    cout << endl;

}

int main(void)
{
    cout << "hello, world." << endl;

    Test1();

    return 0;
}
