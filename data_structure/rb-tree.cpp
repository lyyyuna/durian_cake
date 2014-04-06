#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iterator>

using namespace std;

template <class KEY, class U>
class RB_Tree
{

private:
    RB_Tree(const RB_Tree& input) {}
    const RB_Tree& operator = (const RB_Tree& input) {}
    enum COLOR
    {
        RED,
        BLACK
    };
private:
    class RB_Node
    {
    public:
        RB_Node()
        {
            right = NULL;
            left = NULL;
            parent = NULL;
        }
        COLOR RB_COLOR;
        RB_Node* left;
        RB_Node* right;
        RB_Node* parent;
        KEY key;
        U data;
    };
private:
    RB_Node* m_nullNode;
    RB_Node* m_root;

public:
    RB_Tree()
    {
        m_nullNode = new RB_Node();
        m_root = m_nullNode;
        m_nullNode->right = m_root;
        m_nullNode->left = m_root;
        m_nullNode->parent = m_root;
        m_nullNode->RB_COLOR = BLACK;
    }
    bool isEmpty();
    RB_Node* searchNode(KEY key);
    bool insertNode(KEY key, U data);
    void insertFixUp(RB_Node* node);
    bool rotateLeft(RB_Node* node);
    bool rotateRight(RB_Node* node);
    bool deleteNode(KEY key);
    void deleteFixUp(RB_Node* node);
    RB_Node* treeSuccessor(RB_Node* node);
    void InOrderTraverse(RB_Node* node);
    void print();
    ~RB_Tree()
    {
        clear(m_root);
        delete m_nullNode;
    }

private:
    void clear(RB_Node* node)
    {
        if (node == m_nullNode)
            return ;
        else {
            clear(node->left);
            clear(node->right);
            delete node;
        }
    }
};

template <class KEY, class U>
bool RB_Tree<KEY, U>::isEmpty()
{
    if (this->m_root == this->m_nullNode)
        return true;
    else
        return false;
}

template <typename KEY, typename U>
typename RB_Tree<KEY, U>::RB_Node *
RB_Tree<KEY, U>::searchNode(KEY key)
{
    RB_Node* index = m_root;
    while (index != m_nullNode) {
        if (key < index->key)
            index = index->left;
        else if (key > index->key)
            index = index->right;
        else
            break;
    }

    return index;
}

//--------------------------插入结点总操作----------------------------------  
            //全部的工作，都在下述伪代码中：  
            /*RB-INSERT(T, z)    
            1  y ← nil[T]                 // y 始终指向 x 的父结点。 
            2  x ← root[T]              // x 指向当前树的根结点， 
            3  while x ≠ nil[T] 
            4      do y ← x 
            5         if key[z] < key[x]           //向左，向右.. 
            6            then x ← left[x] 
            7            else x ← right[x]   //为了找到合适的插入点，x探路跟踪路径，直到x成为NIL 为止。 
            8  p[z] ← y         //y置为 插入结点z 的父结点。 
            9  if y = nil[T] 
            10     then root[T] ← z 
            11     else if key[z] < key[y] 
            12             then left[y] ← z 
            13             else right[y] ← z     //此 8-13行，置z 相关的指针。 
            14  left[z] ← nil[T] 
            15  right[z] ← nil[T]            //设为空， 
            16  color[z] ← RED             //将新插入的结点z作为红色 
            17  RB-INSERT-FIXUP(T, z)    
            */  
            //因为将z着为红色，可能会违反某一红黑性质，  
            //所以需要调用下面的RB-INSERT-FIXUP(T, z)来保持红黑性质。  
