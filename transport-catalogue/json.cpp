#include "json.h"

using namespace std;
namespace json {
    namespace {

        Node LoadNode(istream& input);


        std::string LoadLiteral(std::istream& input) {
            std::string str;

            while (std::isalpha(input.peek())) {
                str.push_back(static_cast<char>(input.get()));
            }
            return str;
        }

        Node LoadArray(std::istream& input) {
            std::vector<Node> array;

            for (char ch; input >> ch && ch != ']';) {
                if (ch != ',') {
                    input.putback(ch);
                }

                array.push_back(LoadNode(input));
            }

            if (!input) {
                throw ParsingError("-parse array"s);
            }

            return Node(array);
        }

        //nullptr
        Node LoadNull(std::istream& input) {
            if (auto nul = LoadLiteral(input); nul == "null"sv) {
                return Node(nullptr);
            }
            else {
                throw ParsingError("unable to parse '"s + nul + "' as null"s);
            }
        }

        Node LoadBool(std::istream& input) {
            const auto line = LoadLiteral(input);

            if (line == "true"sv) {
                return Node(true);
            }
            else if (line == "false"sv) {
                return Node(false);
            }
            else {
                throw ParsingError("-parse '"s + line + "' as bool"s);
            }
        }

        Node LoadNumber(std::istream& input) {
            std::string number;

            auto read_char = [&number, &input] {
                number += static_cast<char>(input.get());

                if (!input) {
                    throw ParsingError("-read number"s);
                }
            };

            auto read_digits = [&input, read_char] {

                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                else {
                    while (std::isdigit(input.peek())) {
                        read_char();
                    }
                }
            };

            if (input.peek() == '-') {
                read_char();
            }

            if (input.peek() == '0') {
                read_char();
            }
            else {
                read_digits();
            }

            bool is_int = true;
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();

                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }

                read_digits();
                is_int = false;
            }

