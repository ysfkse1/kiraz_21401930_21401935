#include "Module.h"
#include "Literal.h"
#include <kiraz/Compiler.h>

namespace ast {

Node::Ptr Module::compute_stmt_type(SymbolTable &st) {
    m_cur_symtab = st.get_cur_symtab();
    
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

Node::SymTabEntry Module::get_subsymbol(Node::Ptr subsymbol) const {
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

const SymbolTable *Module::get_subsymbol_all() const {
    return nullptr;
}

Node::Ptr Module::gen_wat(WasmContext &ctx) {
    ctx.body() << "(module\n";
    ctx.body() << "(type $print_s (func (param i32 i32 i32)))\n";
    ctx.body() << "(type $print_i (func (param i32 i64)))\n";
    ctx.body() << "(type $print_b (func (param i32 i32)))\n";
    ctx.body() << "(import \"io\" \"memory\" (memory 1))\n";
    ctx.body() << "(import \"io\" \"print_s\" (func $print_s (type $print_s)))\n";
    ctx.body() << "(import \"io\" \"print_i\" (func $print_i (type $print_i)))\n";
    ctx.body() << "(import \"io\" \"print_b\" (func $print_b (type $print_b)))\n";
    
    for (auto &stmt : m_stmts) {
        auto err = stmt->gen_wat(ctx);
        if (err) return err;
    }
    
    ctx.body() << ")\n";
    return nullptr;
}

}