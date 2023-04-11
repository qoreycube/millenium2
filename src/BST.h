/*
 * Millenium2 Chat Server Software for Unix Operating Systems
 * Copyright (c) 2002  Cory Kratz
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */



/* File BST contains the declarations and definitions of
 * BST, a class template for binary search trees.
 *
 * Written by:   Larry R. Nyhoff
 * Written for:  Lab Manual for C++: An Introduction to Data Structures
 *
 * Add descriptions of the basic operations and other documentation required 
 * by your instructor such as your name, course number, and the current date.
 *************************************************************************/
#include <iostream>
using namespace std;

#ifndef BINARY_SEARCH_TREE
#define BINARY_SEARCH_TREE

template <typename DataType>
class BST
{
 private:
	int myLeaves;
	bool myModified;
/*** Node structure ***/
  class BinNode
  {
   public:
    DataType data;
    BinNode * left,
            * right;

    // BinNode constructors
    // Default -- data part undefined; both links null
    BinNode()
    { left = right = NULL; }

    // Explicit Value -- data part contains item; both links null
    BinNode(DataType item)
    {
      data = item;
      left = right = NULL;
    }
  };

  pthread_mutex_t myMutex; // added
typedef BinNode * BinNodePointer;
  
  void Lock(void) {
	pthread_mutex_lock(&myMutex);
  }
  void Unlock(void) { pthread_mutex_unlock(&myMutex); }
 
 public:
/* numLeaves() - Returns number of leaves in current tree (int).
 * ***************************************************/
int numLeaves(void) { return myLeaves; }

/* Construct an empty BST.
 * Precondition: A BST has been declared
 * Postcondition: An empty BST has been created
 *******************************************************/
BST();


/*  Destruct the BST.
 *  Precondition: A BST has been declared
 *  Postcondition: An empy BST has been created
 * **********************************************/ 
~BST();

/*Copy constructor */
BST(const BST & original);


// copy tree method
void CopyTree(const BinNodePointer original, BinNodePointer & root);


/* Destroy nodes during postorder trasversal
 *  Precondition: A BST has been created
 *************************************************/  
 void Destroy(BinNodePointer root);

 


BST &operator = (const BST<DataType> original);




 /* Check if BST is empty.
 * Receive: BST containing this function (implicitly)
 * Return: true if BST is empty, false otherwise
 *******************************************************/
bool Empty() const;



/* Search the BST for item.
 *
 * Receive: The BST containing this function (implicitly)
 * item being searched for
 * Return: true if item found, false otherwise
* 
 ***********************************************************/
bool Search(DataType & item);

/* Insert item into BST.
 *
 * Receive: The BST containing this function (implicitly)
 * item being inserted
 * Postcondition: BST has been modified with item inserted
 * at proper position to maintain BST property.
 *************************************************************/
void Insert(const DataType & item);


/* Inorder traversal.
 *
 * Receive: The BST containing this function (implicitly)
 * ostream out
 * Output: Contents of the BST in inorder (via InorderAux)
 *************************************************************/
 void Inorder(void (myFunc)(DataType *, int &, void *), void *obj);


 void PostOrder(void (myFunc)(DataType *,int &, void *), void *obj);


 void PreOrder(void (myFunc)(DataType *, int &, void *), void *obj);


/* Inorder traversal auxiliary function
 *
 * Receive: The BST containing this function (implicitly)
 * ostream out, and pointer ptr to a binary tree node
 * Output: Contents of the BST with root at ptr in inorder
 *************************************************************/
 void InorderAux(void (myFunc)(DataType *, int &, void *), BinNodePointer ptr, int &currItem, void *obj);

 void PostOrderAux(void (myFunc)(DataType *, int &, void *), BinNodePointer ptr, int &currItem, void *obj);

