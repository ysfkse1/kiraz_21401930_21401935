#include "Operator.h"
#include "Literal.h"
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
    
    auto left_type = m_left->get_stmt_type();
    auto right_type = m_right->get_stmt_type();
    
    if (left_type && right_type) {
        auto left_id = dynamic_cast<const Id*>(left_type.get());
        auto right_id = dynamic_cast<const Id*>(right_type.get());
        
        if (left_id && right_id) {
            auto left_name = left_id->get_name();
            auto right_name = right_id->get_name();
            
            if (left_name != right_name) {
                set_error(fmt::format("Operator '+' not defined for types '{}' and '{}'", 
                                     left_name, right_name));
                return shared_from_this();
            }
            
            set_stmt_type(left_type);
        }
    }
    
    return nullptr;
}

Node::Ptr Add::gen_wat(WasmContext &ctx) {
    auto err = m_left->gen_wat(ctx);
    if (err) return err;
    
    err = m_right->gen_wat(ctx);
    if (err) return err;
    
    ctx.body() << "i64.add\n";
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

Node::Ptr OpEq::gen_wat(WasmContext &ctx) {
    auto err = m_left->gen_wat(ctx);
    if (err) return err;
    
    err = m_right->gen_wat(ctx);
    if (err) return err;
    
    ctx.body() << "i64.eq\n";
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