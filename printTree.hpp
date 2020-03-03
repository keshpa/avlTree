#pragma once
#include <iostream>
#include <string>
#include "node.hpp"

using namespace std;

class printTree {
public:
	// Hold path information traversed from leaf to root so as to position the offset og leaf contents correctly.
	struct myTrunk {
		myTrunk* prev;
		bool printed;
		bool isLeft;
		myTrunk(myTrunk* prev, bool left) : prev(prev), isLeft(left), printed(false) {}
	};

	printTree(Node *node) {
		printImpl(node, nullptr, false);
		cout << endl << endl << endl;
	}

	// Print spaces or "|" character as appropriate starting with the root to the parent of the leaf ("trunk") node.
	void printTrunks(myTrunk* trunk) {
		if (trunk == nullptr) {
			return;
		}
		printTrunks(trunk->prev);
		if (not trunk->prev) {
				cout << "    ";
				return;
		}
		// If this intermediate is a left node, we want to print nothing till the node itself got printed and then a "|   "
		// to show the connection to this left leaf node.
		/*
			   ,---9
		   ,---7
   	    |   `---6 // The "|" is printed by trunk node for 7 after it is traversed after has itself been printed
		---5
   	    |   ,---4 // The "|" is printed by trunk node for 3 and printed when 3 is traversed but has itself not been printed
		   `---3
   	        `---2
		*/
		if (trunk->isLeft) {
			if (trunk->printed) {
				cout << "|   ";
			} else {
				cout << "    ";
			}
		}
		// If this is a right node, we need to show it's connection only till the node ifself gets printed and nothing
		// afterwards
		if (not trunk->isLeft) {
			if (not trunk->printed) {
				cout << "|   ";
			} else {
				cout << "    ";
			}
		}
	}

	void print(Node* newRoot) {
		printImpl(newRoot);
		cout << endl << endl << endl;
	}

private:
	// Traverse the tree from leaf to right (in-order) traversal and for each leaf call printTrunk to print-position the intermediates leading to the leat node.
	void printImpl(Node* curNode, myTrunk *prev = nullptr, bool left = false) {
		if (curNode == nullptr) {
			return;
		}
		myTrunk trunk(prev, left);
		printImpl(curNode->right, &trunk, true);
	
		printTrunks(trunk.prev); // Print spaces and/or "|" character leading to right position to print the contents of curNode.
		if (left && prev) {
			cout << ".---" << curNode->data << ":" << curNode->leftEdges << "l:" << curNode->rightEdges << "r" << endl;
		} else if (not left && prev) {
			cout << "`---" << curNode->data << ":" << curNode->leftEdges << "l:" << curNode->rightEdges << "r" << endl;
		}
		if (not prev) {
			cout << "----" << curNode->data << ":" << curNode->leftEdges << "l:" << curNode->rightEdges << "r" << endl;
		}
		trunk.printed = true;
	
		printImpl(curNode->left, &trunk, false);
		trunk.printed = false;
	};
};
