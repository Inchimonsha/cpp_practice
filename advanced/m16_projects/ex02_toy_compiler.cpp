// m16_projects/ex02_toy_compiler — lexer -> parser -> bytecode VM.
// Yazyk: chisla, peremennye, + - * /, skobki, prisvaivanie.
// Determinizovano. Standart: C++23. Zapusk: ./run.ps1 m16_projects/ex02_toy_compiler
#include <cctype>
#include <cstddef>
#include <iostream>
#include <print>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace {

struct Token {
    enum class Type {
        Number,
        Identifier,
        Plus,
        Minus,
        Star,
        Slash,
        LeftParen,
        RightParen,
        Equals,
        Newline,
        End
    };
    Type type = Type::End;
    double number = 0;
    std::string text;
};

bool is_digit(char symbol) { return std::isdigit(static_cast<unsigned char>(symbol)) != 0; }
bool is_alpha(char symbol) { return std::isalpha(static_cast<unsigned char>(symbol)) != 0; }

std::vector<Token> lex(std::string_view source) {
    std::vector<Token> tokens;
    std::size_t position = 0;

    while (position < source.size()) {
        const char symbol = source[position];
        if (symbol == '\n') {
            tokens.push_back(Token{Token::Type::Newline, 0, {}});
            ++position;
            continue;
        }
        if (std::isspace(static_cast<unsigned char>(symbol)) != 0) {
            ++position;
            continue;
        }
        if (is_digit(symbol) || symbol == '.') {
            const std::size_t start = position;
            while (position < source.size() &&
                   (is_digit(source[position]) || source[position] == '.')) {
                ++position;
            }
            Token token;
            token.type = Token::Type::Number;
            token.number = std::stod(std::string(source.substr(start, position - start)));
            tokens.push_back(std::move(token));
            continue;
        }
        if (is_alpha(symbol)) {
            const std::size_t start = position;
            while (position < source.size() &&
                   std::isalnum(static_cast<unsigned char>(source[position])) != 0) {
                ++position;
            }
            Token token;
            token.type = Token::Type::Identifier;
            token.text = std::string(source.substr(start, position - start));
            tokens.push_back(std::move(token));
            continue;
        }
        ++position;
        Token token;
        switch (symbol) {
            case '+': token.type = Token::Type::Plus; break;
            case '-': token.type = Token::Type::Minus; break;
            case '*': token.type = Token::Type::Star; break;
            case '/': token.type = Token::Type::Slash; break;
            case '(': token.type = Token::Type::LeftParen; break;
            case ')': token.type = Token::Type::RightParen; break;
            case '=': token.type = Token::Type::Equals; break;
            default: continue;
        }
        tokens.push_back(std::move(token));
    }
    tokens.push_back(Token{});
    return tokens;
}

enum class Opcode { PushNumber, LoadVar, StoreVar, Add, Sub, Mul, Div, Neg, Print };

struct Instruction {
    Opcode opcode = Opcode::PushNumber;
    double number = 0;
    std::string name;
};

class Parser {
public:
    explicit Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {}

    std::vector<Instruction> parse() {
        while (peek().type != Token::Type::End) {
            if (peek().type == Token::Type::Newline) {
                ++index_;
                continue;
            }
            statement();
        }
        return code_;
    }

private:
    const Token& peek(std::size_t offset = 0) const {
        const std::size_t index = index_ + offset;
        return tokens_[index < tokens_.size() ? index : tokens_.size() - 1];
    }

    void emit(Opcode opcode, double number = 0, std::string name = {}) {
        code_.push_back(Instruction{opcode, number, std::move(name)});
    }

    void statement() {
        if (peek().type == Token::Type::Identifier &&
            peek(1).type == Token::Type::Equals) {
            const std::string name = tokens_[index_].text;
            index_ += 2;
            expression();
            code_.push_back({Opcode::StoreVar, 0, name});
            return;
        }
        expression();
        code_.push_back({Opcode::Print, 0, {}});
    }

    void expression() {
        term();
        while (peek().type == Token::Type::Plus || peek().type == Token::Type::Minus) {
            const Token::Type op = peek().type;
            ++index_;
            term();
            code_.push_back({op == Token::Type::Plus ? Opcode::Add : Opcode::Sub, 0, {}});
        }
    }

    void term() {
        factor();
        while (peek().type == Token::Type::Star || peek().type == Token::Type::Slash) {
            const Token::Type op = peek().type;
            ++index_;
            factor();
            code_.push_back({op == Token::Type::Star ? Opcode::Mul : Opcode::Div, 0, {}});
        }
    }

    void factor() {
        const Token& token = peek();
        if (token.type == Token::Type::Number) {
            code_.push_back({Opcode::PushNumber, token.number, {}});
            ++index_;
            return;
        }
        if (token.type == Token::Type::Identifier) {
            code_.push_back({Opcode::LoadVar, 0, token.text});
            ++index_;
            return;
        }
        if (token.type == Token::Type::Minus) {
            ++index_;
            factor();
            code_.push_back({Opcode::Neg, 0, {}});
            return;
        }
        if (token.type == Token::Type::LeftParen) {
            ++index_;
            expression();
            ++index_;
            return;
        }
    }

    std::vector<Token> tokens_;
    std::size_t index_ = 0;
    std::vector<Instruction> code_;
};

std::vector<double> run(const std::vector<Instruction>& code) {
    std::vector<double> stack;
    std::unordered_map<std::string, double> variables;
    std::vector<double> output;
    for (const Instruction& instruction : code) {
        switch (instruction.opcode) {
            case Opcode::PushNumber:
                stack.push_back(instruction.number);
                break;
            case Opcode::LoadVar:
                stack.push_back(variables[instruction.name]);
                break;
            case Opcode::StoreVar:
                variables[instruction.name] = stack.back();
                stack.pop_back();
                break;
            case Opcode::Add:
            case Opcode::Sub:
            case Opcode::Mul:
            case Opcode::Div: {
                const double right = stack.back();
                stack.pop_back();
                const double left = stack.back();
                stack.pop_back();
                double result = 0;
                if (instruction.opcode == Opcode::Add) result = left + right;
                if (instruction.opcode == Opcode::Sub) result = left - right;
                if (instruction.opcode == Opcode::Mul) result = left * right;
                if (instruction.opcode == Opcode::Div) result = left / right;
                stack.push_back(result);
                break;
            }
            case Opcode::Neg:
                stack.back() = -stack.back();
                break;
            case Opcode::Print:
                output.push_back(stack.back());
                stack.pop_back();
                break;
        }
    }
    return output;
}

}  // namespace

int main() {
    const std::string program =
        "x = 2\n"
        "y = 3\n"
        "x + y * 4\n"
        "(x + y) * 4\n"
        "10 / 2 - 3\n"
        "-x + 20\n";

    Parser parser(lex(program));
    const std::vector<Instruction> code = parser.parse();
    const std::vector<double> results = run(code);

    std::println("bytecode instructions = {}", code.size());
    for (std::size_t i = 0; i < results.size(); ++i) {
        std::println("result[{}] = {}", i, results[i]);
    }

    const bool ok = results.size() == 4 && results[0] == 14 && results[1] == 20 &&
                    results[2] == 2 && results[3] == 18;
    std::cout << "toy compiler done\n";
    return ok ? 0 : 1;
}
