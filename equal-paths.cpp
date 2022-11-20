#include "equal-paths.h"
#include <cmath>
#include <iostream>
using namespace std;

// You may add any prototypes of helper functions here
int calculateHeight(Node* root){
    if(root==nullptr){
        return 0;
    }
    int height = 0;
    if(root->left!=nullptr){
        height++;
        if(root->right != nullptr){
            height += calculateHeight(root->left) + calculateHeight(root->right);
        }
        else{
            height += calculateHeight(root->left);
        }
    }
    else if(root->right!=nullptr){
        height++;
        if(root->left != nullptr){
            height += calculateHeight(root->left) + calculateHeight(root->right);
        }
        else{
            height += calculateHeight(root->right);
        }
    }
    return height;
}

bool equalPaths(Node* root)
{
    if(root == nullptr){ // base case
        return true;
    } 
    else if(root->right == nullptr && root->left != nullptr){
        return equalPaths(root->left);
    }
    else if(root->left == nullptr && root->right != nullptr){
        return equalPaths(root->right);
    }
    else{
        int right = calculateHeight(root->right);
        int left = calculateHeight(root->left);
        return left == right;
    }

}
