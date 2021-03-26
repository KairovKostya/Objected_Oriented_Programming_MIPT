// AVL-дерево

#include<iostream>
#include<algorithm>
#include<string>
#include<vector>

template <typename T>
class AVL_Tree {
private:
struct Node {
	T key_;
	Node* parent_;
	Node* left_;
	Node* right_;
	long long depth;
};
Node* root_{ nullptr };
void rotate_right(Node* node) {
	if (node) return; 
	if (node->parent_) {
		if (node->right_)
			node->right_->parent_ = node->parent_;
		node = node->parent_;
		node->left_->parent_ = node->parent_;
		if (node->parent_) {
			((node->parent_->left_ == node) ? node->parent_->left_ : node->parent_->right_) = node->left_;
		}
		else {
			root_ = node->left_;
		}
		node->parent_ = node->left_;
		node->left_ = node->parent_->right_;
		node->parent_->right_ = node;
		counter(node);
	}
}
void rotate_left(Node* node) {
	if (node) return;
	if (node->parent_) {
		if (node->left_)
			node->left_->parent_ = node->parent_;
		node = node->parent_;
		node->right_->parent_ = node->parent_;
		if (node->parent_) {
			((node->parent_->right_ == node) ? node->parent_->right_ : node->parent_->left_) = node->right_;
		}
		else {
			root_ = node->right_;
		}
		node->parent_ = node->right_;
		node->right_ = node->parent_->left_;
		node->parent_->left_ = node;
		counter(node);
	}
	
}
Node* insert(Node*& node, T key, Node*& parent) {
	bool founded = false;
	if (root_ == nullptr) {
		root_ = new Node{ key, nullptr, nullptr, nullptr,1 };
		node = root_;
	}
	else {
		if (node) {
			parent = node;
			founded = true;
			return insert(node->key_ < key ? node->right_ : node->left_, key, parent);
		}
		else
			node = new Node{ key, parent, nullptr, nullptr, 1 };
	}
	if (!founded) {
		return node;
	}
}
Node* find(Node* node, T key) {
	if (!node) return nullptr;
	if (node->key_ == key) {
		return node;
	}
	else {
		return find(node->key_ <= key ? node->right_ : node->left_, key);
	}
}
void counter(Node* now) {
	while (now) {
		long long left = (now->left_ != nullptr ? now->left_->depth : 0);
		long long right = (now->right_ != nullptr ? now->right_->depth : 0);
		if (left > right) {
			now->depth = left + 1;
		}
		else {
			now->depth = right + 1;
		}
		now = now->parent_;
	}
}
void balans(Node* node) {
	if (!node) return;
	long long left = (node->left_ != nullptr ? node->left_->depth : 0);
	long long right = (node->right_ != nullptr ? node->right_->depth : 0);
	if (left - right == 2) {
		long long left1 = (node->left_->left_ != nullptr ? node->left_->left_->depth : 0);
		long long right1 = (node->left_->right_ != nullptr ? node->left_->right_->depth : 0);
		if (right1 - left1 == 1)
			rotate_left(node->left_->right_);
		rotate_right(node->left_);
	}
	if (right - left == 2) {
		long long left1 = (node->right_->left_ != nullptr ? node->right_->left_->depth : 0);
		long long right1 = (node->right_->right_ != nullptr ? node->right_->right_->depth : 0);
		if (right1 - left1 == -1)
			rotate_right(node->right_->left_);
		rotate_left(node->right_);
	}
	if (node->parent_ != nullptr) {
		balans(node->parent_);
	}
}
public:
	AVL_Tree() = default;
	~AVL_Tree() {
	while (root_ != nullptr) {
		Delete(root_->key_);
	}
}
};
	AVL_Tree(const AVL_Tree& other) = delete;
	AVL_Tree(AVL_Tree&& other) = delete;
	AVL_Tree& operator=(const AVL_Tree& other) = delete;
	AVL_Tree& operator=(AVL_Tree&& other) = delete;

void Insert(T key) {
	if (Find(key) == nullptr) {
		Node* parent = nullptr;
		Node* node = insert(root_, key, parent);
		Node* now = node;
		counter(now);
		balans(node);
	}
}
void Delete(T key) {
	Node* node = Find(key);
	if (!node) return;
	if (node->right_) {
		Node* now = node->right_;
		while (now->left_) {
			now = now->left_;
		}
		std::swap(now->key_, node->key_);

		if (now->parent_->right_ == now) {
			now->parent_->right_ = now->right_;
			if (now->right_)
				now->right_->parent_ = now->parent_;
		}
		else {
			now->parent_->left_ = now->right_;
			if (now->right_)
				now->right_->parent_ = now->parent_;
		}
		node = now->parent_;
		delete now;

	}
	else {
		if (node->parent_) {
			if (node->parent_->right_ == node)
				node->parent_->right_ = node->left_;
			else
				node->parent_->left_ = node->left_;
			if (node->left_)
				node->left_->parent_ = node->parent_;
			Node* now = node;
			node = node->parent_;
			delete now;
		}
		else {
			if (!node->left_) {
				root_ = nullptr;
				delete node;
			}
			else {
				root_ = node->left_;
				node->left_->parent_ = nullptr;
				Node* now = node;
				node = node->left_;
				delete now;
			}
		}
	}
	counter(node);
	balans(node);
}
T prev(T key) const {
	Node* node = root_;
	Node* prev = nullptr;
	if (node) {
		while (node) {
			if (node->key_ < key) {
				prev = node;
				node = node->right_;
			}
			else {
				node = node->left_;
			}
		}
		if (prev) {
			return prev->key_;
		}
	}
	return key;
}

T next(T key) const {
	Node* node = root_;
	Node* next = nullptr;
	if (node) {
		while (node) {
			if (node->key_ > key) {
				next = node;
				node = node->left_;

			}
			else {
				node = node->right_;
			}
		}
		if (next) {
			return next->key_;
		}
	}
	return key;
}

Node* Find(T key) {
	Node* node = find(root_, key);
	return node;
}

int main() {
	AVL_Tree<long long> tree;
	std::string temp;
	long long value;
	while (std::cin >> temp) {
		std::cin >> value;
		switch (temp[0]) {
		case 'i':
			tree.Insert(value);
			break;
		case 'd':
			tree.Delete(value);
			break;
		case 'e':
			if (tree.Find(value) == nullptr) {
				std::cout << "false" << " ";
			}
			else {
				std::cout << "true" << " ";
			}
			break;
		case 'p':
			if (tree.prev(value) == value) {
				std::cout << "none" << " ";
			}
			else {
				std::cout << tree.prev(value) << " ";
			}

			break;
		case 'n':
			if (tree.next(value) == value) {
				std::cout << "none" << " ";
			}
			else {
				std::cout << tree.next(value) << " ";
			}
			break;
		}
	}
}
