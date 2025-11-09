#include "Statement.h"
#include "Literal.h"
#include <kiraz/Compiler.h>

namespace ast {

Node::Ptr Func::add_to_symtab_forward(SymbolTable &st) {
    st.add_symbol(dynamic_cast<Id*>(m_name.get())->get_name(), shared_from_this());
    return nullptr;
}

Node::Ptr Class::add_to_symtab_forward(SymbolTable &st) {
    st.add_symbol(dynamic_cast<Id*>(m_name.get())->get_name(), shared_from_this());
    return nullptr;
}

Node::Ptr ListNode::add_to_symtab_forward(SymbolTable &st) {
    for (auto &item : m_items) {
        auto err = item->add_to_symtab_forward(st);
        if (err) return err;
    }
    return nullptr;
}

Node::Ptr Let::add_to_symtab_ordered(SymbolTable &st) {
    st.add_symbol(dynamic_cast<Id*>(m_name.get())->get_name(), shared_from_this());
    return nullptr;
}

}