 void PreOrderAux(void (myFunc)(DataType *, int &, void *), BinNodePointer ptr, int &currItem, void *obj);

 
/* Delete item from BST.
 *
 * Receive: The BST containing this function (implicitly)
 * item being deleted
 * Postcondition: BST has been modified with item removed (if
 * present); BST property is maintained.
 * Note: Delete uses auxiliary function Search2() to locate
 * the node containing item and its parent.
 *************************************************************/
 void Delete(const DataType & item);


/* Search2 locates node containing an item and its parent.
 *
 * Receive: The BST containing this function (implicitly)
 * item to be located
 * Pass back: Pointer locptr to node containing item -- null
 * if not found -- and parent pointing to its
 * parent
 *************************************************************/
 void Search2(const DataType & item, bool & found,
              BinNodePointer & locptr, BinNodePointer & parent);

// Has been modified ?
bool getModified() { return myModified; }
void setModified(bool l) { myModified=l; }

/*** Data members ***/
private:
  BinNodePointer root;

}; // end of class template declaration


//--- Definition of constructor
template <typename DataType>
inline BST<DataType>::BST() { 
	root = NULL;
	myLeaves=0; 
	pthread_mutex_init(&myMutex, NULL); 
	myModified=false;
}

template <typename DataType>
inline BST<DataType>::~BST()
{ Destroy(root); myLeaves=0; pthread_mutex_destroy(&myMutex); }


template <typename DataType>
void BST<DataType>::Destroy(BinNodePointer root)
{
  if(root != NULL)
  {
    Destroy(root->left);
    Destroy(root->right);
    delete root;
  }
}  

template<typename DataType>
 BST<DataType>::BST(const BST & original)
{ Lock(); CopyTree(original.root, root); Unlock(); pthread_mutex_init(&myMutex, NULL); }



template<typename DataType>
void BST<DataType>::CopyTree(const BinNodePointer original, BinNodePointer & root)  // NOT THREAD SAFE, UNLESS LOCKED BEFORE USE.
{
  if (original==NULL)
  {  
    root = NULL;
  }
  else
  {
 
    root = new BinNode(original->data);
    CopyTree(original->left, root->left);
    CopyTree(original->right, root->right);
  }
	 
}


template<typename DataType>
BST<DataType> & BST<DataType>::operator = (const BST<DataType> original)
{
  if(this != &original)
  {
   Destroy(root);
   Lock();
   CopyTree(original.root, root);
   Unlock();
  }
  return *this; 
}



//--- Definition of Empty()
template <typename DataType>
inline bool BST<DataType>::Empty() const
{ return root == NULL; }


//--- Definition of Search()

template <typename DataType>
bool BST<DataType>::Search(DataType & item) 
{
  BinNodePointer locptr = root;
  bool found = false;
  Lock();
  for (;;)
  {
    if (found || locptr == NULL) break;
    if (item < locptr->data) // descend left
      locptr = locptr->left;
    else if (item > locptr->data) // descend right
      locptr = locptr->right;
    else { // item found
      found = true;
      item=locptr->data;
    } 
  }
  Unlock();

  return found;
}

//--- Definition of Insert()
template <typename DataType>
void BST<DataType>::Insert(const DataType & item)
{
  BinNodePointer locptr = root, parent = NULL;        // pointer to parent of current node
  bool found = false;  // indicates if item already in BST

  Lock();
  myModified=true;
  for (;;) {
    if (found || locptr == NULL) break;
    if (item < locptr->data) {  // descend left
	parent = locptr;  
	locptr = locptr->left;
    } else if (item > locptr->data) {  // descend right
	parent = locptr;  
	locptr = locptr->right;
    } else // item found
	found = true;
  }
  if (found) {   // change this to update the record.
    BinNodePointer newPtr = new BinNode(item);
    newPtr->left=locptr->left;
    newPtr->right=locptr->right;
    if (parent == NULL) {
	    root=newPtr;
    } else if (parent->left == locptr) {
	    parent->left = newPtr;
    } else {
	    parent->right = newPtr;
    }
    delete locptr;
	    
  } else {
    locptr = new BinNode(item);   // construct node containing item
    if (parent == NULL)              // empty tree
      root = locptr;
    else if (item < parent->data) // insert to left of parent
      parent->left = locptr;
    else // insert to right of parent
      parent->right = locptr;
    myLeaves++;
  }
  Unlock();
}


//--- Definition of Inorder()
template <typename DataType>
void BST<DataType>::Inorder(void (myFunc)(DataType *, int &, void *), void *obj)
{ int i=1; Lock(); InorderAux(myFunc, root,i, obj); Unlock(); }

