#include "Statement.h"
#include "Literal.h"
#include "Module.h"
#include <kiraz/Compiler.h>
#include <fstream>
 
namespace ast {

Node::Ptr FuncArgs::compute_stmt_type(SymbolTable &st) {
    Node::compute_stmt_type(st);
    
    for (auto &arg : m_args) {
        auto err = arg->compute_stmt_type(st);
        if (err) return err;
    }
    
    return nullptr;
}
 
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
        (scope_type == ScopeType::Class) ||
        (scope_type == ScopeType::Func && parent_scope_type != ScopeType::Func);
    
    if (should_check_parent && st.has_symbol_in_parent(name)) {
        set_error(fmt::format("Identifier '{}' is already in symtab", name));
        return shared_from_this();
    }
    
    st.add_symbol(name, shared_from_this());
    return nullptr;
}

Node::Ptr Let::compute_stmt_type(SymbolTable &st) {
    Node::compute_stmt_type(st);
    
    if (m_init) {
        auto err = m_init->compute_stmt_type(st);
        if (err) return err;
    }
    
    if (m_type) {
        set_stmt_type(m_type);
        
        if (m_init) {
            auto init_type = m_init->get_stmt_type();
            if (init_type) {
                auto explicit_type_id = dynamic_cast<Id*>(m_type.get());
                auto init_type_id = dynamic_cast<const Id*>(init_type.get());
                
                if (explicit_type_id && init_type_id) {
                    auto explicit_name = explicit_type_id->get_name();
                    auto init_name = init_type_id->get_name();
                    
                    if (explicit_name != init_name) {
                        bool is_subclass = false;
                        auto init_class_entry = st.get_symbol(init_name);
                        if (init_class_entry) {
                            auto init_class = dynamic_cast<const Class*>(init_class_entry.stmt.get());
                            if (init_class) {
                                auto parent = init_class->get_parent_class();
                                while (parent) {
                                    auto parent_id = dynamic_cast<Id*>(parent.get());
                                    if (parent_id && parent_id->get_name() == explicit_name) {
                                        is_subclass = true;
                                        break;
                                    }
                                    auto parent_entry = st.get_symbol(parent_id->get_name());
                                    if (parent_entry) {
                                        auto parent_class = dynamic_cast<const Class*>(parent_entry.stmt.get());
                                        if (parent_class) {
                                            parent = parent_class->get_parent_class();
                                        } else {
                                            break;
                                        }
                                    } else {
                                        break;
                                    }
                                }
                            }
                        }
                        
                        if (!is_subclass) {
                            set_error(fmt::format("Initializer type '{}' does not match explicit type '{}'",
                                                 init_name, explicit_name));
                            return shared_from_this();
                        }
                    }
                }
            }
        }
    } else if (m_init) {
        auto init_type = m_init->get_stmt_type();
        if (init_type) {
            set_stmt_type(init_type);
        }
    }
    
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
    
    {
        auto scope = st.enter_scope(ScopeType::Func, shared_from_this());
        
        for (auto &stmt : m_stmts) {
            auto err = stmt->add_to_symtab_ordered(st);
            if (err) return err;
        }
        
        for (auto &stmt : m_stmts) {
            auto err = stmt->compute_stmt_type(st);
            if (err) return err;
        }
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
    
   
    st.add_symbol("this", shared_from_this());
    
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
            
            auto expr_type = m_expr->get_stmt_type();
            if (expr_type) {
                auto expr_type_id = dynamic_cast<const Id*>(expr_type.get());
                if (expr_type_id) {
                    auto actual_type = expr_type_id->get_name();
                    
                    if (actual_type != expected_type) {
                        set_error(fmt::format("Return statement type '{}' does not match function return type '{}'",
                                            actual_type, expected_type));
                        return shared_from_this();
                    }
                }
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
    
    auto left_type = m_left->get_stmt_type();
    auto right_type = m_right->get_stmt_type();
    
    if (left_type && right_type) {
        auto left_type_id = dynamic_cast<const Id*>(left_type.get());
        auto right_type_id = dynamic_cast<const Id*>(right_type.get());
        
        if (left_type_id && right_type_id) {
            auto left_name = left_type_id->get_name();
            auto right_name = right_type_id->get_name();
            
            if (left_name != right_name) {
                set_error(fmt::format("Left type '{}' of assignment does not match the right type '{}'",
                                     left_name, right_name));
                return shared_from_this();
            }
            
            if (left_name == "Module" && right_name == "Module") {
                auto left_id = dynamic_cast<Id*>(m_left.get());
                if (left_id) {
                    set_error(fmt::format("Overriding imported module '{}' is not allowed", left_id->get_name()));
                    return shared_from_this();
                }
            }
        }
    }
    
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
    
    std::string func_name;
    const Func *func_node = nullptr;
    
    if (auto name_id = dynamic_cast<Id*>(m_name.get())) {
        func_name = name_id->get_name();
        auto func_entry = st.get_symbol(func_name);
        
        if (func_entry) {
            func_node = dynamic_cast<const Func*>(func_entry.stmt.get());
        }
    } else if (auto dot = dynamic_cast<Dot*>(m_name.get())) {
        auto left_id = dynamic_cast<Id*>(dot->get_left().get());
        auto right_id = dynamic_cast<Id*>(dot->get_right().get());
        
        if (left_id && right_id) {
            auto module_name = left_id->get_name();
            auto method_name = right_id->get_name();
            func_name = module_name + "." + method_name;
            
            auto module_entry = st.get_symbol(module_name);
            if (module_entry && module_entry.stmt) {
                auto subsymbol = module_entry.stmt->get_subsymbol(dot->get_right());
                if (subsymbol && subsymbol.stmt) {
                    func_node = dynamic_cast<const Func*>(subsymbol.stmt.get());
                }
            }
        }
    }
    
    if (func_node) {
        auto ret_type = func_node->get_ret_type();
        if (ret_type) {
            set_stmt_type(ret_type);
        }
        
        auto func_args_node = func_node->get_args();
        auto call_args_node = m_args.get();
        
        auto func_args = dynamic_cast<const FuncArgs*>(func_args_node.get());
        auto call_args = dynamic_cast<const FuncArgs*>(call_args_node);
        
        size_t expected_count = 0;
        if (func_args) {
            expected_count = func_args->get_args().size();
        }
        
        size_t actual_count = 0;
        if (call_args) {
            actual_count = call_args->get_args().size();
        }
        
        if (expected_count != actual_count) {
            set_error(fmt::format("Call to function '{}' has wrong number of arguments", func_name));
            return shared_from_this();
        }
        
        if (func_args && call_args && expected_count > 0) {
            for (size_t i = 0; i < expected_count; i++) {
                auto farg = dynamic_cast<const FArg*>(func_args->get_args()[i].get());
                auto call_arg = call_args->get_args()[i];
                
                if (farg && call_arg) {
                    auto expected_type_id = dynamic_cast<const Id*>(farg->get_type().get());
                    auto actual_type = call_arg->get_stmt_type();
                    
                    if (expected_type_id && actual_type) {
                        auto actual_type_id = dynamic_cast<const Id*>(actual_type.get());
                        
                        if (actual_type_id) {
                            auto expected_type_name = expected_type_id->get_name();
                            auto actual_type_name = actual_type_id->get_name();
                            
                            if (expected_type_name != actual_type_name) {
                                bool is_module_call = func_name.find('.') != std::string::npos;
                                bool is_builtin_type = (actual_type_name == "Integer64" || actual_type_name == "String" || actual_type_name == "Boolean");
                                
                                if (!(is_module_call && is_builtin_type)) {
                                    set_error(fmt::format("Argument {} in call to function '{}' has type '{}'"
                                                        " which does not match definition type '{}'",
                                                        i + 1, func_name, actual_type_name, expected_type_name));
                                    return shared_from_this();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
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

Node::Ptr Import::add_to_symtab_forward(SymbolTable &st) {
    auto path_id = dynamic_cast<Id*>(m_path.get());
    if (!path_id) {
        set_error("Import path must be an identifier");
        return shared_from_this();
    }
    
    auto module_name = path_id->get_name();
    auto file_name = module_name + ".ki";
    
    std::ifstream file(file_name);
    if (!file.is_open()) {
        file_name = "../" + file_name;
        file.open(file_name);
    }
    if (!file.is_open()) {
        set_error(fmt::format("Cannot open module file '{}'", file_name));
        return shared_from_this();
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    auto compiler = Compiler::current();
    if (!compiler) {
        set_error("No compiler instance available");
        return shared_from_this();
    }
    
    auto module_root = compiler->compile_module(content);
    if (!module_root) {
        set_error(fmt::format("Failed to parse module '{}'", module_name));
        return shared_from_this();
    }
    
    auto module = dynamic_cast<Module*>(module_root.get());
    if (!module) {
        set_error(fmt::format("Module '{}' is not a valid module", module_name));
        return shared_from_this();
    }
    
    auto err = module->compute_stmt_type(st);
    if (err) {
        set_error(fmt::format("Error in module '{}': {}", module_name, err->get_error()));
        return shared_from_this();
    }
    
    st.add_symbol(module_name, module_root);
    
    return nullptr;
}
 
}