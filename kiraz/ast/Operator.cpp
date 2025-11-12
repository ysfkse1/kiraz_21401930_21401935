#include "Operator.h"
#include <kiraz/Compiler.h>

namespace ast {

Node::Ptr Add::compute_stmt_type(SymbolTable &st) {
    Node::compute_stmt_type(st);
    
    auto scope_type = st.get_scope_type();
    if (scope_type == ScopeType::Module || scope_type == ScopeType::Class) {
        set_error("add operation is misplaced");
        return shared_from_this();
    }
    
    auto err = m_left->compute_stmt_type(st);
    if (err) return err;
    
    err = m_right->compute_stmt_type(st);
    if (err) return err;
    
    return nullptr;
}

Node::Ptr Sub::compute_stmt_type(SymbolTable &st) {
    Node::compute_stmt_type(st);
    
    auto scope_type = st.get_scope_type();
    if (scope_type == ScopeType::Module || scope_type == ScopeType::Class) {
        set_error("sub operation is misplaced");
        return shared_from_this();
    }
    
    auto err = m_left->compute_stmt_type(st);
    if (err) return err;
    
    err = m_right->compute_stmt_type(st);
    if (err) return err;
    
    return nullptr;
}

Node::Ptr Mult::compute_stmt_type(SymbolTable &st) {
    Node::compute_stmt_type(st);
    
    auto scope_type = st.get_scope_type();
    if (scope_type == ScopeType::Module || scope_type == ScopeType::Class) {
        set_error("mul operation is misplaced");
        return shared_from_this();
    }
    
    auto err = m_left->compute_stmt_type(st);
    if (err) return err;
    
    err = m_right->compute_stmt_type(st);
    if (err) return err;
    
    return nullptr;
}

Node::Ptr DivF::compute_stmt_type(SymbolTable &st) {
    Node::compute_stmt_type(st);
    
    auto scope_type = st.get_scope_type();
    if (scope_type == ScopeType::Module || scope_type == ScopeType::Class) {
        set_error("div operation is misplaced");
        return shared_from_this();
    }
    
    auto err = m_left->compute_stmt_type(st);
    if (err) return err;
    
    err = m_right->compute_stmt_type(st);
    if (err) return err;
    
    return nullptr;
}

Node::Ptr OpEq::compute_stmt_type(SymbolTable &st) {
    Node::compute_stmt_type(st);
    
    auto scope_type = st.get_scope_type();
    if (scope_type == ScopeType::Module || scope_type == ScopeType::Class) {
        set_error("comparison operation is misplaced");
        return shared_from_this();
    }
    
    auto err = m_left->compute_stmt_type(st);
    if (err) return err;
    
    err = m_right->compute_stmt_type(st);
    if (err) return err;
    
    return nullptr;
}

Node::Ptr OpGt::compute_stmt_type(SymbolTable &st) {
    Node::compute_stmt_type(st);
    
    auto scope_type = st.get_scope_type();
    if (scope_type == ScopeType::Module || scope_type == ScopeType::Class) {
        set_error("comparison operation is misplaced");
        return shared_from_this();
    }
    
    auto err = m_left->compute_stmt_type(st);
    if (err) return err;
    
    err = m_right->compute_stmt_type(st);
    if (err) return err;
    
    return nullptr;
}

Node::Ptr OpGe::compute_stmt_type(SymbolTable &st) {
    Node::compute_stmt_type(st);
    
    auto scope_type = st.get_scope_type();
    if (scope_type == ScopeType::Module || scope_type == ScopeType::Class) {
        set_error("comparison operation is misplaced");
        return shared_from_this();
    }
    
    auto err = m_left->compute_stmt_type(st);
    if (err) return err;
    
    err = m_right->compute_stmt_type(st);
    if (err) return err;
    
    return nullptr;
}

Node::Ptr OpLt::compute_stmt_type(SymbolTable &st) {
    Node::compute_stmt_type(st);
    
    auto scope_type = st.get_scope_type();
    if (scope_type == ScopeType::Module || scope_type == ScopeType::Class) {
        set_error("comparison operation is misplaced");
        return shared_from_this();
    }
    
    auto err = m_left->compute_stmt_type(st);
    if (err) return err;
    
    err = m_right->compute_stmt_type(st);
    if (err) return err;
    
    return nullptr;
}

Node::Ptr OpLe::compute_stmt_type(SymbolTable &st) {
    Node::compute_stmt_type(st);
    
    auto scope_type = st.get_scope_type();
    if (scope_type == ScopeType::Module || scope_type == ScopeType::Class) {
        set_error("comparison operation is misplaced");
        return shared_from_this();
    }
    
    auto err = m_left->compute_stmt_type(st);
    if (err) return err;
    
    err = m_right->compute_stmt_type(st);
    if (err) return err;
    
    return nullptr;
}

}