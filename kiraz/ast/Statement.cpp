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
    
    if (!name.empty() && std::islower(name[0])) {
        set_error(fmt::format("Class name '{}' can not start with an lowercase letter", name));
        return shared_from_this();
    }
    
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
    
    if (st.is_locally_defined(name)) {
        set_error(fmt::format("Identifier '{}' is already in symtab", name));
        return shared_from_this();
    }
    
    auto scope_type = st.get_scope_type();
    auto parent_scope_type = st.get_parent_scope_type();
    
    bool should_check_parent = 
        (scope_type == ScopeType::Module) ||
        (scope_type == ScopeType::Class) ||
        (scope_type == ScopeType::Func && parent_scope_type == ScopeType::Class);
    
    if (should_check_parent && st.has_symbol_in_parent(name)) {
        set_error(fmt::format("Identifier '{}' is already in symtab", name));
        return shared_from_this();
    }
    
    st.add_symbol(name, shared_from_this());
    return nullptr;
}
 
Node::Ptr Func::compute_stmt_type(SymbolTable &st) {
    Node::compute_stmt_type(st);
    
    auto func_name = dynamic_cast<Id*>(m_name.get())->get_name();
    
    auto ret_type_id = dynamic_cast<Id*>(m_ret_type.get());
    if (ret_type_id) {
        auto ret_type_name = ret_type_id->get_name();
        auto ret_type_entry = st.get_symbol(ret_type_name);
        if (!ret_type_entry) {
            set_error(fmt::format("Return type '{}' of function '{}' is not found",
                                ret_type_name, func_name));
            return shared_from_this();
        }
    }
    
    if (m_args) {
        auto func_args = dynamic_cast<FuncArgs*>(m_args.get());
        if (func_args) {
            std::set<std::string> arg_names;
            arg_names.insert(func_name);
            
            for (auto &arg : func_args->get_args()) {
                auto farg = dynamic_cast<FArg*>(arg.get());
                if (farg) {
                    auto arg_name_id = dynamic_cast<Id*>(farg->get_name().get());
                    auto arg_type_id = dynamic_cast<Id*>(farg->get_type().get());
                    
                    if (arg_name_id && arg_type_id) {
                        auto arg_name = arg_name_id->get_name();
                        auto arg_type_name = arg_type_id->get_name();
                        
                        if (arg_names.find(arg_name) != arg_names.end()) {
                            set_error(fmt::format("Identifier '{}' in argument list of function '{}' is already in symtab",
                                                arg_name, func_name));
                            return shared_from_this();
                        }
                        arg_names.insert(arg_name);
                        
                        auto arg_type_entry = st.get_symbol(arg_type_name);
                        if (!arg_type_entry) {
                            set_error(fmt::format("Identifier '{}' in type of argument '{}' in function '{}' is not found",
                                                arg_type_name, arg_name, func_name));
                            return shared_from_this();
                        }
                    }
                }
            }
        }
    }
    
    auto scope = st.enter_scope(ScopeType::Func, shared_from_this());
    
    for (auto &stmt : m_stmts) {
        auto err = stmt->add_to_symtab_ordered(st);
        if (err) return err;
    }
    
    for (auto &stmt : m_stmts) {
        auto err = stmt->compute_stmt_type(st);
        if (err) return err;
    }
    
    if (ret_type_id) {
        auto ret_type_name = ret_type_id->get_name();
        if (ret_type_name != "Null") {
            bool has_return = false;
            for (auto &stmt : m_stmts) {
                if (dynamic_cast<Return*>(stmt.get())) {
                    has_return = true;
                    break;
                }
            }
            if (!has_return) {
                set_error("Function is missing return value");
                return shared_from_this();
            }
        }
    }
    
    return nullptr;
}
 
