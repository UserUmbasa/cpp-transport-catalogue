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
        //создадим указатель на вариант со значением пустого словаря (using Dict = std::map<std::string, Node>)
        Dict dict;                          //создаем мап
        Node* help_ptr = new Node(dict);            //сырой указатель на выделенную ! память
        std::unique_ptr<Node>help_uni_ptr(help_ptr); //умный указатель 
        //добавим его в стек указателей (std::move перместить,передать права)
        nodes_stack_.emplace_back(std::move(help_uni_ptr));
        //nodes_stack_.emplace_back(std::move(std::make_unique<Node>(Dict()))); краткая запись
        return DictionaryContext(*this);
    }

    KeyContext Builder::Key(std::string key_)
    {
        if (nodes_stack_.empty()) {
            throw std::logic_error("unable to create key");
        }
        Node* help_ptr = new Node(key_);             //сырой указатель на выделенную ! память
        std::unique_ptr<Node>help_uni_ptr(help_ptr); //умный указатель 
        //StartDict должен быть первым вызовом в цепочке (убеждаемся ,что вариант содержит словарь Dict)
        //добавляем в стек ключ
        if (nodes_stack_.back()->IsDict()) {
            //(std::move перместить, передать права)
            nodes_stack_.emplace_back(std::move(help_uni_ptr));
        }
        return KeyContext(*this);
    }

    ArrayContext Builder::StartArray()
    {
        //добавляем в стек вариант с массивом Array (vector<Node>)
        Array arr;
        Node* help_ptr = new Node(arr);             //сырой указатель на выделенную ! память
        std::unique_ptr<Node>help_uni_ptr(help_ptr); //умный указатель 
        //(std::move перместить, передать права)
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
        //присвоим переменной адрес последнего элемента стека StartDict
        //там всегда по идее висит StartDict
        Node node = *nodes_stack_.back();

        if (!node.IsDict()) {
            throw std::logic_error("object isn't dictionary");
        }
        nodes_stack_.pop_back();
        add_node(node);

        return *this;
    }

    void Builder::add_node(Node node)
    {
        //если стек (вектор) пустой ,то присвоим конструируемому объекту адрес ноды
        if (nodes_stack_.empty())
        {
            if (!root_.IsNull()) {
                throw std::logic_error("root has been added");
            }
            root_ = node;
            return;
        }
        //если стек (вектор) не пустой , то посмотрим последний элемент!!! (вариант) на наличие массива или строки
        else {
            //не содержит Array и не содержит String
            if (!nodes_stack_.back()->IsArray() && !nodes_stack_.back()->IsString()) {

                throw std::logic_error("unable to create node");
            }
            //если вариант массив Array std::vector<Node>;
            if (nodes_stack_.back()->IsArray())
            {
                //создадим массив и присвоим ему значение другого массива из последней ноды вектора методом AsArray()
                Array arr = nodes_stack_.back()->AsArray();
                //добавим новое значение в массив
                arr.emplace_back(node);
                //очистим стек
                nodes_stack_.pop_back();
                //добавим новый массив в стек
                Node* help_ptr = new Node(arr);             //сырой указатель на выделенную ! память
                std::unique_ptr<Node>help_uni_ptr(help_ptr); //умный указатель 
                nodes_stack_.emplace_back(std::move(help_uni_ptr));

                return;
            }
            //если вариант строка (ключ) ,то добавим в мап (Dict) значение Value (ноду - вариант) ;
            if (nodes_stack_.back()->IsString())
            {
                //вытащим ключ из стека (вариант) и удалим его стека (буфера)
                std::string str = nodes_stack_.back()->AsString();

                nodes_stack_.pop_back();
                //убедимся ? что в стеке остался вариант с мапом
                if (nodes_stack_.back()->IsDict())
                {
                    //переменная (может быть уже не пустым) мап 
                    Dict dictionary = nodes_stack_.back()->AsDict();
                    //добавим в него новые данные кей и фалуе
                    dictionary.emplace(str, node);
                    //удалим из стека последний элемент
                    nodes_stack_.pop_back();
                    //добавим в стек новый словарь с новыми значениями
                    Node* help_ptr = new Node(dictionary);       //сырой указатель на выделенную ! память
                    std::unique_ptr<Node>help_uni_ptr(help_ptr); //умный указатель 
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
