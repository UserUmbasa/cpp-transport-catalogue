#include "json_builder.h"

namespace json
{
    
    //---------------------BaseContext---------------------

    BaseContext::BaseContext(Builder& builder) : builder_(builder) {}

    KeyContext BaseContext::Key(std::string key) { return builder_.Key(key); }
    Builder& BaseContext::Value(Node value) { return builder_.Value(value); }

    DictionaryContext BaseContext::StartDict() { return DictionaryContext(builder_.StartDict()); }
    Builder& BaseContext::EndDict() { return builder_.EndDict(); }

    ArrayContext BaseContext::StartArray() { return ArrayContext(builder_.StartArray()); }
    Builder& BaseContext::EndArray() { return builder_.EndArray(); }

    //---------------------KeyContext-----------------------

    KeyContext::KeyContext(Builder& builder) : BaseContext(builder) {}

    DictionaryContext  KeyContext::Value(Node value) { return BaseContext::Value(std::move(value)); }

    //---------------------DictionaryContext----------------

    DictionaryContext::DictionaryContext(Builder& builder) : BaseContext(builder) {}

    //---------------------ArrayContext---------------------

    ArrayContext::ArrayContext(Builder& builder) : BaseContext(builder) {}
    ArrayContext ArrayContext::Value(Node value) { return BaseContext::Value(move(value)); }

    //---------------------Builder--------------------------

    Builder& Builder::Value(Node value_)
    {
        add_node(value_);
        return *this;
    }

    DictionaryContext Builder::StartDict()
    {
        Dict dict;                          
        Node* help_ptr = new Node(dict); 
        std::unique_ptr<Node>help_uni_ptr(help_ptr); 
        nodes_stack_.emplace_back(std::move(help_uni_ptr));
        //nodes_stack_.emplace_back(std::move(std::make_unique<Node>(Dict()))); краткая запись
        return DictionaryContext(*this);
    }

    KeyContext Builder::Key(std::string key_)
    {
        if (nodes_stack_.empty()) {
            throw std::logic_error("unable to create key");
        }
        Node* help_ptr = new Node(key_);             
        std::unique_ptr<Node>help_uni_ptr(help_ptr);
        if (nodes_stack_.back()->IsMap()) {
            nodes_stack_.emplace_back(std::move(help_uni_ptr));
        }
        return KeyContext(*this);
    }

    ArrayContext Builder::StartArray()
    {
        Array arr;
        Node* help_ptr = new Node(arr);             
        std::unique_ptr<Node>help_uni_ptr(help_ptr);  
        nodes_stack_.emplace_back(std::move(help_uni_ptr));
        return ArrayContext(*this);
    }

    Builder& Builder::EndArray()
    {
        if (nodes_stack_.empty()) {
            throw std::logic_error("unable to close without opening");
        }

        Node node = *nodes_stack_.back();

        if (!node.IsArray()) {
            throw std::logic_error("object isn't array");
        }

        nodes_stack_.pop_back();
        add_node(node);

        return *this;
    }

    Builder& Builder::EndDict()
    {
        if (nodes_stack_.empty()) {
            throw std::logic_error("unable to close as without opening");
        }
        Node node = *nodes_stack_.back();
        if (!node.IsMap()) {
            throw std::logic_error("object isn't dictionary");
        }
        nodes_stack_.pop_back();
        add_node(node);
        return *this;
    }

    void Builder::add_node(Node node)
    {
        if (nodes_stack_.empty())
        {
            if (!root_.IsNull()) {
                throw std::logic_error("root has been added");
            }
            root_ = node;
            return;
        }
        else {
            if (!nodes_stack_.back()->IsArray() && !nodes_stack_.back()->IsString()) {
                throw std::logic_error("unable to create node");
            }
            if (nodes_stack_.back()->IsArray())
            {
                Array arr = nodes_stack_.back()->AsArray();
                arr.emplace_back(node);
                nodes_stack_.pop_back();
                Node* help_ptr = new Node(arr);             
                std::unique_ptr<Node>help_uni_ptr(help_ptr); 
                nodes_stack_.emplace_back(std::move(help_uni_ptr));
                return;
            }
            if (nodes_stack_.back()->IsString())
            {
                std::string str = nodes_stack_.back()->AsString();
                nodes_stack_.pop_back();
                if (nodes_stack_.back()->IsMap())
                {
                    Dict dictionary = nodes_stack_.back()->AsMap();
                    dictionary.emplace(str, node);
                    nodes_stack_.pop_back();
                    Node* help_ptr = new Node(dictionary);       
                    std::unique_ptr<Node>help_uni_ptr(help_ptr); 
                    nodes_stack_.emplace_back(std::move(help_uni_ptr));
                }
                return;
            }
        }
    }

    Node Builder::Build()
    {
        if (root_.IsNull()) {
            throw std::logic_error("empty json");
        }

        if (!nodes_stack_.empty()) {
            throw std::logic_error("invalid json");
        }
        return root_;
    }

}
