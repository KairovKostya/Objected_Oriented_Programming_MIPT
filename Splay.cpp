#include<iostream>
#include<algorithm>
#include<string>
#include<vector>

const long long mil = 1000000000;

template <typename T>
class SplayTree {
public:
	SplayTree() = default;
	~SplayTree();
	SplayTree(const SplayTree& other) = delete;
	SplayTree(SplayTree&& other) = delete;
	SplayTree& operator=(const SplayTree& other) = delete;
	SplayTree& operator=(SplayTree&& other) = delete;
private:
	struct Node {
		T key_;
		Node* parent_;
		Node* left_;
		Node* right_;
		long long sum;
	};
	Node* root_{ nullptr };

public:
	void rotate_right(Node* node, Node*& root) {
		if (node) {
			if (node->parent_) {
				if (node->right_)
					node->right_->parent_ = node->parent_;
				node = node->parent_;
				node->left_->parent_ = node->parent_;

				if (node->parent_) {
					((node->parent_->left_ == node) ? node->parent_->left_ : node->parent_->right_) = node->left_;
				}
				else {
					root = node->left_;
				}
				node->parent_ = node->left_;
				node->left_ = node->parent_->right_;
				node->parent_->right_ = node;
				counter(node);
			}
		}
	}
	void rotate_left(Node* node, Node*& root) {
		if (node) {
			if (node->parent_) {
				if (node->left_)
					node->left_->parent_ = node->parent_;
				node = node->parent_;
				node->right_->parent_ = node->parent_;

				if (node->parent_) {
					((node->parent_->right_ == node) ? node->parent_->right_ : node->parent_->left_) = node->right_;
				}
				else {
					root = node->right_;
				}
				node->parent_ = node->right_;
				node->right_ = node->parent_->left_;
				node->parent_->left_ = node;
				counter(node);
			}
		}
	}

	void counter(Node* now) {
		if (now) {
			long long left = (now->left_ != nullptr ? now->left_->sum : 0);
			long long right = (now->right_ != nullptr ? now->right_->sum : 0);
			now->sum = (left + right + now->key_);
			now = now->parent_;
			if (now) {
				long long left = (now->left_ != nullptr ? now->left_->sum : 0);
				long long right = (now->right_ != nullptr ? now->right_->sum : 0);
				now->sum = (left + right + now->key_);
				now = now->parent_;
			}
		}
	}

	void Splay(Node* node, Node*& root) {
		while (node->parent_) {
			if (!node->parent_->parent_) {
				if (node->parent_->left_ == node)
					rotate_right(node, root);
				else
					rotate_left(node, root);
			}
			else if (node->parent_->left_ == node) {
				if (node->parent_->parent_->left_ == node->parent_) {
					rotate_right(node->parent_, root);
					rotate_right(node, root);
				}
				else {
					rotate_right(node, root);
					rotate_left(node, root);
				}

			}
			else if (node->parent_->parent_->left_ == node->parent_) {
				rotate_left(node, root);
				rotate_right(node, root);

			}
			else {
				rotate_left(node->parent_, root);
				rotate_left(node, root);
			}
		}
	}

	T Sum(T left, T right) {
		Node* mid = nullptr;
		Split(root_, mid, left);
		Node* right_tree = nullptr;
		Split(mid, right_tree, right + 1);
		T sm = 0;
		if (mid)
			sm = mid->sum;
		Merge(mid, right_tree);
		Merge(root_, mid);
		return sm;
	}

	void Merge(Node*& left, Node*& right) {
		if (left && right) {
			Node* max = left;
			while (max->right_) {
				max = max->right_;
			}
			Splay(max, left);
			left = max;
			left->right_ = right;
			right->parent_ = left;
			counter(left);
		}
		else if (right) {
			left = right;
			right = nullptr;
		}
	}

	void Split(Node*& node, Node*& node3, T key) {
		Node* node1 = previous(node, key);
		if (node1) {
			Splay(node1, node);
			node = node1;
			if (node1->right_) {
				Node* node2 = node1->right_;
				node2->parent_ = nullptr;
				node1->right_ = nullptr;
				counter(node1);
				node3 = node2;
			}
			else
				node3 = nullptr;
		}
		else {
			node3 = node;
			node = nullptr;
		}
	}

	Node* insert(Node*& node, T key, Node*& parent) {
		if (Find(key) == nullptr) {
			bool fonded = 0;
			if (root_ == nullptr) {
				root_ = new Node{ key, nullptr, nullptr, nullptr, key % mil };
				node = root_;
			}
			else {
				if (node) {
					parent = node;
					fonded = 1;
					return insert(node->key_ < key ? node->right_ : node->left_, key, parent);
				}
				else
					node = new Node{ key, parent, nullptr, nullptr, key % mil };
			}
			if (!fonded) {
				return node;
			}
		}
	}

	void Insert(T key) {
		Node* parent = nullptr;
		Node* node = insert(root_, key, parent);
		Node* now = node;
		counter(node);
		Splay(node, root_);
	}

	Node* previous(Node* node, T key) {
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
				Splay(prev, node);
				return prev;
			}
		}
		return nullptr;
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

	Node* Find(T key) {
		Node* node = find(root_, key);
		return node;
	}

	void Delete(Node* node) {
		if (node->left_) {
			Delete(node->left_);
		}
		if (node->right_) {
			Delete(node->right_);
		}
		delete node;
	}

	~SplayTree() {
		Delete(root_);
	}
};
int main() {
	SplayTree<long long>tree;
	long long n;
	std::cin >> n;
	char now;
	long long prev = 0;
	for (long long i = 0; i < n; ++i) {
		std::cin >> now;
		if (now == '+') {
			long long add_value;
			std::cin >> add_value;
			tree.Insert((add_value + prev) % mil);
			prev = 0;
		}
		else {
			long long left, right;
			std::cin >> left >> right;

			prev = tree.Sum(left, right);
			std::cout << prev << " ";
		}
	}
}
