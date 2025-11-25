#include "Literal.h"
#include <kiraz/Compiler.h>

namespace ast {

Node::SymTabEntry Id::get_symbol(const SymbolTable &st) const {
    return st.get_symbol(m_name);
}

Node::Ptr Id::compute_stmt_type(SymbolTable &st) {
    Node::compute_stmt_type(st);
    
    auto entry = get_symbol(st);
    if (!entry) {
        set_error(fmt::format("Identifier '{}' is not found", m_name));
        return shared_from_this();
    }
    
    return nullptr;
}

Node::Ptr Integer::compute_stmt_type(SymbolTable &st) {
    Node::compute_stmt_type(st);
    set_stmt_type(std::make_shared<Id>("Integer64"));
    return nullptr;
}

Node::Ptr Str::compute_stmt_type(SymbolTable &st) {
    Node::compute_stmt_type(st);
    set_stmt_type(std::make_shared<Id>("String"));
    return nullptr;
}

}