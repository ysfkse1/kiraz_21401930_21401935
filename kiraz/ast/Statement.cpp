#include "Statement.h"
#include "Literal.h"
#include <kiraz/Compiler.h>
 
namespace ast {
 
Node::Ptr Func::add_to_symtab_forward(SymbolTable &st) {
    auto name = dynamic_cast<Id*>(m_name.get())->get_name();
    
    
    auto existing = st.get_symbol(name);
    if (existing) {
        set_error(fmt::format("Identifier '{}' is already in symtab", name));
        return shared_from_this();
    }
    
    st.add_symbol(name, shared_from_this());
    return nullptr;
}
 
Node::Ptr Class::add_to_symtab_forward(SymbolTable &st) {
    auto name = dynamic_cast<Id*>(m_name.get())->get_name();
    
    
    auto existing = st.get_symbol(name);
    if (existing) {
        set_error(fmt::format("Identifier '{}' is already in symtab", name));
        return shared_from_this();
    }
    
    st.add_symbol(name, shared_from_this());
    return nullptr;
}
 
Node::Ptr ListNode::add_to_symtab_forward(SymbolTable &st) {
    for (auto &item : m_items) {
        auto err = item->add_to_symtab_forward(st);
        if (err) return err;
    }
    return nullptr;
}
 
Node::Ptr ListNode::compute_stmt_type(SymbolTable &st) {
    for (auto &item : m_items) {
        auto err = item->compute_stmt_type(st);
        if (err) return err;
    }
    return nullptr;
}
 
Node::Ptr Let::add_to_symtab_ordered(SymbolTable &st) {
    auto name = dynamic_cast<Id*>(m_name.get())->get_name();
    
    if (st.is_builtin(name)) {
        set_error(fmt::format("Overriding builtin '{}' is not allowed", name));
        return shared_from_this();
    }
    
    if (st.is_locally_defined(name) || st.has_symbol_in_parent(name)) {
        set_error(fmt::format("Identifier '{}' is already in symtab", name));
        return shared_from_this();
    }
    
    st.add_symbol(name, shared_from_this());
    return nullptr;
}
 
Node::Ptr Func::compute_stmt_type(SymbolTable &st) {
    Node::compute_stmt_type(st);
    
    auto scope = st.enter_scope(ScopeType::Func, shared_from_this());
    
    for (auto &stmt : m_stmts) {
        auto err = stmt->add_to_symtab_ordered(st);
        if (err) return err;
    }
    
    for (auto &stmt : m_stmts) {
        auto err = stmt->compute_stmt_type(st);
        if (err) return err;
    }
    
    return nullptr;
}
 
Node::Ptr Class::compute_stmt_type(SymbolTable &st) {
    Node::compute_stmt_type(st);
    
    auto scope = st.enter_scope(ScopeType::Class, shared_from_this());
    
    // Forward pass: add functions and classes first
    for (auto &stmt : m_stmts) {
        auto err = stmt->add_to_symtab_forward(st);
        if (err) return err;
    }
    
    // Ordered pass: add variables and other statements
    for (auto &stmt : m_stmts) {
        auto err = stmt->add_to_symtab_ordered(st);
        if (err) return err;
    }
    
    // Type checking pass
    for (auto &stmt : m_stmts) {
        auto err = stmt->compute_stmt_type(st);
        if (err) return err;
    }
    
    return nullptr;
}
 
Node::Ptr While::compute_stmt_type(SymbolTable &st) {
    Node::compute_stmt_type(st);
    
    auto scope_type = st.get_scope_type();
    if (scope_type == ScopeType::Module || scope_type == ScopeType::Class) {
        set_error("while statement is misplaced");
        return shared_from_this();
    }
    
    auto err = m_condition->compute_stmt_type(st);
    if (err) return err;
    
    for (auto &stmt : m_stmts) {
        err = stmt->compute_stmt_type(st);
        if (err) return err;
    }
    
    return nullptr;
}
 
Node::Ptr If::compute_stmt_type(SymbolTable &st) {
    Node::compute_stmt_type(st);
    
    auto scope_type = st.get_scope_type();
    if (scope_type == ScopeType::Module || scope_type == ScopeType::Class) {
        set_error("if statement is misplaced");
        return shared_from_this();
    }
    
    auto err = m_condition->compute_stmt_type(st);
    if (err) return err;
    
    for (auto &stmt : m_then_stmts) {
        err = stmt->compute_stmt_type(st);
        if (err) return err;
    }
    
    if (m_else_part) {
        err = m_else_part->compute_stmt_type(st);
        if (err) return err;
    }
    
    return nullptr;
}
 
Node::Ptr Return::compute_stmt_type(SymbolTable &st) {
    Node::compute_stmt_type(st);
    
    auto scope_type = st.get_scope_type();
    if (scope_type == ScopeType::Module || scope_type == ScopeType::Class) {
        set_error("return statement is misplaced");
        return shared_from_this();
    }
    
    auto err = m_expr->compute_stmt_type(st);
    if (err) return err;
    
    return nullptr;
}
 
Node::Ptr Assign::compute_stmt_type(SymbolTable &st) {
    Node::compute_stmt_type(st);
    
    auto scope_type = st.get_scope_type();
    if (scope_type == ScopeType::Module || scope_type == ScopeType::Class) {
        set_error("assignment is misplaced");
        return shared_from_this();
    }
    
    // Check if trying to override a builtin
    if (auto id = dynamic_cast<Id*>(m_left.get())) {
        auto name = id->get_name();
        if (st.is_builtin(name)) {
            set_error(fmt::format("Overriding builtin '{}' is not allowed", name));
            return shared_from_this();
        }
    }
    
    auto err = m_left->compute_stmt_type(st);
    if (err) return err;
    
    err = m_right->compute_stmt_type(st);
    if (err) return err;
    
    return nullptr;
}
 
}