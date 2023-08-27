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
        // Value(Node::Value). Задаёт значение, соответствующее ключу при определении словаря, очередной элемент массива или, 
        //если вызвать сразу после конструктора json::Builder, всё содержимое конструируемого JSON-объекта. 
        //Может принимать как простой объект — число или строку — так и целый массив или словарь.
        //Здесь Node::Value — это синоним для базового класса Node, шаблона variant с набором возможных типов - значений
        //в аргументах создается объект типа Node (в конструктор которого передаем строку (создается оъект типа Value)
        Builder& Value(Node value_);

        // Начинает определение сложного значения-словаря. Вызывается в тех же контекстах, что и Value (создается оъект типа Value)
        // Следующим вызовом обязательно должен быть Key или EndDict.
        DictionaryContext StartDict();

        //При определении словаря задаёт строковое значение ключа для очередной пары ключ-значение. Следующий вызов метода обязательно
        //должен задавать соответствующее этому ключу значение с помощью метода Value или начинать его определение с помощью 
        //StartDict или StartArray.
        KeyContext Key(std::string key_);

        // Начинает определение сложного значения-массива. Вызывается в тех же контекстах, что и Value. Следующим вызовом 
        //обязательно должен быть EndArray или любой, задающий новое значение: Value, StartDict или StartArray.
        ArrayContext StartArray();

        // Завершает определение сложного значения-массива. Последним незавершённым вызовом Start* должен быть StartArray.
        Builder& EndArray();

        //EndDict(). Завершает определение сложного значения-словаря. Последним незавершённым вызовом Start* должен быть StartDict.
        Builder& EndDict();

        void add_node(Node node);

        Node Build();


    private:
        //конструируемый объект.
        Node root_;
        //стек указателей на те вершины JSON, которые ещё не построены: то есть текущее описываемое значение и цепочка его родителей. 
        //Он поможет возвращаться в нужный контекст после вызова End-методов.
        // «последним вошел — первым вышел» 
        // чтобы не заморачиваться самому с динамической памятью применим
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