template <class KEY, class U>
bool
RB_Tree<KEY, U>::insertNode(KEY key, U data)
{
    RB_Node* Y = m_nullNode;
    RB_Node* X = m_root;
    while (X != m_nullNode) {
        Y = X;
        if (X->key < key)
            X = X->right;
        else if (X->key > key)
            X = X->left;
        else
            return false;
    }

    RB_Node* Z = new RB_Node();
    Z->key = key;
    Z->data = data;
    Z->RB_COLOR = RED;
    Z->left = m_nullNode;
    Z->right = m_nullNode;

    // Z->parent = Y;

    if (Y == m_nullNode)
    {
        m_root = Z;
        m_root->parent = m_nullNode;
        m_nullNode->left = m_root;
        m_nullNode->right = m_root;
        m_nullNode->parent = m_root;
    } else {
        if (key < Y->key)
            Y->left = Z;
        else
            Y->right = Z;

        Z->parent = Y;
    }

    insertFixUp(Z);
}

            //---------------------插入结点性质修复--------------------------------  
            //3种插入情况，都在下面的伪代码中(未涉及到所有全部的插入情况)。  
            /* 
            RB-INSERT-FIXUP(T, z) 
            1 while color[p[z]] = RED 
            2     do if p[z] = left[p[p[z]]] 
            3           then y ← right[p[p[z]]] 
            4                if color[y] = RED 
            5                   then color[p[z]] ← BLACK                    ? Case 1 
            6                        color[y] ← BLACK                       ? Case 1 
            7                        color[p[p[z]]] ← RED                   ? Case 1 
            8                        z ← p[p[z]]                            ? Case 1 
            9                   else if z = right[p[z]] 
            10                           then z ← p[z]                       ? Case 2 
            11                                LEFT-ROTATE(T, z)              ? Case 2 
            12                           color[p[z]] ← BLACK                 ? Case 3 
            13                           color[p[p[z]]] ← RED                ? Case 3 
            14                           RIGHT-ROTATE(T, p[p[z]])            ? Case 3 
            15           else (same as then clause with "right" and "left" exchanged) 
            16 color[root[T]] ← BLACK 
            */  
            //然后的工作，就非常简单了，即把上述伪代码改写为下述的c++代码：  
template <class KEY, class U>
void
RB_Tree<KEY, U>::insertFixUp(RB_Node* node)
{
    while (node->parent->RB_COLOR == RED) {
        if (node->parent == node->parent->parent->left) {
            RB_Node* uncle = node->parent->parent->right;
            // case 1
            if (uncle->RB_COLOR == RED) {
                node->parent->RB_COLOR = BLACK;
                uncle->RB_COLOR = BLACK;
                node->parent->parent->RB_COLOR = RED;
                node = node->parent->parent;
            }
            else if (uncle->RB_COLOR == BLACK) {
                // case 2
                if (node == node->parent->right) {
                    node = node->parent;
                    rotateLeft(node);
                } 
                // case 3
                else {
                    node->parent->RB_COLOR = BLACK;
                    node->parent->parent->RB_COLOR = RED;
                    rotateRight(node->parent->parent);
                }
            }
        }
        else {
            RB_Node* uncle = node->parent->parent->left;
            if (uncle->RB_COLOR == RED) {
                node->parent->RB_COLOR = BLACK;
                uncle->RB_COLOR = BLACK;
                uncle->parent->RB_COLOR = RED;
                node = node->parent->parent;
            } else if (uncle->RB_COLOR == BLACK) {
                if (node == node->parent->left) {
                    node = node->parent;
                    rotateRight(node);
                } else {
                    node->parent->RB_COLOR = BLACK;
                    node->parent->parent->RB_COLOR = RED;
                    rotateLeft(node->parent->parent);
                }
            }
        }
    }

    m_root->RB_COLOR = BLACK;
}

