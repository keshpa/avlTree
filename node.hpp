#pragma once
#include <iostream>
#include <string>
#include <assert.h>
using namespace std;

#define max(a, b) ((a) > (b) ? (a) : (b))

// Data structure to store a Binary Tree node
class Node {
public:
	Node(int data)
	{
		this->data = data;
		this->left = this->right = nullptr;
		leftEdges = 0;
		rightEdges = 0;
	}

	friend class printTree;
	friend class avlTree;
	Node *left, *right;
private:
	int data;
	int leftEdges;
	int rightEdges;
};

class avlTree {
public:
	avlTree() : root(nullptr) {}

	Node* getRoot() const {
		return root;
	}

	enum class Imbalance {
		None = 0,
		LLImbalance,
		LRImbalance,
		RRImbalance,
		RLImbalance,
		Unknown
	};

	int insert(int data) {
		if (root == nullptr) {
			root = new Node(data);
			return 1;
		}
		if (data == root->data) {
			return 0;
		}

		int gcDir; //grand child direction
		Imbalance imbalance = Imbalance::None;
		int inserted = insertImpl(root, data, gcDir, imbalance);

		if (imbalance != Imbalance::None) {
			if (handleImbalance(&root, imbalance)) {
				inserted = 0; // Since we handle the imbalance, the added edge due to the just inserted node has been squashed
			}
		}

		if (inserted && data < root->data) {
			if (abs(root->rightEdges - root->leftEdges) > 1) {
				if (gcDir) {
					cout << "RR Imbalance at root node : " << root->data << " when inserting : " << data << endl;
					imbalance = Imbalance::RRImbalance;
				} else {
					cout << "RL Imbalance at root node : " << root->data << " when inserting : " << data << endl;
					imbalance = Imbalance::RLImbalance;
				}
			}
		}
		if (inserted && data > root->data) {
			if (abs(root->rightEdges - root->leftEdges) > 1) {
				if (gcDir) {
					cout << "RR Imbalance at root node : " << root->data << " when inserting : " << data << endl;
					imbalance = Imbalance::RRImbalance;
				} else {
					cout << "RL Imbalance at root node : " << root->data << " when inserting : " << data << endl;
					imbalance = Imbalance::RLImbalance;
				}
			}
		}
	}

	int insertImpl(Node* curNode, int data, int& gcDir, Imbalance& imbalance) {
		assert(curNode);
		if (data == curNode->data) {
			return 0;
		}
		int inserted = 0;
		Imbalance nodeImbalance = Imbalance::None;
		if (data > curNode->data) {
			if (curNode->right) {
				gcDir = 1;
				int nextGCDir;
				inserted = insertImpl(curNode->right, data, nextGCDir, nodeImbalance);
				if (nodeImbalance != Imbalance::None) {
					if (handleImbalance(&curNode->right, nodeImbalance)) {
						inserted = 0; // Since we handle the imbalance, the added edge due to the just inserted node has been squashed
					}
				}

				if (inserted) {
					++curNode->rightEdges;
				}
				// We can only have a RR or RL imbalance
				if (abs(curNode->rightEdges - curNode->leftEdges) > 1) {
					if (nextGCDir) {
						cout << "RR Imbalance at node : " << curNode->data << " when inserting : " << data << endl;
						imbalance = Imbalance::RRImbalance;
					} else {
						cout << "RL Imbalance at node : " << curNode->data << " when inserting : " << data << endl;
						imbalance = Imbalance::RLImbalance;
					}
				}
			} else {
				gcDir = 1; // Right direction is a one, left is a zero
				curNode->right = new Node(data);
				++curNode->rightEdges;
				if (not curNode->leftEdges) {
					return 1;
				} else {
					return 0;
				}
			}
		} else {
			if (curNode->left) {
				gcDir = 0;
				int nextGCDir;
				inserted = insertImpl(curNode->left, data, nextGCDir, nodeImbalance);
				if (nodeImbalance != Imbalance::None) {
					if (handleImbalance(&curNode->left, nodeImbalance)) {
						inserted = 0; // Since we handle the imbalance, the added edge due to the just inserted node has been squashed
					}
				}
				if (inserted) {
					++curNode->leftEdges;
				}
				// We can only have a LL or LR imbalance
				if (abs(curNode->rightEdges - curNode->leftEdges) > 1) {
					if (nextGCDir) {
						cout << "LR Imbalance at node : " << curNode->data << " when inserting : " << data << endl;
						imbalance = Imbalance::LRImbalance;
					} else {
						cout << "LL Imbalance at node : " << curNode->data << " when inserting : " << data << endl;
						imbalance = Imbalance::LLImbalance;
					}
				}
			} else {
				gcDir = 0; // Right direction is a one, left is a zero
				curNode->left = new Node(data);
				++curNode->leftEdges;
				if (not curNode->rightEdges) {
					return 1;
				} else {
					return 0;
				}
			}
		}
		return inserted;
	}

