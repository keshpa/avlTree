#include <iostream>
#include <string>
#include "node.hpp"
#include "printTree.hpp"
#include <stdlib.h>
#include <time.h>

using namespace std;

int main()
{
	Node* root = nullptr;

	// Construct above tree    
	root = new Node(1);
	root->left = new Node(2);
	root->right = new Node(3);
	root->left->left = new Node(4);
	root->left->right = new Node(5);
	root->right->left = new Node(6);
	root->right->right = new Node(7);
	root->left->left->left = new Node(8);
	root->left->left->right = new Node(9);
	root->left->right->left = new Node(10);
	root->left->right->right = new Node(11);
	root->right->left->left = new Node(12);
	root->right->left->right = new Node(13);
	root->right->right->left = new Node(14);
	root->right->right->right = new Node(15);

	// print constructed binary tree
//	printTree pt(root);

//	cout << endl << endl << endl;

//	pt.print();

	avlTree avl;
	time_t curTime = time(nullptr);
	srandom(curTime);

	for (int i = 0; i < 50; ++i) {
		auto data  = random() % 100;
//		cout << "Now inserting : " << data << endl;
		avl.insert(data);
		avl.traverseForBalance(avl.getRoot());
	}

	printTree avlpt(avl.getRoot());

	avl.traverseForBalance(avl.getRoot());

	for (int i = 0; i < 500; ++i) {
		auto data  = random() % 100;
//		cout << "Now deleting : " << data << endl;
		avl.remove(data);
		avl.traverseForBalance(avl.getRoot());
	}

	avlpt.print(avl.getRoot());

	avl.traverseForBalance(avl.getRoot());
//	avl.insert(50);
//	avl.insert(60);
//	avl.insert(40);
//	avl.insert(30);
//	avl.insert(20);
//	avl.insert(35);
//	avl.insert(37);
//	avl.insert(36);
//	avl.insert(70);
//	printTree avlpt(avl.getRoot());
//	avlpt.print(avl.getRoot());

	return 0;
}
