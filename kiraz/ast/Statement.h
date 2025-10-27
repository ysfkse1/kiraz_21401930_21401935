#ifndef KIRAZ_AST_STATEMENT_H
#define KIRAZ_AST_STATEMENT_H

#include <kiraz/Node.h>

namespace ast {

class Let : public Node {
public:
    Let(Node::Ptr name, Node::Ptr type, Node::Ptr init) 
        : m_name(name), m_type(type), m_init(init) {}
    
    std::string as_string() const override {
        std::string result = "Let(";
        if (m_name) {
            result += fmt::format("n={}", m_name->as_string());
        }
        if (m_type) {
            result += fmt::format(", t={}", m_type->as_string());
        }
        if (m_init) {
            result += fmt::format(", i={}", m_init->as_string());
        }
        result += ")";
        return result;
    }
    
private:
    Node::Ptr m_name;
    Node::Ptr m_type;
    Node::Ptr m_init;
};

class Assign : public Node {
public:
    Assign(Node::Ptr left, Node::Ptr right) 
        : m_left(left), m_right(right) {}
    
    std::string as_string() const override {
        return fmt::format("Assign(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
    
private:
    Node::Ptr m_left;
    Node::Ptr m_right;
};

class FArg : public Node {
public:
    FArg(Node::Ptr name, Node::Ptr type) 
        : m_name(name), m_type(type) {}
    
    std::string as_string() const override {
        return fmt::format("FArg(n={}, t={})", m_name->as_string(), m_type->as_string());
    }
    
private:
    Node::Ptr m_name;
    Node::Ptr m_type;
};

class FuncArgs : public Node {
public:
    FuncArgs(std::vector<Node::Ptr> args) 
        : m_args(args) {}
    
    std::string as_string() const override {
        std::string result = "FuncArgs([";
        for (size_t i = 0; i < m_args.size(); i++) {
            if (i > 0) result += ", ";
            result += m_args[i]->as_string();
        }
        result += "])";
        return result;
    }
    
private:
    std::vector<Node::Ptr> m_args;
};

class Func : public Node {
public:
    Func(Node::Ptr name, Node::Ptr args, Node::Ptr ret_type, std::vector<Node::Ptr> stmts) 
        : m_name(name), m_args(args), m_ret_type(ret_type), m_stmts(stmts) {}
    
    std::string as_string() const override {
        std::string result = "Func(";
        result += fmt::format("n={}, a=", m_name->as_string());
        
        // If args is null, print empty array
        if (!m_args) {
            result += "[]";
        } else {
            result += m_args->as_string();
        }
        
        result += fmt::format(", r={}, s=[", m_ret_type->as_string());
        
        for (size_t i = 0; i < m_stmts.size(); i++) {
            if (i > 0) result += ", ";
            result += m_stmts[i]->as_string();
        }
        result += "])";
        return result;
    }
    
private:
    Node::Ptr m_name;
    Node::Ptr m_args;
    Node::Ptr m_ret_type;
    std::vector<Node::Ptr> m_stmts;
};

class If : public Node {
public:
    If(Node::Ptr condition, std::vector<Node::Ptr> then_stmts, Node::Ptr else_part) 
        : m_condition(condition), m_then_stmts(then_stmts), m_else_part(else_part) {}
    
    std::string as_string() const override {
        std::string result = "If(?=";
        result += m_condition->as_string();
        result += ", then=[";
        
        for (size_t i = 0; i < m_then_stmts.size(); i++) {
            if (i > 0) result += ", ";
            result += m_then_stmts[i]->as_string();
        }
        
        result += "], else=";
        
        if (m_else_part) {
            result += m_else_part->as_string();
        } else {
            result += "[]";
        }
        
        result += ")";
        return result;
    }
    
private:
    Node::Ptr m_condition;
    std::vector<Node::Ptr> m_then_stmts;
    Node::Ptr m_else_part; 
};

class ElseBlock : public Node {
public:
    ElseBlock(std::vector<Node::Ptr> stmts) 
        : m_stmts(stmts) {}
    
    std::string as_string() const override {
        std::string result = "[";
        for (size_t i = 0; i < m_stmts.size(); i++) {
            if (i > 0) result += ", ";
            result += m_stmts[i]->as_string();
        }
        result += "]";
        return result;
    }
    
private:
    std::vector<Node::Ptr> m_stmts;
};

class While : public Node {
public:
    While(Node::Ptr condition, std::vector<Node::Ptr> stmts) 
        : m_condition(condition), m_stmts(stmts) {}
    
    std::string as_string() const override {
        std::string result = "While(?=";
        result += m_condition->as_string();
        result += ", repeat=[";
        
        for (size_t i = 0; i < m_stmts.size(); i++) {
            if (i > 0) result += ", ";
            result += m_stmts[i]->as_string();
        }
        
        result += "])";
        return result;
    }
    
private:
    Node::Ptr m_condition;
    std::vector<Node::Ptr> m_stmts;
};


class ListNode : public Node {
public:
    ListNode(std::vector<Node::Ptr> items) : m_items(items) {}
    
    std::string as_string() const override {
        return "[ListNode]"; 
    }
    
    std::vector<Node::Ptr> get_items() const { return m_items; }
    
private:
    std::vector<Node::Ptr> m_items;
};

}

#endif