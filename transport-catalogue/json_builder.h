#pragma once
#include "json.h"
#include <stack>
#include <string>
#include<memory>

namespace json
{
    class KeyContext;
    class DictionaryContext;
    class ArrayContext;
    class Builder
    {
    public:
        Builder& Value(Node value_);
        DictionaryContext StartDict();
        KeyContext Key(std::string key_);
        ArrayContext StartArray();
        Builder& EndArray();
        Builder& EndDict();
        Node Build();
    private:
        void add_node(Node node);
        Node root_;
        std::vector<std::unique_ptr<Node>>nodes_stack_;
    };
    //контроль вызова (создание объекта)
    class BaseContext {
    public:
        BaseContext(Builder& builder);

        KeyContext Key(std::string key);
        DictionaryContext StartDict();
        ArrayContext StartArray();

        Builder& Value(Node value);

        Builder& EndDict();

        Builder& EndArray();

    protected:
        Builder& builder_;

    };

    class KeyContext : public BaseContext {
    public:
        KeyContext(Builder& builder);

        KeyContext Key(std::string key) = delete;

        BaseContext EndDict() = delete;
        BaseContext EndArray() = delete;

        DictionaryContext Value(Node value);
    };

    class DictionaryContext : public BaseContext {
    public:
        DictionaryContext(Builder& builder);

        DictionaryContext StartDict() = delete;

        ArrayContext StartArray() = delete;
        Builder& EndArray() = delete;

        Builder& Value(Node value) = delete;
    };

    class ArrayContext : public BaseContext {
    public:
        ArrayContext(Builder& builder);

        KeyContext Key(std::string key) = delete;

        Builder& EndDict() = delete;

        ArrayContext Value(Node value);
    };

}
