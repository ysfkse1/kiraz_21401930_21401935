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

class SignedNode : public Node {
public:
// TODO

};
}

#endif