template <class KEY, class U>
bool
RB_Tree<KEY, U>::deleteNode(KEY key)
{
    RB_Node* Z = searchNode(key);
    if (Z == m_nullNode)
        return false;
    if (Z->left != m_nullNode && Z->right != m_nullNode) {
        RB_Node* successor = treeSuccessor(Z);
        Z->data = successor->data;
        Z->key = successor->key;
        Z = successor;
    }

    RB_Node* X;
    if (Z->left != m_nullNode)
        X = Z->left;
    else if (Z->right != m_nullNode)
        X = Z->right;
    else
        X = m_nullNode;

    X->parent = Z->parent;

    if (Z->parent == m_nullNode) {
        m_root = X;
        m_nullNode->parent = m_root;
        m_nullNode->left = m_root;
        m_nullNode->right = m_root;
    } else if (Z == Z->parent->left)
        Z->parent->left = X;
    else
        Z->parent->right = X;

    if (Z->RB_COLOR == BLACK)
        if (!(X == m_nullNode && X->parent == m_nullNode))
            deleteFixUp(X);

    return true;
}

            //---------------------删除结点性质修复-----------------------------------  
            //所有的工作，都在下述23行伪代码中：  
            /* 
            RB-DELETE-FIXUP(T, x) 
            1 while x ≠ root[T] and color[x] = BLACK 
            2     do if x = left[p[x]] 
            3           then w ← right[p[x]] 
            4                if color[w] = RED 
            5                   then color[w] ← BLACK                        ?  Case 1 
            6                        color[p[x]] ← RED                       ?  Case 1 
            7                        LEFT-ROTATE(T, p[x])                    ?  Case 1 
            8                        w ← right[p[x]]                         ?  Case 1 
            9                if color[left[w]] = BLACK and color[right[w]] = BLACK 
            10                   then color[w] ← RED                          ?  Case 2 
            11                        x p[x]                                  ?  Case 2 
            12                   else if color[right[w]] = BLACK 
            13                           then color[left[w]] ← BLACK          ?  Case 3 
            14                                color[w] ← RED                  ?  Case 3 
            15                                RIGHT-ROTATE(T, w)              ?  Case 3 
            16                                w ← right[p[x]]                 ?  Case 3 
            17                         color[w] ← color[p[x]]                 ?  Case 4 
            18                         color[p[x]] ← BLACK                    ?  Case 4 
            19                         color[right[w]] ← BLACK                ?  Case 4 
            20                         LEFT-ROTATE(T, p[x])                   ?  Case 4 
            21                         x ← root[T]                            ?  Case 4 
            22        else (same as then clause with "right" and "left" exchanged) 
            23 color[x] ← BLACK  
            */  
            //接下来的工作，很简单，即把上述伪代码改写成c++代码即可。  
template <class KEY, class U>
void
RB_Tree<KEY, U>::deleteFixUp(RB_Node* node)
{
    while (node != m_root && node->RB_COLOR == BLACK) {
        if (node == node->parent->left) {
            RB_Node* brother = node->parent->right;
            // case 1
            if (brother->RB_COLOR == RED) {
                brother->RB_COLOR = BLACK;
                node->parent->RB_COLOR = RED;
                rotateLeft(node->parent);
            }
            // case 2
            else {
                if (brother->left->RB_COLOR == BLACK && brother->right->RB_COLOR == BLACK) {
                    brother->RB_COLOR = RED;
                    node = node->parent;
                }
                // case 3
                else if (brother->right->RB_COLOR = BLACK) {
                    brother->RB_COLOR = RED;
                    brother->right->RB_COLOR = BLACK;
                    rotateRight(brother);
                }
                // case 4
                else if (brother->right->RB_COLOR == RED) {
                    brother->RB_COLOR = node->parent->RB_COLOR;
                    node->parent->RB_COLOR = BLACK;
                    brother->right->RB_COLOR = BLACK;
                    rotateLeft(node->parent);
                    node = m_root;
                }
            }
        }
        else {
            RB_Node* brother = node->parent->left;
            if (brother->RB_COLOR == RED) {
                brother->RB_COLOR = BLACK;
                node->parent->RB_COLOR = RED;
                rotateRight(node->parent);
            } else {
                if (brother->left->RB_COLOR == BLACK && brother->right->RB_COLOR == BLACK) {
                    brother->RB_COLOR = RED;
                    node = node->parent;
                } 
                else if ( brother->left->RB_COLOR == BLACK ) {
                    brother->RB_COLOR = RED;
                    brother->right->RB_COLOR = BLACK;
                    rotateLeft(brother);
                }
                else if ( brother->right->RB_COLOR == BLACK ) {
                    brother->RB_COLOR = node->parent->RB_COLOR;
                    node->parent->RB_COLOR = BLACK;
                    brother->left->RB_COLOR = BLACK;
                    rotateRight(node->parent);
                    node = m_root;
                }

            }
        }
    }

    m_nullNode->parent = m_root;
    node->RB_COLOR = BLACK;
}