//---- Definition of PostOrder()
template <typename DataType>
void BST<DataType>::PostOrder(void (myFunc)(DataType *, int &, void *), void *obj)
{ int i=1; Lock(); PostOrderAux(myFunc, root, i, obj); Unlock();}

//---- Definition of PreOrder()
template <typename DataType>
void BST<DataType>::PreOrder(void (myFunc)(DataType *, int &, void *), void *obj)
{ int i=1; Lock();  PreOrderAux(myFunc, root, i, obj); Unlock(); }

//--- Definition of InorderAux()
template <typename DataType>
void BST<DataType>::InorderAux(void (myFunc)(DataType *, int &, void *), BinNodePointer ptr, int &currItem, void *obj) {
if (ptr != NULL)
  {
    InorderAux(myFunc, ptr->left, currItem, obj);
    myFunc(ptr->data, currItem, obj);
    currItem++;
    InorderAux(&myFunc, ptr->right, currItem, obj);
  }
}


//---- Definition of PostOrderAux()
template <typename DataType>
void BST<DataType>::PostOrderAux(void (myFunc)(DataType *, int &, void *), BinNodePointer ptr, int &currItem, void *obj)
{
   if(ptr !=NULL)
   {
     PostOrderAux(myFunc, ptr->left, currItem, obj);
     PostOrderAux(myFunc, ptr->right, currItem, obj);
     myFunc(&ptr->data, currItem, obj);
     currItem++;
   }
}


//---- Definition of PreOrderAux()
template <typename DataType>
void BST<DataType>::PreOrderAux(void (myFunc)(DataType *, int &, void *), BinNodePointer ptr, int &currItem, void *obj)
{
  if(ptr != NULL)
  { 
    //printf("Visit");
    //fflush(stdout);
    myFunc(&ptr->data, currItem, obj);
    currItem++;
    //printf("left->");
    //fflush(stdout);
    PreOrderAux(myFunc, ptr->left, currItem, obj);
    //printf("right->");
    //fflush(stdout);
    PreOrderAux(myFunc, ptr->right, currItem, obj);
  }
  //printf("up->");
  //fflush(stdout);
}



//--- Definition of Delete()
template <typename DataType>
void BST<DataType>::Delete(const DataType & item)
{
  bool found;          // signals if item is found
  BinNodePointer x,    // points to node containing item
  parent;              // R R parent of x and xSucc
  Search2(item, found, x, parent);
  if (!found)
  {
    return;
  }

  Lock();
  myModified=true;
  if (x->left != NULL && x->right != NULL)
  { // node has 2 children
    // Find xUs inorder successor and its parent
    BinNodePointer xSucc = x->right;
    parent = x;
    while (xSucc->left != NULL) // descend left
    {
      parent = xSucc;
      xSucc = xSucc->left;
    }

    // Move contents of xSucc to x and change x
    // to point to successor, which will be deleted.
    x->data = xSucc->data;
    x = xSucc;
  } // end if node has 2 children

  // Now proceed with case where node has 0 or 1 child
  BinNodePointer subtree = x->left; // pointer to a subtree of x
  if (subtree == NULL)
    subtree = x->right;
  if (parent == NULL) // root being deleted
    root = subtree;
  else if (parent->left == x) // left child of parent
    parent->left = subtree;
  else // right child of parent
    parent->right = subtree;
  delete x;
  myLeaves--;
  Unlock();
}


//--- Definition of Search2()
template <typename DataType>
void BST<DataType>::Search2(const DataType & item, bool & found,
                    BinNodePointer & locptr, BinNodePointer & parent)
{
  locptr = root;
  parent = NULL;
  found = false;
  Lock();
  for (;;)
  {
    if (found || locptr == NULL) {
	Unlock();
	return;
    }
    if (item < locptr->data) // descend left
    {
      parent = locptr;
      locptr = locptr->left;
    }
    else if (item > locptr->data) // descend right
    {
      parent = locptr;
      locptr = locptr->right;
    }
    else // item found
      found = true;
  }
  Unlock();
}

#endif

