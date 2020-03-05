#include <iostream>
#include <string>
#include "node.hpp"
#include "printTree.hpp"
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <algorithm>

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

	printTree avlpt(avl.getRoot());

	std::vector<int> myInts;
	for (int i = 0; i < 10000; ++i) {
		auto data  = random() % 1000;
//		cout << "Now inserting : " << data << endl;
		avl.insert(data);
		auto it = lower_bound(myInts.begin(), myInts.end(), data);
		if (it == myInts.end() || *it != data) {
			myInts.insert(it, data);
		}
		for (int storedValue : myInts) {
			auto found = avl.find(storedValue);
			if (not found) {
				cout << "Did not find value : " << storedValue << " in AVL Tree." << endl;
				avlpt.print(avl.getRoot());
				assert(0);
			}
		}
		avl.traverseForBalance(avl.getRoot());
	}


	avl.traverseForBalance(avl.getRoot());

	for (int i = 0; i < 10000; ++i) {
		auto data  = random() % 1000;
		cout << "Now deleting : " << data << endl;
		auto it = lower_bound(myInts.begin(), myInts.end(), data);
		if (it != myInts.end() && *it == data) {
			myInts.erase(it);
		}
		avl.remove(data);

		int storedValue;
		for (int storedValue : myInts) {
			auto found = avl.find(storedValue);
			if (not found) {
				cout << "Did not find value : " << storedValue << " in AVL Tree." << endl;
				avlpt.print(avl.getRoot());
				assert(0);
			}
		}

		avlpt.print(avl.getRoot());
		avl.traverseForBalance(avl.getRoot());
	}

	for (int present : myInts) {
		bool found = avl.find(present);
		if (not found) {
			cout << "Did not find value : " << present << " in AVL Tree." << endl;
			avlpt.print(avl.getRoot());
			assert(0);
		}
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
