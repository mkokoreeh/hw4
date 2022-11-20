#ifndef RBBST_H
#define RBBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError
{
};

/**
 * A special kind of node for an AVL tree, which adds the balance as a data member, plus
 * other additional helper functions. You do NOT need to implement any functionality or
 * add additional data members or helper functions.
 */
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key &key, const Value &value, AVLNode<Key, Value> *parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance() const;
    void setBalance(int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value> *getParent() const override;
    virtual AVLNode<Key, Value> *getLeft() const override;
    virtual AVLNode<Key, Value> *getRight() const override;

protected:
    int8_t balance_; // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
 * An explicit constructor to initialize the elements by calling the base class constructor and setting
 * the color to red since every new node will be red when it is first inserted.
 */
template <class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key &key, const Value &value, AVLNode<Key, Value> *parent) : Node<Key, Value>(key, value, parent), balance_(0)
{
}

/**
 * A destructor which does nothing.
 */
template <class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{
}

/**
 * A getter for the balance of a AVLNode.
 */
template <class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
 * A setter for the balance of a AVLNode.
 */
template <class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
 * Adds diff to the balance of a AVLNode.
 */
template <class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
 * An overridden function for getting the parent since a static_cast is necessary to make sure
 * that our node is a AVLNode.
 */
template <class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value> *>(this->parent_);
}

/**
 * Overridden for the same reasons as above.
 */
template <class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value> *>(this->left_);
}

/**
 * Overridden for the same reasons as above.
 */
template <class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value> *>(this->right_);
}

/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/

template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert(const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key &key);                              // TODO
protected:
    virtual void nodeSwap(AVLNode<Key, Value> *n1, AVLNode<Key, Value> *n2);

    // Add helper functions here
    void removeFix(AVLNode<Key, Value> *p, int8_t diff);
    void insertFix(AVLNode<Key, Value> *p, AVLNode<Key, Value> *n);
    void rotate(AVLNode<Key, Value> *p, char direction);
};