            try {
                if (is_int) {
                    try {
                        return Node(std::stoi(number));

                    }
                    catch (...) {}
                }
                return Node(std::stod(number));

            }
            catch (...) {
                throw ParsingError("unable to convert "s + number + " to number"s);
            }
        }

        Node LoadString(std::istream& input) {
            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string str;

            while (true) {
                if (it == end) {
                    throw ParsingError("unable to parse string");
                }

                const char ch = *it;
                if (ch == '"') {
                    ++it;
                    break;

                }
                else if (ch == '\\') {
                    ++it;
                    if (it == end) {
                        throw ParsingError("-parse string");
                    }

                    const char esc_ch = *(it);
                    switch (esc_ch) {
                    case 'n':
                        str.push_back('\n');
                        break;
                    case 't':
                        str.push_back('\t');
                        break;
                    case 'r':
                        str.push_back('\r');
                        break;
                    case '"':
                        str.push_back('"');
                        break;
                    case '\\':
                        str.push_back('\\');
                        break;
                    default:
                        throw ParsingError("invalid"s + esc_ch);
                    }

                }
                else if (ch == '\n' || ch == '\r') {
                    throw ParsingError("-line end"s);
                }
                else {
                    str.push_back(ch);
                }

                ++it;
            }

            return Node(str);
        }

        Node LoadDict(std::istream& input) {
            Dict dictionary;

            for (char ch; input >> ch && ch != '}';) {

                if (ch == '"') {
                    std::string key = LoadString(input).AsString();

                    if (input >> ch && ch == ':') {

                        if (dictionary.find(key) != dictionary.end()) {
                            throw ParsingError("error '"s + key + "'found");
                        }

                        dictionary.emplace(std::move(key), LoadNode(input));

                    }
                    else {
                        throw ParsingError(": error '"s + ch + "' found"s);
                    }

                }
                else if (ch != ',') {
                    throw ParsingError("',' error'"s + ch + "' found"s);
                }
            }

            if (!input) {
                throw ParsingError("-parse dictionary"s);
            }
            else {
                return Node(dictionary);
            }

        }

        Node LoadNode(istream& input) {
            char c;
            input >> c;

            if (c == '[') {
                return LoadArray(input);
            }
            else if (c == '{') {
                return LoadDict(input);
            }
            else if (c == '"') {
                return LoadString(input);
            }
            else if (c == 't' || c == 'f') {
                input.putback(c);
                return LoadBool(input);
            }
            else if (c == 'n') {
                input.putback(c);
                return LoadNull(input);
            }
            else {
                input.putback(c);
                return LoadNumber(input);
            }
        }

    }//end namespace

    //BOOL
    bool Node::IsNull() const {
        return std::holds_alternative<std::nullptr_t>(value_);
    }
    bool Node::IsInt() const {
        return std::holds_alternative<int>(value_);
    }
    bool Node::IsDouble() const {
        return IsPureDouble() || IsInt();
    }
    bool Node::IsPureDouble() const {
        return std::holds_alternative<double>(value_);
    }
    bool Node::IsBool() const {
        return std::holds_alternative<bool>(value_);
    }
    bool Node::IsString() const {
        return std::holds_alternative<std::string>(value_);
    }
    bool Node::IsArray() const {
        return std::holds_alternative<Array>(value_);
    }
    bool Node::IsMap() const {
        return std::holds_alternative<Dict>(value_);
    }
    //BOOL


    //NODE
    Node::Node(Array array) :
        value_(std::move(array)) {
    }
    Node::Node(std::nullptr_t) :
        Node() {
    }
    Node::Node(bool value) :
        value_(value) {
    }
    Node::Node(Dict map) :
        value_(std::move(map)) {
    }
    Node::Node(int value) :
        value_(value) {
    }
    Node::Node(string value) :
        value_(std::move(value)) {
    }
    Node::Node(double value) :
        value_(value) {
    }
    //NODE

    const Array& Node::AsArray() const {
        using namespace std::literals;

        if (!IsArray()) {
            throw std::logic_error(""s);
        }
        else {
            return std::get<Array>(value_);
        }
    }

    const Dict& Node::AsMap() const {
        using namespace std::literals;

        if (!IsMap()) {
            throw std::logic_error(""s);
        }
        else {
            return std::get<Dict>(value_);
        }
    }

    const string& Node::AsString() const {
        using namespace std::literals;

        if (!IsString()) {
            throw std::logic_error(""s);
        }
        else {
            return std::get<std::string>(value_);
        }
    }

    int Node::AsInt() const {
        using namespace std::literals;

        if (!IsInt()) {
            throw std::logic_error(""s);
        }
        else {
            return std::get<int>(value_);
        }
    }

    double Node::AsDouble() const {
        using namespace std::literals;

        if (!IsDouble()) {
            throw std::logic_error(""s);
        }
        else if (IsPureDouble()) {
            return std::get<double>(value_);
        }
        else {
            return AsInt();
        }
    }

    bool Node::AsBool() const {
        using namespace std::literals;

        if (!IsBool()) {
            throw std::logic_error(""s);
        }
        else {
            return std::get<bool>(value_);
        }
    }

    Document::Document(Node root) : root_(std::move(root)) {
    }
    const Node& Document::GetRoot() const {
        return root_;
    }
    Document Load(istream& input) {
        return Document{ LoadNode(input) };
    }


    void PrintString(const std::string& value, std::ostream& out) {
        out.put('"');

        for (const char ch : value) {
            switch (ch) {
            case '\r':
                out << R"(\r)";
                break;
            case '\n':
                out << R"(\n)";
                break;
            case '"':
                out << R"(\")";
                break;
            case '\\':
                out << R"(\\)";
                break;
            default:
                out.put(ch);
                break;
            }
        }

        out.put('"');
    }

    template <typename Value>
    void PrintValue(const Value& value, const PrintContext& context) {
        context.out << value;
    }

    void PrintValue(const std::string& value, const PrintContext& context) {
        PrintString(value, context.out);
    }

    void PrintValue(const std::nullptr_t&, const PrintContext& context) {
        context.out << "null"s;
    }

    void PrintValue(bool value, const PrintContext& context) {
        context.out << std::boolalpha << value;
    }

    void PrintValue(Array nodes, const PrintContext& context) {
        std::ostream& out = context.out;
        out << "[\n"sv;
        bool first = true;
        auto inner_context = context.Indented();

        for (const Node& node : nodes) {
            if (first) {
                first = false;
            }
            else {
                out << ",\n"sv;
            }

            inner_context.PrintIndent();
            PrintNode(node, inner_context);
        }

        out.put('\n');
        context.PrintIndent();
        out.put(']');
    }

    void PrintValue(Dict nodes, const PrintContext& context) {
        std::ostream& out = context.out;
        out << "{\n"sv;
        bool first = true;
        auto inner_context = context.Indented();

        for (const auto& [key, node] : nodes) {
            if (first) {
                first = false;
            }
            else {
                out << ",\n"sv;
            }

            inner_context.PrintIndent();
            PrintString(key, context.out);
            out << ": "sv;
            PrintNode(node, inner_context);
        }

        out.put('\n');
        context.PrintIndent();
        out.put('}');
    }

    void PrintNode(const Node& node, const PrintContext& context) {
        std::visit([&context](const auto& value) {
            PrintValue(value, context);
            }, node.GetValue());
    }

    void Print(const Document& document, std::ostream& output) {
        PrintNode(document.GetRoot(), PrintContext{ output });
    }

}
