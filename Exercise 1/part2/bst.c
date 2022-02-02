#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>
#include "bst.h"

Node* insertNode(Node *root, int value) {
    if (root == NULL) {
        //This is an empty tree, return a new node.
        Node* newNode = (Node*)malloc(sizeof(struct Node));
        if (newNode == NULL) {
            printf("ERROR: Memory allocation failed");
            return root;
        }
        newNode->data = value;
        newNode->left = NULL;
        newNode->right = NULL;
        return newNode;
    }
    if (root->data < value) {
        if (root->right == NULL) {
            Node* newNode = (Node*)malloc(sizeof(struct Node));
            if (newNode == NULL) {
                printf("ERROR: Memory allocation failed");
                return root;
            }
            newNode->data = value;
            newNode->left = NULL;
            newNode->right = NULL;
            root->right = newNode;
        } else {
            root->right = insertNode(root->right, value);
        }
    } else {
        if (root->left == NULL) {
            Node* newNode = (Node*)malloc(sizeof(struct Node));
            if (newNode == NULL) {
                printf("ERROR: Memory allocation failed");
                return root;
            }
            newNode->data = value;
            newNode->left = NULL;
            newNode->right = NULL;
            root->left = newNode;
        } else {
            root->left = insertNode(root->left, value);
        }
    }
    return root;
}

Node* findSmallest(Node* root) {
    Node* current = root;
    if (root == NULL) {
        return NULL;
    }
    while (current->left != NULL) {
        current = current->left;
    }
    return current;
}

Node* deleteNode(Node *root, int value) {
    Node* newTree;
    if (root->data == value) {
        if (root->left == NULL && root->right == NULL) {
            //This is a leaf node, free this node and return null;
            free(root);
            return NULL;
        } else if (root->left == NULL) {
            //This is the only child.
            newTree = root->right;
        } else if (root->right == NULL) {
            //This is the only child.
            newTree = root->left;
        } else {
            //Find the smallest successor in the right side of the tree to replace this.
            Node* smallestSuccessor = findSmallest(root->right);
            //Copy data.
            root->data = smallestSuccessor->data;
            //Replace the right side of this node with a subtree with the copied node removed (the left side will always remain the same in this operation as we found the smallest successor, as a result all of the left side of the tree will remain smaller than the node).
            root->right = deleteNode(root->right, root->data);
            return root;
        }
        //Free root as to remove this node from memory and stop memory leaks.
        free(root);

        //Return the only child tree.
        return newTree;
    } else if (root->data < value) {
        //Traverse the right side of the tree for the node.
        root->right = deleteNode(root->right, value);
    } else {
        //Traverse the left side of the tree for the node.
        root->left = deleteNode(root->left, value);
    }
    return root;
}

void printSubtree(Node *root) {
    if (root == NULL) {
        return;
    }
    if (root->left != NULL) {
        printSubtree(root->left);
    }
    printf("%d\n", root->data);
    if (root->right != NULL) {
        printSubtree(root->right);
    }
}

int countNodes(Node *root) {
    if (root == NULL) {
        return 0;
    }
    return countNodes(root->left) + 1 + countNodes(root->right);
}

Node* freeSubtree(Node *root) {
    if (root != NULL) {
        if (root->left != NULL) {
            freeSubtree(root->left);
        }
        if (root->right != NULL) {
            freeSubtree(root->left);
        }
        free(root);
    }
    return NULL;
}

int sumSubtree(Node *root) {
    if (root == NULL) {
        return 0;
    }
    return sumSubtree(root->left) + root->data + sumSubtree(root->right);
}