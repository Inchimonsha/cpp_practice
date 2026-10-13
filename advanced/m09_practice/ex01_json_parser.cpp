// m09_practice/ex01_json_parser — JSON parser (rekursivnyj spusk).
// Standart: C++23. Zapusk: ./run.ps1 m09_practice/ex01_json_parser
#include <cstddef>
#include <expected>
#include <format>
#include <iostream>
#include <print>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

namespace json {

struct Value;
struct Member;

struct Value {
    std::variant<std::nullptr_t, bool, double, std::string,
                 std::vector<Value>, std::vector<Member>> data;
};

struct Member {
    std::string key;
    Value value;
};

struct ParseError {
    std::size_t pos;
    std::string message;
};

class Parser {
public:
    explicit Parser(std::string_view text) : text_(text) {}

    std::expected<Value, ParseError> parse() {
        skip_ws();
        auto value = parse_value();
        if (!value) { return value; }
        skip_ws();
        if (pos_ != text_.size()) { return fail("trailing characters"); }
        return value;
    }

private:
    std::string_view text_;
    std::size_t pos_ = 0;

    std::unexpected<ParseError> fail(std::string message) const {
        return std::unexpected(ParseError{pos_, std::move(message)});
    }
    void skip_ws() {
        while (pos_ < text_.size() &&
               (text_[pos_] == ' ' || text_[pos_] == '\n' ||
                text_[pos_] == '\t' || text_[pos_] == '\r')) {
            ++pos_;
        }
    }
    bool consume(char ch) {
        if (pos_ < text_.size() && text_[pos_] == ch) { ++pos_; return true; }
        return false;
    }

    std::expected<Value, ParseError> parse_value() {
        if (pos_ >= text_.size()) { return fail("unexpected end of input"); }
        const char ch = text_[pos_];
        switch (ch) {
            case '{': return parse_object();
            case '[': return parse_array();
            case '"': {
                auto str = parse_string();
                if (!str) { return std::unexpected(str.error()); }
                return Value{std::move(*str)};
            }
            case 't': return parse_literal("true", Value{true});
            case 'f': return parse_literal("false", Value{false});
            case 'n': return parse_literal("null", Value{nullptr});
            default: return parse_number();
        }
    }

    std::expected<Value, ParseError> parse_literal(std::string_view word, Value value) {
        if (text_.substr(pos_, word.size()) != word) { return fail("bad literal"); }
        pos_ += word.size();
        return value;
    }

    std::expected<Value, ParseError> parse_object() {
        ++pos_;   // '{'
        std::vector<Member> members;
        skip_ws();
        if (consume('}')) { return Value{std::move(members)}; }
        for (;;) {
            skip_ws();
            auto key = parse_string();
            if (!key) { return std::unexpected(key.error()); }
            skip_ws();
            if (!consume(':')) { return fail("expected ':'"); }
            skip_ws();
            auto value = parse_value();
            if (!value) { return value; }
            members.push_back(Member{std::move(*key), std::move(*value)});
            skip_ws();
            if (consume('}')) { break; }
            if (!consume(',')) { return fail("expected ',' or '}'"); }
        }
        return Value{std::move(members)};
    }

    std::expected<Value, ParseError> parse_array() {
        ++pos_;   // '['
        std::vector<Value> items;
        skip_ws();
        if (consume(']')) { return Value{std::move(items)}; }
        for (;;) {
            skip_ws();
            auto value = parse_value();
            if (!value) { return value; }
            items.push_back(std::move(*value));
            skip_ws();
            if (consume(']')) { break; }
            if (!consume(',')) { return fail("expected ',' or ']'"); }
        }
        return Value{std::move(items)};
    }

    std::expected<std::string, ParseError> parse_string() {
        if (!consume('"')) { return fail("expected string"); }
        std::string out;
        while (pos_ < text_.size()) {
            const char ch = text_[pos_++];
            if (ch == '"') { return out; }
            if (ch == '\\') {
                if (pos_ >= text_.size()) { return fail("bad escape"); }
                const char next = text_[pos_++];
                switch (next) {
                    case '"': out.push_back('"'); break;
                    case '\\': out.push_back('\\'); break;
                    case 'n': out.push_back('\n'); break;
                    case 't': out.push_back('\t'); break;
                    default: return fail("unsupported escape");
                }
            } else {
                out.push_back(ch);
            }
        }
        return fail("unterminated string");
    }

