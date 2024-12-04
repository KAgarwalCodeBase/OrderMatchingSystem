#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <vector>
#include <memory>
#include "Order.h"

class AVLTree {
private:
    struct Node {
    double price;                      // Price level
    std::vector<Order> orders;        // Orders at this price level
    std::shared_ptr<Node> left, right;
    int height;

    // Constructor with reordered initializer list
    Node(double p) : price(p), orders(), left(nullptr), right(nullptr), height(1) {}
};


    std::shared_ptr<Node> root;

    int getHeight(std::shared_ptr<Node> node) {
        return node ? node->height : 0;
    }

    int getBalance(std::shared_ptr<Node> node) {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }

    std::shared_ptr<Node> rotateRight(std::shared_ptr<Node> y) {
        auto x = y->left;
        auto T2 = x->right;
        x->right = y;
        y->left = T2;

        y->height = 1 + std::max(getHeight(y->left), getHeight(y->right));
        x->height = 1 + std::max(getHeight(x->left), getHeight(x->right));
        return x;
    }

    std::shared_ptr<Node> rotateLeft(std::shared_ptr<Node> x) {
        auto y = x->right;
        auto T2 = y->left;
        y->left = x;
        x->right = T2;

        x->height = 1 + std::max(getHeight(x->left), getHeight(x->right));
        y->height = 1 + std::max(getHeight(y->left), getHeight(y->right));
        return y;
    }

    std::shared_ptr<Node> insertNode(std::shared_ptr<Node> node, double price, const Order& order) {
        if (!node) {
            auto newNode = std::make_shared<Node>(price);
            newNode->orders.push_back(order);
            return newNode;
        }

        if (price < node->price)
            node->left = insertNode(node->left, price, order);
        else if (price > node->price)
            node->right = insertNode(node->right, price, order);
        else
            node->orders.push_back(order); // Same price level, add to the vector

        node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));

        int balance = getBalance(node);

        // Perform rotations if unbalanced
        if (balance > 1 && price < node->left->price)
            return rotateRight(node);

        if (balance < -1 && price > node->right->price)
            return rotateLeft(node);

        if (balance > 1 && price > node->left->price) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        if (balance < -1 && price < node->right->price) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    std::shared_ptr<Node> getMinValueNode(std::shared_ptr<Node> node) {
        auto current = node;
        while (current->left)
            current = current->left;
        return current;
    }

    std::shared_ptr<Node> deleteNode(std::shared_ptr<Node> node, double price) {
        if (!node)
            return node;

        if (price < node->price)
            node->left = deleteNode(node->left, price);
        else if (price > node->price)
            node->right = deleteNode(node->right, price);
        else {
            if (!node->left || !node->right) {
                node = (node->left) ? node->left : node->right;
            } else {
                auto temp = getMinValueNode(node->right);
                node->price = temp->price;
                node->orders = temp->orders;
                node->right = deleteNode(node->right, temp->price);
            }
        }

        if (!node)
            return node;

        node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));

        int balance = getBalance(node);

        if (balance > 1 && getBalance(node->left) >= 0)
            return rotateRight(node);

        if (balance > 1 && getBalance(node->left) < 0) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        if (balance < -1 && getBalance(node->right) <= 0)
            return rotateLeft(node);

        if (balance < -1 && getBalance(node->right) > 0) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    void findLowestHelper(std::shared_ptr<Node> node, double price, std::vector<Order>& result) const {
        if (!node) return;

        if (node->price <= price) {
            // Add all orders at this price
            result.insert(result.end(), node->orders.begin(), node->orders.end());
            // Check the left subtree for smaller prices
            findLowestHelper(node->left, price, result);
            // Check the right subtree for prices less than or equal to `price`
            findLowestHelper(node->right, price, result);
        } else {
            // Only check the left subtree
            findLowestHelper(node->left, price, result);
        }
    }

    void findHighestHelper(std::shared_ptr<Node> node, double price, std::vector<Order>& result) const {
        if (!node) return;

        if (node->price >= price) {
            // Add all orders at this price
            result.insert(result.end(), node->orders.begin(), node->orders.end());
            // Check the right subtree for larger prices
            findHighestHelper(node->right, price, result);
            // Check the left subtree for prices greater than or equal to `price`
            findHighestHelper(node->left, price, result);
        } else {
            // Only check the right subtree
            findHighestHelper(node->right, price, result);
        }
    }

public:
    AVLTree() : root(nullptr) {}

    void insert(double price, const Order& order) {
        root = insertNode(root, price, order);
    }

    void remove(double price) {
        root = deleteNode(root, price);
    }

    std::vector<Order> find(double price) {
        auto current = root;
        while (current) {
            if (price < current->price)
                current = current->left;
            else if (price > current->price)
                current = current->right;
            else
                return current->orders;
        }
        return {};
    }

    std::vector<Order> findLowestLessThanEqual(double price) const {
        std::vector<Order> result;
        findLowestHelper(root, price, result);
        return result;
    }

    std::vector<Order> findHighestGreaterThanEqual(double price) const {
        std::vector<Order> result;
        findHighestHelper(root, price, result);
        return result;
    }
};

#endif // AVL_TREE_H
