#pragma once
#include <iostream>
#include <functional>
#include <string>
#include <assert.h>
using namespace std;

#define INTmax(a, b) ((a) > (b) ? (a) : (b))

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

	void setEdges(Node* curNode, bool left, bool right) {
		assert(curNode);
		if (left) {
			if (curNode->left) {
				curNode->leftEdges = INTmax(curNode->left->leftEdges, curNode->left->rightEdges) + 1;
			} else {
				curNode->leftEdges = 0;
			}
		}
		if (right) {
			if (curNode->right) {
				curNode->rightEdges = INTmax(curNode->right->leftEdges, curNode->right->rightEdges) + 1;
			} else {
				curNode->rightEdges = 0;
			}
		}
	}

	Imbalance detectImbalance(Node* curNode) {
		Imbalance imbalance = Imbalance::None;
		if (abs(curNode->leftEdges - curNode->rightEdges) > 1) {
			if (curNode->leftEdges > curNode->rightEdges) {
				// Can only have LLImbalance or LRImbalance
				assert(curNode->left);
				if (curNode->left->leftEdges >= curNode->left->rightEdges) {
					imbalance = Imbalance::LLImbalance;
				} else {
					imbalance = Imbalance::LRImbalance;
				}
			} else {
				// Can only have RLImbalance or RRImbalance
				assert(curNode->right);
				if (curNode->right->rightEdges >= curNode->right->leftEdges) {
					imbalance = Imbalance::RRImbalance;
				} else {
					imbalance = Imbalance::RLImbalance;
				}
			}
		}
		return imbalance;
	}

	void remove(int data) {
		if (root == nullptr) {
			return;
		}
		auto imbalance = removeImpl(&root, data);
		if (imbalance != Imbalance::None) {
			handleImbalance(&root, imbalance);
		}

		if (root) {
			setEdges(root, true, true);
		}
	}

	Imbalance removeImpl(Node** parent, int data) {
		if (not (*parent)) {
			return Imbalance::None;
		}
		assert(*parent);

		Node* curNode = *parent;
		Imbalance imbalance = Imbalance::None;

		if (data < curNode->data) {
			imbalance = removeImpl(&curNode->left, data);

			if (imbalance != Imbalance::None) {
				handleImbalance(&curNode->left, imbalance);
			}

			setEdges(curNode, true, false);
			imbalance = detectImbalance(curNode);
		} else if (data > curNode->data) {
			imbalance = removeImpl(&curNode->right, data);

			if (imbalance != Imbalance::None) {
				handleImbalance(&curNode->right, imbalance);
			}

			setEdges(curNode, false, true);
			imbalance = detectImbalance(curNode);
		} else {
//			cout << "Found victim node : " << curNode->data << " at address : " << hex << curNode << dec << endl;
			imbalance = deleteNode(parent, &curNode->data);
			if (imbalance != Imbalance::None) {
				handleImbalance(parent, imbalance);
			}
			if (*parent) {
				setEdges(*parent, true, true);
				imbalance = detectImbalance(*parent);
			} else {
				imbalance = Imbalance::None;
			}
		}
		return imbalance;
	}

	Imbalance deleteNode(Node** parent, int* replaceValue) {
		assert(parent);
		Node* curNode = *parent;
		assert(curNode);
		Imbalance imbalance = Imbalance::None;

		if (not curNode->right && not curNode->left) {
//			cout << "Found secondary victim node : " << curNode->data << " at address : " << hex << curNode << dec << endl;
			if (replaceValue) {
				*replaceValue = curNode->data;
			}
			free(curNode);
			*parent = nullptr;
			return Imbalance::None;
		}
		if (curNode->right) {
			if (curNode->right->left) {
				Node** newParent = &curNode->right;
				imbalance = pruneSmallestLeftNode(newParent, replaceValue);
				if (imbalance != Imbalance::None) {
					handleImbalance(newParent, imbalance);
				}
				if (*newParent) {
					setEdges(*newParent, true, true);
					imbalance = detectImbalance(*newParent);
				} else {
					imbalance = Imbalance::None;
				}
			} else {
//				cout << "Replacing old value : " << *replaceValue << " with new value : " << curNode->right->data << endl;
				*replaceValue = curNode->right->data;
//				cout << "Need replacement value for : " << curNode->right->data << endl;
				imbalance = deleteNode(&curNode->right, &curNode->right->data);
				if (imbalance != Imbalance::None) {
					handleImbalance(&curNode->right, imbalance);
				}
				setEdges(curNode, false, true);
				imbalance = detectImbalance(curNode);
			}
		} else {
			if (curNode->left->right) {
				imbalance = pruneLargestRightNode(&curNode->left, replaceValue);
				if (imbalance != Imbalance::None) {
					handleImbalance(&curNode->left, imbalance);
				}
				if (curNode->left) {
					setEdges(curNode->left, true, true);
					imbalance = detectImbalance(curNode->left);
				} else {
					imbalance = Imbalance::None;
				}
			} else {
//				cout << "Replacing old value : " << *replaceValue << " with new value : " << curNode->left->data << endl;
				curNode->data = curNode->left->data;
//				cout << "Need replacement value for : " << curNode->left->data << endl;
				imbalance = deleteNode(&curNode->left, &curNode->left->data);
				if (imbalance != Imbalance::None) {
					handleImbalance(&curNode->left, imbalance);
				}
				setEdges(curNode, true, false);
				imbalance = detectImbalance(curNode);
			}
		}
		return imbalance;
	}

	Imbalance pruneSmallestLeftNode(Node** parent, int* replacePayload) {
		Imbalance imbalance = Imbalance::None;
		assert(parent);
		Node* curNode = *parent;
		assert(curNode);

		if (curNode->left) {
			imbalance = pruneSmallestLeftNode(&curNode->left, replacePayload);
			if (imbalance != Imbalance::None) {
				handleImbalance(&curNode->left, imbalance);
			}
			setEdges(curNode, true, false);
			imbalance = detectImbalance(curNode);
		} else {
//			cout << "Replacing old value : " << *replacePayload << " with new value : " << curNode->data << endl;
			*replacePayload = curNode->data;
			if (curNode->right) {
//				cout << "Need replacement value for : " << curNode->data << endl;
				imbalance = deleteNode(&curNode, &curNode->data);
				if (imbalance != Imbalance::None) {
					handleImbalance(&curNode, imbalance);
				}
				setEdges(curNode, false, true);
				imbalance = detectImbalance(curNode);
			} else {
				free(curNode);
				*parent = nullptr;
			}
		}
		return imbalance;
	}

	Imbalance pruneLargestRightNode(Node** parent, int* replacePayload) {
		Imbalance imbalance = Imbalance::None;
		assert(parent);
		Node* curNode = *parent;
		assert(curNode);

		if (curNode->right) {
			imbalance = pruneLargestRightNode(&curNode->right, replacePayload);
			if (imbalance != Imbalance::None) {
				handleImbalance(&curNode->right, imbalance);
			}
			setEdges(curNode, false, true);
			imbalance = detectImbalance(curNode);
		} else {
//			cout << "Replacing old value : " << *replacePayload << " with new value : " << curNode->data << endl;
			*replacePayload = curNode->data;
			if (curNode->left) {
//				cout << "Need replacement value for : " << curNode->data << endl;
				imbalance = deleteNode(&curNode, &curNode->data);
				if (imbalance != Imbalance::None) {
					handleImbalance(&curNode, imbalance);
				}
				setEdges(curNode, true, true);
				imbalance = detectImbalance(curNode);
			} else {
				free(curNode);
				*parent = nullptr;
			}
		}
		return imbalance;
	}

	bool find(int data) {
		if (root == nullptr) {
			return false;
		}
		if (root->data == data) {
			return true;
		}
		return findTraverse(root, data);
	}

	bool findTraverse(Node* curNode, int data) {
		bool found = false;

		if (data > curNode->data) {
			if (curNode->right) {
				found = findTraverse(curNode->right, data);
			} else {
				return false;
			}
		} else if (data < curNode->data) {
			if (curNode->left) {
				found = findTraverse(curNode->left, data);
			} else {
				return false;
			}
		} else {
			found = true;
		}
		return found;
	}

	void insert(int data) {
		if (root == nullptr) {
			root = new Node(data);
			return;
		}
		if (data == root->data) {
			return;
		}

		bool nextTookRight = false;
		Imbalance imbalance = insertImpl(root, data, nextTookRight);

		if (imbalance != Imbalance::None) {
			handleImbalance(&root, imbalance);
		}

		setEdges(root, true, true);

		if (data < root->data) {
			imbalance = checkImbalance(root, true, nextTookRight);
			string myString = "Imbalance at root node after left insert : " + to_string(data);
			printImbalance(myString, imbalance);
		}
		if (data > root->data) {
			imbalance = checkImbalance(root, right, nextTookRight);
			string myString = "Imbalance at root node after right insert : " + to_string(data);
			printImbalance(myString, imbalance);
		}
	}

	Imbalance insertImpl(Node* curNode, int data, bool& tookRight) {
		assert(curNode);
		Imbalance imbalance = Imbalance::None;

		if (data == curNode->data) {
			return Imbalance::None;
		}
		Imbalance nodeImbalance = Imbalance::None;
		if (data > curNode->data) {
			tookRight = true;
			if (curNode->right) {
				bool nextTookRight = false;
				nodeImbalance = insertImpl(curNode->right, data, nextTookRight);
				if (nodeImbalance != Imbalance::None) {
					handleImbalance(&curNode->right, nodeImbalance);
				}

				assert(curNode->right);
				setEdges(curNode, false, true);

				// We can only have a RR or RL imbalance
				imbalance = checkImbalance(curNode, true, nextTookRight);
			} else {
				curNode->right = new Node(data);
				++curNode->rightEdges;
				return Imbalance::None;
			}
		} else {
			tookRight = false;;
			if (curNode->left) {
				bool nextTookRight = false;
				nodeImbalance = insertImpl(curNode->left, data, nextTookRight);
				if (nodeImbalance != Imbalance::None) {
					handleImbalance(&curNode->left, nodeImbalance);
				}

				assert(curNode->left);
				setEdges(curNode, true, false);

				// We can only have a LL or LR imbalance
				imbalance = checkImbalance(curNode, false, nextTookRight);

			} else {
				curNode->left = new Node(data);
				++curNode->leftEdges;
				return Imbalance::None;
			}
		}
		return imbalance;
	}

	void traverseForBalance(Node* curNode) {
		if (not curNode) {
			return;
		}
		
		if (not curNode->left) {
			if (curNode->leftEdges) {
				cout << "ERROR : Left edges should be zero at : " << curNode->data << ". leftEdges : " << curNode->leftEdges << endl;
			}
		} else {
			if (not curNode->left->data > curNode->data) {
				cout << "ERROR : Node with value : " << curNode->data << " has left child with greater value than parent : " << curNode->left->data << endl;
			}
			traverseForBalance(curNode->left);
			if (curNode->leftEdges != (INTmax(curNode->left->leftEdges, curNode->left->rightEdges) + 1)) {
				cout << "ERROR : Left edges should be : " << (INTmax(curNode->left->leftEdges, curNode->left->rightEdges) + 1) << " at : " << curNode->data << ". Actual leftEdges : " << curNode->leftEdges << endl;
			}
		}

		if (not curNode->right) {
			if (curNode->rightEdges) {
				cout << "ERROR : Right edges should be zero at : " << curNode->data << ". rightEdges : " << curNode->rightEdges << endl;
			}
		} else {
			if (not curNode->right->data > curNode->data) {
				cout << "ERROR : Node with value : " << curNode->data << " has right child with greater value than parent : " << curNode->right->data << endl;
			}
			traverseForBalance(curNode->right);
			if (curNode->rightEdges != (INTmax(curNode->right->leftEdges, curNode->right->rightEdges) + 1)) {
				cout << "ERROR : Right edges should be : " << (INTmax(curNode->right->leftEdges, curNode->right->rightEdges) + 1) << " at : " << curNode->data << ". Actual rightEdges : " << curNode->rightEdges << endl;
			}
		}

		if (abs(curNode->leftEdges -  curNode->rightEdges) > 1) {
			cout << "ERROR : Node with value : " << curNode->data << " is not balanced. The left edges value is : " << curNode->leftEdges << " and right edges value is : " << curNode->rightEdges << endl;
		}
		return;
	}

	Imbalance checkImbalance(Node* node, bool firstDirRight, bool nextDirRight) {
		assert(node);
		Imbalance imbalance = Imbalance::None;
		auto diff = abs(node->rightEdges - node->leftEdges);
		if (diff <= 1) {
			return Imbalance::None;
		}

		if (firstDirRight) { // first direction was right
			if (nextDirRight) {
				imbalance = Imbalance::RRImbalance;
			} else {
				imbalance = Imbalance::RLImbalance;
			}
		} else {
			if (not nextDirRight) {
				imbalance = Imbalance::LLImbalance;
			} else {
				imbalance = Imbalance::LRImbalance;
			}
		}
		return imbalance;
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

		/***************************************************************************

                        *-100                        *-50
                       / \                          / \
                      /   \                        /   \
                     *-50  *-200  ===>            *-10  *-100
                    / \                                / \
                   /   \                              /   \
                  *-10  *-60                         *-60  *-200


		***************************************************************************/
		Node* lRight = (*gP)->left->right;
		Node* lNode = (*gP)->left;

		(*gP)->left = lRight;
		lNode->right = (*gP);
		*gP = lNode;

		if (not lRight) {
			(*gP)->right->leftEdges = 0;
		} else {
			(*gP)->right->leftEdges = INTmax(lRight->leftEdges, lRight->rightEdges) + 1;
		}

		setEdges(*gP, false, true);
	}

	// We get the address of grand parent of the inserted node on which to operate on.
	void handleRRImbalance(Node** gP) {
		assert(*gP);
		assert((*gP)->right);

		/***************************************************************************

                        *-50                        *-100
                       / \                         / \
                      /   \                       /   \
                     *-10  *-100   ====>         *-50  *-110
                          / \                   / \
                         /   \                 /   \
                        *-60  *-110           *-10  *-60


		***************************************************************************/
		Node* rLeft = (*gP)->right->left;
		Node* rNode = (*gP)->right;

		(*gP)->right = rLeft;
		rNode->left = (*gP);
		*gP = rNode;

		if (not rLeft) {
			(*gP)->left->rightEdges = 0;
		} else {
			(*gP)->left->rightEdges = INTmax(rLeft->leftEdges, rLeft->rightEdges) + 1;
		}

		setEdges(*gP, true, false);
	}

	// We get the address of grand parent of the inserted node on which to operate on.
	void handleLRImbalance(Node** gP) {
		assert(*gP);
		assert((*gP)->left->right);

		/***************************************************************************
                                                       *-90
                                                      / \
                                                     /   \
                        *-100                       /     \
                       / \                         /       \
                      /   \                       /         \
                     *-50  *-200   ====>         *-50        *-200
                    / \     \                   / \         / \
                   /   \     \                 /   \       /   \
                  *-10  *-90  *-300           *-10  *-60  *-95  *-300
                       / \
                      /   \
                     *-60  *-95

		***************************************************************************/
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
			newR->right->leftEdges = INTmax(lrr->leftEdges, lrr->rightEdges) + 1;
		}

		if(not lrl) {
			newR->left->rightEdges = 0;
		} else {
			newR->left->rightEdges = INTmax(lrl->leftEdges, lrl->rightEdges) + 1;
		}

		setEdges(newR, true, true);

	}

	// We get the address of grand parent of the inserted node on which to operate on.
	void handleRLImbalance(Node** gP) {
		assert(*gP);
		assert((*gP)->right->left);

		/***************************************************************************
                                                       *-75
                                                      / \
                                                     /   \
                        *-50                        /     \
                       / \                         /       \
                      /   \                       /         \
                     *-10  *-100   ====>         *-50        *-100
                          / \                   / \         / \
                         /   \                 /   \       /   \
                        *-75  *-200           *-10  *-60  *-80  *-200
                       / \
                      /   \
                     *-60  *-80

		***************************************************************************/

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
			newR->left->rightEdges = INTmax(rll->leftEdges, rll->rightEdges) + 1;
		}

		if (not rlr) {
			newR->right->leftEdges = 0;
		} else {
			newR->right->leftEdges = INTmax(rlr->leftEdges, rlr->rightEdges) + 1;
		}

		setEdges(newR, true, true);
	}

	void printImbalance(string& prtStr, Imbalance imbalance) {
		if (imbalance == Imbalance::None) {
			return;
		}

		cout << prtStr;
		switch (imbalance) {
		case Imbalance::LLImbalance:
			cout << "LLImbalance" << endl;
			break;
		case Imbalance::LRImbalance:
			cout << "LRImbalance" << endl;
			break;
		case Imbalance::RRImbalance:
			cout << "RRImbalance" << endl;
			break;
		case Imbalance::RLImbalance:
			cout << "RLImbalance" << endl;
			break;
		default:
			cout << "RLImbalance" << endl;
			break;
		}
	}

private:
	Node* root;
};