    std::expected<Value, ParseError> parse_number() {
        const std::size_t start = pos_;
        if (pos_ < text_.size() && text_[pos_] == '-') { ++pos_; }
        while (pos_ < text_.size() &&
               ((text_[pos_] >= '0' && text_[pos_] <= '9') || text_[pos_] == '.' ||
                text_[pos_] == 'e' || text_[pos_] == 'E' ||
                text_[pos_] == '+' || text_[pos_] == '-')) {
            ++pos_;
        }
        if (pos_ == start || (pos_ == start + 1 && text_[start] == '-')) {
            return fail("invalid number");
        }
        try {
            const std::string slice(text_.substr(start, pos_ - start));
            std::size_t used = 0;
            const double value = std::stod(slice, &used);
            if (used != slice.size()) { return fail("invalid number"); }
            return Value{value};
        } catch (...) {
            return fail("invalid number");
        }
    }
};

std::string serialize(const Value& value) {
    struct Visitor {
        std::string operator()(std::nullptr_t) const { return "null"; }
        std::string operator()(bool flag) const { return flag ? "true" : "false"; }
        std::string operator()(double number) const { return std::format("{}", number); }
        std::string operator()(const std::string& text) const {
            return "\"" + text + "\"";
        }
        std::string operator()(const std::vector<Value>& items) const {
            std::string out = "[";
            for (std::size_t i = 0; i < items.size(); ++i) {
                if (i != 0) { out += ','; }
                out += serialize(items[i]);
            }
            return out + "]";
        }
        std::string operator()(const std::vector<Member>& members) const {
            std::string out = "{";
            for (std::size_t i = 0; i < members.size(); ++i) {
                if (i != 0) { out += ','; }
                out += "\"" + members[i].key + "\":" + serialize(members[i].value);
            }
            return out + "}";
        }
    };
    return std::visit(Visitor{}, value.data);
}

const Value* member(const Value& object, std::string_view key) {
    const auto* members = std::get_if<std::vector<Member>>(&object.data);
    if (members == nullptr) { return nullptr; }
    for (const auto& entry : *members) {
        if (entry.key == key) { return &entry.value; }
    }
    return nullptr;
}

}  // namespace json

int main() {
    const std::string_view doc =
        R"({"name":"engine","version":3,"ready":true,)"
        R"("tags":["cpp","lua"],"config":{"width":1280,"height":720}})";

    json::Parser parser(doc);
    auto root = parser.parse();
    if (!root) {
        std::println("unexpected error at {}: {}", root.error().pos, root.error().message);
        return 1;
    }

    const auto* name = json::member(*root, "name");
    const auto* version = json::member(*root, "version");
    const auto* ready = json::member(*root, "ready");
    const auto* tags = json::member(*root, "tags");
    const auto* config = json::member(*root, "config");
    const auto* width = (config != nullptr) ? json::member(*config, "width") : nullptr;

    std::println("name = {}", std::get<std::string>(name->data));
    std::println("version = {}", std::get<double>(version->data));
    std::println("ready = {}", std::get<bool>(ready->data));
    const auto& tag_items = std::get<std::vector<json::Value>>(tags->data);
    std::println("tags[1] = {}", std::get<std::string>(tag_items[1].data));
    std::println("config.width = {}", std::get<double>(width->data));

    const std::string canonical = json::serialize(*root);
    const std::string_view expected =
        R"({"name":"engine","version":3,"ready":true,"tags":["cpp","lua"],"config":{"width":1280,"height":720}})";
    std::println("serialized = {}", canonical);

    json::Parser bad(R"({"a": 1,})");
    auto broken = bad.parse();
    if (!broken) {
        std::println("expected error at {}: {}", broken.error().pos, broken.error().message);
    }

    std::cout << "json parser done\n";
    return (canonical == expected && !broken) ? 0 : 1;
}
