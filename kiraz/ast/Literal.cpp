#include "Literal.h"
#include "Statement.h"
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
    
    if (m_name == "this" && entry.stmt && entry.stmt->is_class()) {
        auto class_node = const_cast<Node*>(entry.stmt.get())->shared_from_this();
        auto class_name_id = dynamic_cast<Class*>(class_node.get())->get_name();
        set_stmt_type(class_name_id);
        return nullptr;
    }
    
    if (entry.stmt && entry.stmt->is_module()) {
        set_stmt_type(std::make_shared<Id>("Module"));
        return nullptr;
    }
    
    if (entry.stmt) {
        auto entry_type = entry.stmt->get_stmt_type();
        if (entry_type) {
            set_stmt_type(entry_type);
        }
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