// rotate left
template <class KEY, class U>
bool
RB_Tree<KEY, U>::rotateLeft(RB_Node* node)
{
    if (node == m_nullNode || node->right == m_nullNode)
        return false;

    RB_Node* node_r = node->right;
    node_r->parent = node->parent;
    node->right = node_r->left;
    if (node_r->left != m_nullNode)
        node_r->parent = node;

    if (node->parent == m_nullNode) {
        m_root = node_r;
        m_nullNode->left = m_root;
        m_nullNode->right = m_root;
    } else {
        if (node->parent->left == node)
            node->parent->left = node_r;
        else 
            node->parent->right = node_r;
    }
    node->parent = node_r;
    node_r->left = node;
}

// rotate right
template <class KEY, class U>
bool
RB_Tree<KEY, U>::rotateRight(RB_Node* node)
{
    if (node == m_nullNode || node->left == m_nullNode)
        return false;

    RB_Node* node_l = node->left;
    node_l->parent = node->parent;
    node->left = node_l->right;
    if (node_l->right != m_nullNode)
        node_l->parent = node;

    if (node->parent == m_nullNode) {
        m_root = node_l;
        m_nullNode->left = m_root;
        m_nullNode->right = m_root;
    } else {
        if (node->parent->left == node)
            node->parent->left = node_l;
        else
            node->parent->right = node_l;
    }
    node->parent = node_l;
    node_l->right = node;

}

template <class KEY, class U>
typename RB_Tree<KEY, U>::RB_Node*
RB_Tree<KEY, U>::treeSuccessor(RB_Node* node)
{
    if (node == m_nullNode)
        return m_nullNode;

    RB_Node* result = node->right;
    while (result != m_nullNode) {
        if (result->left != m_nullNode)
            result = result->left;
        else
            break;
    }
    if (result == m_nullNode) {
        RB_Node* index = node->parent;
        result = node;
        while (index != m_nullNode && result == index->right) {
            result = index;
            index = index->parent;
        }
        return result;
    }

}

template <class KEY, class U>
void
RB_Tree<KEY, U>::InOrderTraverse(RB_Node* node)
{
    if(node==m_nullNode)  
    {  
        return;  
    }  
    else  
    {  
        InOrderTraverse(node->left);  
        cout<<node->key << " ";  
        InOrderTraverse(node->right);  
    }  
}

template <class KEY, class U>
void
RB_Tree<KEY, U>::print()
{
    InOrderTraverse(m_root);
    cout << endl;
}

int main()
{
    RB_Tree<int, int> tree;

    vector<int> v;
    for (int i = 0; i < 20; ++i)
        v.push_back(i);

    random_shuffle(v.begin(), v.end());
    copy(v.begin(), v.end(), ostream_iterator<int>(cout, " "));
    cout << endl;
    for(size_t i = 0; i < v.size(); ++i) {
        tree.insertNode(v[i], i);
    }
    tree.print();
    cout << endl;

    cout << "delete " << v[5] << endl;
    tree.deleteNode(v[5]);
    tree.print();
    cout << endl;
    return 0;
}