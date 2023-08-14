#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json {

    class Node;

    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node final : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> 
    {
    public:
        using variant::variant;
        using Value = variant;

        bool IsInt() const;
        bool IsDouble() const;
        bool IsPureDouble() const;
        bool IsBool() const;
        bool IsString() const;
        bool IsNull() const;
        bool IsArray() const;
        bool IsMap() const;

        int AsInt() const;
        bool AsBool() const;
        double AsDouble() const;
        const std::string& AsString() const;
        const Array& AsArray() const;
        const Dict& AsMap() const;

        const Value& GetValue() const {
            return *this;
        };

    private:
        Value value_;
    };


    inline bool operator==(const Node& lhs, const Node& rhs) {
        return lhs.GetValue() == rhs.GetValue();
    }
    inline bool operator!=(const Node& lhs, const Node& rhs) {
        return !(lhs == rhs);
    }

    class Document {
    public:
        explicit Document(Node root);

        const Node& GetRoot() const;

    private:
        Node root_;
    };

    Document Load(std::istream& input);

    inline bool operator==(const Document& lhs, const Document& rhs) {
        return lhs.GetRoot() == rhs.GetRoot();
    }
    inline bool operator!=(const Document& lhs, const Document& rhs) {
        return !(lhs == rhs);
    }

    struct PrintContext {
        std::ostream& out;
        int indent_step = 4;
        int indent = 0;

        void PrintIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        [[nodiscard]] PrintContext Indented() const {
            return { out, indent_step,
                    indent + indent_step };
        }
    };

    void PrintNode(const Node& node, const PrintContext& context);
    void PrintValue(const std::string& value, const PrintContext& context);
    void PrintValue(Dict nodes, const PrintContext& context);
    void PrintValue(const std::nullptr_t&, const PrintContext& context);
    void PrintValue(Array nodes, const PrintContext& context);
    void PrintValue(bool value, const PrintContext& context);
    template <typename Value>
    void PrintValue(const Value& value, const PrintContext& context);

    void Print(const Document& doc, std::ostream& output);

}
