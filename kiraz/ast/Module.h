#ifndef KIRAZ_AST_MODULE_H
#define KIRAZ_AST_MODULE_H

#include <kiraz/Node.h>
#include <vector>
#include <memory>

class SymbolTable;
struct Scope;

namespace ast {

class Module : public Node {
public:
    Module(std::vector<Node::Ptr> stmts) : m_stmts(stmts) {}
    
    std::string as_string() const override {
        return fmt::format("Module({})", m_stmts);
    }

    bool is_module() const override { return true; }
    Ptr compute_stmt_type(SymbolTable &st) override;
    SymTabEntry get_subsymbol(Ptr subsymbol) const override;
    const SymbolTable *get_subsymbol_all() const override;
    Ptr gen_wat(WasmContext &ctx) override;
    
private:
    std::vector<Node::Ptr> m_stmts;
    std::shared_ptr<Scope> m_cur_symtab;
};

}

#endif // KIRAZ_AST_MODULE_H