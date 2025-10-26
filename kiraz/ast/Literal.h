#ifndef KIRAZ_AST_LITERAL_H
#define KIRAZ_AST_LITERAL_H

#include <kiraz/Node.h>

namespace ast {
class Integer : public Node {
public:
    Integer(long long value) : m_value(value) {}
    std::string as_string() const override {
        return fmt::format("Int({})", m_value);
    }
private:
    long long m_value;
};

class Id : public Node {
public:
    Id(const std::string& name) : m_name(name) {}
    std::string as_string() const override {
        return fmt::format("Id({})", m_name);
    }
private:
    std::string m_name;
};

class SignedNode : public Node {
public:
    SignedNode(Node::Ptr operand) : m_operand(operand) {}
    std::string as_string() const override {
        return fmt::format("Signed(OP_MINUS, {})", m_operand->as_string());
    }
private:
    Node::Ptr m_operand;
};

class Str : public Node {
public:
    Str(const std::string& value) : m_value(value) {}
    std::string as_string() const override {
        return fmt::format("Str({})", m_value);
    }
private:
    std::string m_value;
};
}

#endif