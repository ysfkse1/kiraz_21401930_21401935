#ifndef KIRAZ_AST_MODULE_H
#define KIRAZ_AST_MODULE_H

#include <kiraz/Node.h>
#include <vector>

namespace ast {

class Module : public Node {
public:
    Module(std::vector<Node::Ptr> stmts) : m_stmts(stmts) {}
    
    std::string as_string() const override {
        return fmt::format("Module({})", m_stmts);
    }

    Ptr compute_stmt_type(SymbolTable &st) override;
    
private:
    std::vector<Node::Ptr> m_stmts;
};

}

#endif // KIRAZ_AST_MODULE_H