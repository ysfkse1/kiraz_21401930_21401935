#ifndef KIRAZ_AST_OPERATOR_H
#define KIRAZ_AST_OPERATOR_H

#include <cassert>

#include <kiraz/Node.h>

namespace ast {

class Add : public Node {
public:
    Add(Node::Ptr left, Node::Ptr right) : m_left(left), m_right(right) {}
    std::string as_string() const override {
        return fmt::format("Add(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
private:
    Node::Ptr m_left;
    Node::Ptr m_right;
};

class Sub : public Node {
public:
    Sub(Node::Ptr left, Node::Ptr right) : m_left(left), m_right(right) {}
    std::string as_string() const override {
        return fmt::format("Sub(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
private:
    Node::Ptr m_left;
    Node::Ptr m_right;
};

class Mult : public Node {
public:
    Mult(Node::Ptr left, Node::Ptr right) : m_left(left), m_right(right) {}
    std::string as_string() const override {
        return fmt::format("Mult(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
private:
    Node::Ptr m_left;
    Node::Ptr m_right;
};

class DivF : public Node {
public:
    DivF(Node::Ptr left, Node::Ptr right) : m_left(left), m_right(right) {}
    std::string as_string() const override {
        return fmt::format("DivF(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
private:
    Node::Ptr m_left;
    Node::Ptr m_right;
};

class OpEq : public Node {
public:
    OpEq(Node::Ptr left, Node::Ptr right) : m_left(left), m_right(right) {}
    std::string as_string() const override {
        return fmt::format("OpEq(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
private:
    Node::Ptr m_left;
    Node::Ptr m_right;
};

class OpGt : public Node {
public:
    OpGt(Node::Ptr left, Node::Ptr right) : m_left(left), m_right(right) {}
    std::string as_string() const override {
        return fmt::format("OpGt(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
private:
    Node::Ptr m_left;
    Node::Ptr m_right;
};

class OpGe : public Node {
public:
    OpGe(Node::Ptr left, Node::Ptr right) : m_left(left), m_right(right) {}
    std::string as_string() const override {
        return fmt::format("OpGe(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
private:
    Node::Ptr m_left;
    Node::Ptr m_right;
};

class OpLt : public Node {
public:
    OpLt(Node::Ptr left, Node::Ptr right) : m_left(left), m_right(right) {}
    std::string as_string() const override {
        return fmt::format("OpLt(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
private:
    Node::Ptr m_left;
    Node::Ptr m_right;
};

class OpLe : public Node {
public:
    OpLe(Node::Ptr left, Node::Ptr right) : m_left(left), m_right(right) {}
    std::string as_string() const override {
        return fmt::format("OpLe(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
private:
    Node::Ptr m_left;
    Node::Ptr m_right;
};

}

#endif // KIRAZ_AST_OPERATOR_H