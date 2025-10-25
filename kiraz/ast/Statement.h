#ifndef KIRAZ_AST_STATEMENT_H
#define KIRAZ_AST_STATEMENT_H

#include <kiraz/Node.h>

namespace ast {

class Let : public Node {
public:
    Let(Node::Ptr name, Node::Ptr type, Node::Ptr init) 
        : m_name(name), m_type(type), m_init(init) {}
    
    std::string as_string() const override {
        std::string result = "Let(";
        if (m_name) {
            result += fmt::format("n={}", m_name->as_string());
        }
        if (m_type) {
            result += fmt::format(", t={}", m_type->as_string());
        }
        if (m_init) {
            result += fmt::format(", i={}", m_init->as_string());
        }
        result += ")";
        return result;
    }
    
private:
    Node::Ptr m_name;
    Node::Ptr m_type;
    Node::Ptr m_init;
};

}

#endif