Node::Ptr Class::compute_stmt_type(SymbolTable &st) {
    Node::compute_stmt_type(st);
    
    Node::Ptr parent_class_node = nullptr;
    if (m_parent) {
        auto parent_id = dynamic_cast<Id*>(m_parent.get());
        if (parent_id) {
            auto parent_name = parent_id->get_name();
            auto parent_entry = st.get_symbol(parent_name);
            if (!parent_entry) {
                set_error(fmt::format("Type '{}' is not found", parent_name));
                return shared_from_this();
            }
            parent_class_node = const_cast<Node*>(parent_entry.stmt.get())->shared_from_this();
        }
    }
    
    auto scope = st.enter_scope(ScopeType::Class, shared_from_this());
    
    m_cur_symtab = st.get_cur_symtab();
    
    if (parent_class_node) {
        auto parent_class = dynamic_cast<Class*>(parent_class_node.get());
        if (parent_class && parent_class->m_cur_symtab) {
            for (auto& [name, symbol] : parent_class->m_cur_symtab->symbols) {
                if (!st.is_builtin(name)) {
                    st.add_symbol(name, symbol);
                }
            }
        }
    }
    
    for (auto &stmt : m_stmts) {
        auto err = stmt->add_to_symtab_forward(st);
        if (err) return err;
    }
    
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
 
Node::Ptr While::compute_stmt_type(SymbolTable &st) {
    Node::compute_stmt_type(st);
    
    auto scope_type = st.get_scope_type();
    if (scope_type == ScopeType::Module || scope_type == ScopeType::Class) {
        set_error("while statement is misplaced");
        return shared_from_this();
    }
    
    auto err = m_condition->compute_stmt_type(st);
    if (err) return err;
    
    auto cond_type = m_condition->get_stmt_type();
    if (cond_type) {
        auto type_id = dynamic_cast<const Id*>(cond_type.get());
        if (type_id) {
            auto type_name = type_id->get_name();
            if (type_name != "Boolean") {
                set_error("While only accepts tests of type 'Boolean'");
                return shared_from_this();
            }
        }
    }
    
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
    
    auto cond_type = m_condition->get_stmt_type();
    if (cond_type) {
        auto type_id = dynamic_cast<const Id*>(cond_type.get());
        if (type_id) {
            auto type_name = type_id->get_name();
            if (type_name != "Boolean") {
                set_error("If only accepts tests of type 'Boolean'");
                return shared_from_this();
            }
        }
    }
    
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
    
    auto func_stmt = st.get_scope_stmt();
    auto func = dynamic_cast<Func*>(func_stmt.get());
    if (func) {
        auto ret_type_id = dynamic_cast<Id*>(func->get_ret_type().get());
        if (ret_type_id) {
            auto expected_type = ret_type_id->get_name();
            std::string actual_type;
            
            if (auto int_lit = dynamic_cast<Integer*>(m_expr.get())) {
                actual_type = "Integer64";
            } else if (auto str_lit = dynamic_cast<Str*>(m_expr.get())) {
                actual_type = "String";
            } else if (auto id = dynamic_cast<Id*>(m_expr.get())) {
                actual_type = "Unknown";
            }
            
            if (!actual_type.empty() && actual_type != expected_type && actual_type != "Unknown") {
                set_error(fmt::format("Return statement type '{}' does not match function return type '{}'",
                                    actual_type, expected_type));
                return shared_from_this();
            }
        }
    }
    
    return nullptr;
}
 
Node::Ptr Assign::compute_stmt_type(SymbolTable &st) {
    Node::compute_stmt_type(st);
    
    auto scope_type = st.get_scope_type();
    if (scope_type == ScopeType::Module || scope_type == ScopeType::Class) {
        set_error("assignment is misplaced");
        return shared_from_this();
    }
    
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

Node::SymTabEntry Class::get_subsymbol(Node::Ptr subsymbol) const {
    if (!m_cur_symtab) {
        return {};
    }
    
    auto id = dynamic_cast<Id*>(subsymbol.get());
    if (!id) {
        return {};
    }
    
    auto name = id->get_name();
    return m_cur_symtab->get_symbol(name);
}

const SymbolTable *Class::get_subsymbol_all() const {
    return nullptr;
}

Node::Ptr Call::compute_stmt_type(SymbolTable &st) {
    Node::compute_stmt_type(st);
    
    auto err = m_name->compute_stmt_type(st);
    if (err) return err;
    
    err = m_args->compute_stmt_type(st);
    if (err) return err;
    
    return nullptr;
}

Node::Ptr Dot::compute_stmt_type(SymbolTable &st) {
    Node::compute_stmt_type(st);
    
    auto err = m_left->compute_stmt_type(st);
    if (err) return err;
    
    auto left_id = dynamic_cast<Id*>(m_left.get());
    if (!left_id) {
        return nullptr;
    }
    
    auto left_name = left_id->get_name();
    auto left_entry = st.get_symbol(left_name);
    
    if (!left_entry) {
        set_error(fmt::format("Identifier '{}' is not found", left_name));
        return shared_from_this();
    }
    
    auto left_let = dynamic_cast<const Let*>(left_entry.stmt.get());
    Node::Ptr type_node = nullptr;
    
    if (left_let) {
        auto type_id = dynamic_cast<Id*>(left_let->get_type().get());
        if (!type_id) {
            return nullptr;
        }
        
        auto type_name = type_id->get_name();
        auto type_entry = st.get_symbol(type_name);
        
        if (!type_entry) {
            set_error(fmt::format("Type '{}' is not found", type_name));
            return shared_from_this();
        }
        
        type_node = const_cast<Node*>(type_entry.stmt.get())->shared_from_this();
    } else {
        type_node = const_cast<Node*>(left_entry.stmt.get())->shared_from_this();
    }
    
    auto right_id = dynamic_cast<Id*>(m_right.get());
    if (!right_id) {
        return nullptr;
    }
    
    auto right_name = right_id->get_name();
    
    if (st.is_builtin(right_name)) {
        set_error(fmt::format("Identifier '{}' has no subsymbol '{}'", left_name, right_name));
        return shared_from_this();
    }
    
    auto subsymbol = type_node->get_subsymbol(m_right);
    
    if (!subsymbol) {
        set_error(fmt::format("Identifier '{}.{}' is not found", left_name, right_name));
        return shared_from_this();
    }
    
    return nullptr;
}
 
}