	bool handleImbalance(Node** anchor, Imbalance imbalance) {
		bool handled = true;
		switch (imbalance) {
			case Imbalance::LLImbalance:
				handleLLImbalance(anchor);
				break;
			case Imbalance::LRImbalance:
				handleLRImbalance(anchor);
				break;
			case Imbalance::RRImbalance:
				handleRRImbalance(anchor);
				break;
			case Imbalance::RLImbalance:
				handleRLImbalance(anchor);
				break;
			default:
				cout << "This tree is screwed. I don't understand the type of imbalance encountered." << endl;
				handled = false;
				break;
		}
		return handled;
	}

	// We get the address of grand parent of the inserted node on which to operate on.
	void handleLLImbalance(Node** gP) {
		assert(*gP);
		assert((*gP)->left);

		Node* lRight = (*gP)->left->right;
		Node* lNode = (*gP)->left;

		(*gP)->left = lRight;
		lNode->right = (*gP);
		*gP = lNode;

		if (not lRight) {
			(*gP)->right->leftEdges = 0;
		} else {
			(*gP)->right->leftEdges = max(lRight->leftEdges, lRight->rightEdges) + 1;
		}

		(*gP)->rightEdges = max((*gP)->right->rightEdges, (*gP)->right->leftEdges) + 1;
	}

	// We get the address of grand parent of the inserted node on which to operate on.
	void handleRRImbalance(Node** gP) {
		assert(*gP);
		assert((*gP)->right);

		Node* rLeft = (*gP)->right->left;
		Node* rNode = (*gP)->right;

		(*gP)->right = rLeft;
		rNode->left = (*gP);
		*gP = rNode;

		if (not rLeft) {
			(*gP)->left->rightEdges = 0;
		} else {
			(*gP)->left->rightEdges = max(rLeft->leftEdges, rLeft->rightEdges) + 1;
		}

		(*gP)->leftEdges = max((*gP)->left->rightEdges, (*gP)->left->leftEdges) + 1;
	}

	// We get the address of grand parent of the inserted node on which to operate on.
	void handleLRImbalance(Node** gP) {
		assert(*gP);
		assert((*gP)->left->right);

		Node* l = (*gP)->left;
		Node* r = (*gP)->right;
		Node* lrr = (*gP)->left->right->right;
		Node* lrl = (*gP)->left->right->left;
		Node* newR = (*gP)->left->right;

		newR->right = *gP;
		newR->left = l;
		l->right = lrl;
		newR->right->left = lrr;
		*gP = newR;

		if (not lrr) {
			newR->right->leftEdges = 0;
		} else {
			newR->right->leftEdges = max(lrr->leftEdges, lrr->rightEdges) + 1;
		}

		if(not lrl) {
			newR->left->rightEdges = 0;
		} else {
			newR->left->rightEdges = max(lrl->leftEdges, lrl->rightEdges) + 1;
		}

		newR->rightEdges = max(newR->right->leftEdges, newR->right->rightEdges) + 1;
		newR->leftEdges = max(newR->left->leftEdges, newR->left->rightEdges) + 1;

	}

	// We get the address of grand parent of the inserted node on which to operate on.
	void handleRLImbalance(Node** gP) {
		assert(*gP);
		assert((*gP)->right->left);

		Node* r = (*gP)->right;
		Node* l = (*gP)->left;
		Node* rll = (*gP)->right->left->left;
		Node* rlr = (*gP)->right->left->right;
		Node* newR = (*gP)->right->left;

		newR->right = r;
		newR->left = (*gP);
		newR->left->right = rll;
		r->left = rlr;
		(*gP) = newR;

		if (not rll) {
			newR->left->rightEdges = 0;
		} else {
			newR->left->rightEdges = max(rll->leftEdges, rll->rightEdges) + 1;
		}

		if (not rlr) {
			newR->right->leftEdges = 0;
		} else {
			newR->right->leftEdges = max(rlr->leftEdges, rlr->rightEdges) + 1;
		}

		newR->leftEdges = max(newR->left->leftEdges, newR->left->rightEdges) + 1;
		newR->rightEdges = max(newR->right->leftEdges, newR->right->rightEdges) + 1;
	}

private:
	Node* root;
};