/*
 * Recall: If key is already in the tree, you should
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    if(this->root_ == nullptr)
    {
        AVLNode<Key, Value> *temp = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        this->root_ = temp;
        temp->setBalance(0);
    }
    // find node
    AVLNode<Key, Value> *temp = static_cast<AVLNode<Key, Value> *>(this->root_);
    
    if (temp == nullptr){
        return;
    }
    else{//there exists a tree,insert like in bst
        while(temp != nullptr){ // loop through tree
            if(new_item.first > temp->getKey()){ // if inserted node is greater than the current node, treat it as a right node 
                if (temp->getRight() == nullptr){
                    AVLNode<Key, Value> *right = new AVLNode<Key, Value>(new_item.first, new_item.second, temp);
                    temp->setRight(right);
                    right->setBalance(0); //update balance
                    if(abs(temp->getBalance()) == 1){
                        temp->setBalance(0);
                    }
                    else if(temp->getBalance() == 0){//fix tree if balance was 0
                        temp->updateBalance(1);
                        insertFix(temp, right);
                    }
                    return;
                }
                else{
                    temp = temp->getRight();
                }
            }
            else if(new_item.first < temp->getKey()){
                if (temp->getLeft() != NULL){
                    temp = temp->getLeft();
                }
                // iterate to next left 
                else{   
                    AVLNode<Key, Value> *left = new AVLNode<Key, Value>(new_item.first, new_item.second, temp);
                    temp->setLeft(left);
                    left->setBalance(0);
                    if(abs(temp->getBalance()) == 1){
                        temp->setBalance(0);
                    }
                    else if(temp->getBalance() == 0){
                        temp->updateBalance(-1);
                        insertFix(temp, left);
                    }
                    return;
                }
            }
            else{
                temp->setValue(new_item.second);
                break;
            }
        }

    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    // TODO
    if (this->root_ == nullptr) {
        return; 
    }
    AVLNode<Key,Value>* temp;
    AVLNode<Key, Value> *trash = static_cast<AVLNode<Key, Value> *>(this->internalFind(key)); 

    // if c is null then we return 
    if (trash == nullptr) {
        return; 
    }

    int8_t diff = 0;
    // 2 children 1st since it yields to a 1/0 child case
    if(trash->getLeft() != nullptr && trash->getRight() != nullptr) {
        nodeSwap(trash, static_cast<AVLNode<Key, Value> *>(this->predecessor(trash)));
    }
    AVLNode <Key, Value> * parent = trash->getParent();
    if(parent != nullptr){
        if(parent->getLeft() == trash){
            diff = 1;
        }
        else if(parent->getRight() == trash){
            diff = -1;
        }
    }
    // 0 children case
    if(trash->getRight() == nullptr && trash->getLeft() == nullptr) {
        temp = nullptr;
        if (parent == nullptr) {
            this->root_ = temp; 
        } 
        else{
            if(trash->getParent()->getLeft() == trash) {
                parent->setLeft(temp); 
            } 
            else if(trash->getParent()->getRight() == trash) {
                parent->setRight(temp); 
            } 
        }
        // deleting trash 
        delete trash;
        removeFix(parent, diff); 
        return;
    }

    // one child case
    if (trash->getRight()==NULL && trash->getLeft()!=NULL) {
        temp = trash->getLeft(); 
        if (parent == NULL) {
        this->root_ = temp; 
        } 
        else{
            if(trash->getParent()->getLeft() == trash) {
                parent->setLeft(temp); 
            } 
            else if (trash->getParent()->getRight() == trash) {
                parent->setRight(temp); 
            } 
        }
        // check to set the parent of one to two 
        if (temp != nullptr) {
            temp->setParent(parent); 
        }
        delete trash;
        removeFix(parent, diff); 
        return;
        } 
    else if(trash->getRight()!=NULL && trash->getLeft()==NULL){
            temp = trash->getRight();
            AVLNode <Key, Value> * parent = trash->getParent(); 
        if (parent == nullptr) {
            this->root_ = temp; 
        } 
        else {
            if (trash->getParent()->getLeft() == trash) {
                parent->setLeft(temp); 
            } 
            else if (trash->getParent()->getRight() == trash) {
                parent->setRight(temp); 
            } 
        }

        // check to set the parent of one to two 
        if (temp != nullptr) {
            temp->setParent(parent); 
        }
        // deleting
        delete trash;
        removeFix(parent, diff);
        return;
    } 
}

template <class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value> *n, int8_t diff)
{
    //USE SLIDES
/*
		If n is null, return
• Compute next recursive call's arguments now before altering the tree
– Let p = parent(n) and if p is not NULL let ndiff (nextdiff) = +1 if n is a left child and -1 otherwise
• Assume diff = -1 and follow the remainder of this approach, mirroring if diff = +1
• Case 1: b(n) + diff == -2
– [Perform the check for the mirror case where b(n) + diff == +2, flipping left/right and -1/+1]
– Let c = left(n), the taller of the children
– Case 1a: b(c) == -1 // zig-zig case
• rotateRight(n), b(n) = b(c) = 0, removeFix(p, ndiff)
– Case 1b: b(c) == 0 // zig-zig case
• rotateRight(n), b(n) = -1, b(c) = +1 // Done!
– Case 1c: b(c) == +1 // zig-zag case
• Let g = right(c)
• rotateLeft(c) then rotateRight(n)
• If b(g) was +1 then b(n) = 0, b(c) = -1, b(g) = 0
• If b(g) was 0 then b(n) = 0, b(c) = 0, b(g) = 0
• If b(g) was -1 then b(n) = +1, b(c) = 0, b(g) = 0
• removeFix(p, ndiff);
• Case 2: b(n) + diff == -1: then b(n) = -1; // Done!
• Case 3: b(n) + diff == 0: then b(n) = 0, removeFix(p, ndiff)
*/
    if (n == nullptr){
        return;
    }
    AVLNode<Key, Value> *p = n->getParent();
    int8_t ndiff = 0;
    if (p != nullptr){
        if (p->getLeft() == n){
            ndiff = 1;
        }
        else{
            ndiff = -1;
        }
    }
    // Assuming diff = -1

    if (diff == -1){
        if (n->getBalance() + diff == -2){
            AVLNode<Key, Value> *c = n->getLeft();
            if (c->getBalance() == -1){
                rotate(n,'r');
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff);
            }
            else if (c->getBalance() == 0){
                rotate(n,'r');
                n->setBalance(-1);
                c->setBalance(1);
            }
            else if (c->getBalance() == 1){
                AVLNode<Key, Value> *g = c->getRight();
                rotate(c,'l');
                rotate(n,'r');
                if (g->getBalance() == 1){
                    n->setBalance(0);
                    c->setBalance(-1);
                    g->setBalance(0);
                }
                else if (g->getBalance() == 0){
                    n->setBalance(0);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                else if (g->getBalance() == -1){
                    n->setBalance(1);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                removeFix(p, ndiff);
            }
        }
        else if (n->getBalance() + diff == -1){
            n->setBalance(-1);
        }
        else if (n->getBalance() + diff == 0){
            n->setBalance(0);
            removeFix(p, ndiff);
        }
    }

    // mirror cases
    else if (diff == 1){
        if (n->getBalance() + diff == 2){
            AVLNode<Key, Value> *c = n->getRight();
            if (c->getBalance() == 1){
                rotate(n,'l');
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff);
            }
            else if (c->getBalance() == 0){
                rotate(n,'l');
                n->setBalance(1);
                c->setBalance(-1);
            }
            else if (c->getBalance() == -1){
                AVLNode<Key, Value> *g = c->getLeft();
                rotate(c, 'r');
                rotate(n, 'l');
                if (g->getBalance() == -1){
                    n->setBalance(0);
                    c->setBalance(1);
                    g->setBalance(0);
                }
                else if (g->getBalance() == 0){
                    n->setBalance(0);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                else if (g->getBalance() == 1){
                    n->setBalance(-1);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                removeFix(p, ndiff);
            }
        }
        else if (n->getBalance() + diff == 1){
            n->setBalance(1);
        }
        else if (n->getBalance() + diff == 0){
            n->setBalance(0);
            removeFix(p, ndiff);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key,Value>::rotate(AVLNode<Key, Value> *node, char direction){
    if(direction == 'r'){ //right rotate
        if(node == nullptr){// base case, if no node stop
            return;
        }
        AVLNode<Key, Value> *parent = node->getParent();
        AVLNode<Key, Value> *left = node->getLeft();
        if (left == nullptr){ //if no node for rotation
            return;
        }
        else if(left != nullptr){
            node->setLeft(left->getRight());
        }
        if(parent == nullptr){ // if no parent, child becomes parent
            this->root_ = left;
        }
        else if (parent->getLeft() == node){ // if the node is a left
            parent->setLeft(left); // set the child
        }
        else if (parent->getRight() == node){
            parent->setRight(left);
        }
        left->setParent(parent);//set the parent of the new child node
        if (left->getRight() != nullptr){
					//readjust children of new node
            left->getRight()->setParent(node);
        }
        node->setParent(left);
        left->setRight(node);
        return;
    }
    if(direction == 'l'){ // left rotate, same as right rotate but opposite
        if (node == nullptr){
            return;
        }
        AVLNode<Key, Value> *right = node->getRight();
        AVLNode<Key, Value> *parent = node->getParent();
        if (right == nullptr){
            return;
        } 
        node->setRight(right->getLeft());
        if (parent == nullptr){
            this->root_ = right;
        }
        else if (parent->getLeft() == node){
            parent->setLeft(right);
        }
        else if (parent->getRight() == node){
            parent->setRight(right);
        }
        
        right->setParent(parent);
        if (right->getLeft() != nullptr){
            right->getLeft()->setParent(node);
        }
        node->setParent(right);
        right->setLeft(node);
        return;
    }
}
template <class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value> *p, AVLNode<Key, Value> *n)
{
    //initial condition
    if((abs(p->getBalance()) <= 1)&& (abs(n->getBalance()) <= 1)){
        if ((p == nullptr) || (p->getParent() == nullptr)){
            return;
        }
				//let g = parent
        AVLNode<Key, Value> *g = p->getParent();
        // left child check
        if(g->getLeft() == p){ 
            g->updateBalance(-1);
            if (g->getBalance() == 0){ // case 1, b(g) is 0
                return;
            }
            else if (g->getBalance() == -1){ // case 2, fix tree
                insertFix(g, p);
            }
            else if (g->getBalance() == -2){//case 3 zig zag
                if (g->getLeft() == p && p->getLeft() == n){ //rotate right case
                    rotate(g,'r');
                    g->setBalance(0);
                    p->setBalance(0);
                }
                else if (g->getLeft() == p && p->getRight() == n){//rotate left (p) and rotate right (g)
                    rotate(p,'l');
                    rotate(g,'r');
                    if (n->getBalance() == -1){
                        p->setBalance(0);
                        g->setBalance(1);
                        n->setBalance(0);
                    }
                    else if (n->getBalance() == 0){
                        p->setBalance(0);
                        g->setBalance(0);
                        n->setBalance(0);
                    }
                    else if (n->getBalance() == 1){
                        p->setBalance(-1);
                        g->setBalance(0);
                        n->setBalance(0);
                    }
                }
            }
        }
        // check for right, same but inverse
        else if (g->getRight() == p){
            g->updateBalance(1);
            if (g->getBalance() == 0){
                return;
            }
            else if (g->getBalance() == 1){
                insertFix(g, p);
            }
            else if (g->getBalance() == 2){
                if (g->getRight() == p && p->getRight() == n){
                    rotate(g,'l');
                    g->setBalance(0);
                    p->setBalance(0);
                }
                if (g->getRight() == p && p->getLeft() == n){
                    rotate(p,'r');
                    rotate(g,'l');
                    if (n->getBalance() == 1){
                        p->setBalance(0);
                        g->setBalance(-1);
                        n->setBalance(0);
                    }
                    else if (n->getBalance() == 0){
                        p->setBalance(0);
                        g->setBalance(0);
                        n->setBalance(0);
                    }
                    else if (n->getBalance() == -1){
                        p->setBalance(1);
                        g->setBalance(0);
                        n->setBalance(0);
                    }
                }
            }
        }
    }
}
template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}
#endif