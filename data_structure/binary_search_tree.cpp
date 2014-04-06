#include <iostream>

using namespace std;

struct Node
{
    int key;
    struct Node* left;
    struct Node* right;
    struct Node* parent;
};

void
insert(Node** root, int key)
{
    // init the node
    Node* p = new Node;
    p->key = key;
    p->left = p->right = p->parent = NULL;

    // the tree is null
    if (*root == NULL) {
        *root = p;
        return ;
    }
    
    // insert
    if ((*root)->left == NULL && (*root)->key > key) {
        p->parent = *root;
        (*root)->left = p;
        return ;
    }

    // insert right
    if ((*root)->right == NULL && (*root)->key < key) {
        p->parent = *root;
        (*root)->right = p;
        return ;
    }

    // else
    if ((*root)->key > key)
        insert(&(*root)->left, key);
    else
        insert(&(*root)->right, key);

    return;
}

Node*
search(Node* root, int key)
{
    if (root == NULL)
        return NULL;
    if (key > root->key)
        return search(root->right, key);
    else if (key < root->key)
        return search(root->left, key);
    else 
        return root;
}

Node* 
searchMin(Node* root)
{
    if (root == NULL)
        return NULL;
    if (root->left == NULL)
        return root;
    return searchMin(root->left);
}

Node*
searchMax(Node* root)
{
    if (root == NULL)
        return NULL;
    if (root->right == NULL)
        return root;
    return searchMax(root->right);
}

Node*
treeSuccessor(Node* root)
{
    if (root->right != NULL)
        return searchMin(root->right);

    Node* y = root->parent;
    while (y != NULL && y->right==root) {
        root = y;
        y = root->parent;
    }

    return y;
}

void
deleteNode(Node** root, int key)
{
    // three cases

    Node* nodeToBeDeleted = NULL;
    nodeToBeDeleted = search(*root, key);
    Node* nodeY;
    if (nodeToBeDeleted->left == NULL || nodeToBeDeleted->right == NULL) {
        nodeY = nodeToBeDeleted;
    } else {
        nodeY = treeSuccessor(nodeToBeDeleted);
    }

    Node* nodeX;
    if (nodeY->left != NULL)
        nodeX = nodeY->left;
    else
        nodeX = nodeY->right;

    // case2
    if (nodeX != NULL)
        nodeX->parent = nodeY->parent;

    if (nodeY->parent == NULL)
        *root = nodeX;
    else if (nodeY == nodeY->parent->left)
        nodeY->parent->left = nodeX;
    else
        nodeY->parent->right = nodeX;

    if (nodeY != nodeToBeDeleted) {
        nodeToBeDeleted->key = nodeY->key;
    }
}

void
create(Node** root, int* arr, int len)
{
    for (int i = 0; i < len; ++i)
        insert(root, arr[i]);
}


int 
main()
{
    Node* root = NULL;
    int arr[11] = {15,6,18,3,7,17,20,2,4,13,9};
    create(&root, arr, 11);
    cout << root->key << endl;
    for(int i=0;i<2;i++)  
        deleteNode(&root,arr[i]);  
    //printf("%d\n",searchPredecessor(root)->key);  
    printf("%d\n",root->key);  
    printf("%d\n",searchMin(root)->key);  
    printf("%d\n",searchMax(root)->key);  
    printf("%d\n",search(root,13)->key);  
    return 0;
}