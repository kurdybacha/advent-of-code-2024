#include <assert.h>
#include <math.h>

#include <algorithm>
#include <cstdint>
#include <format>
#include <fstream>
#include <functional>
#include <limits>
#include <print>
#include <queue>
#include <ranges>
#include <sstream>

#include "../util/point.h"
#include "../utils.h"

using namespace std;
using namespace utils;

using reg = int64_t;
using program = vector<reg>;

template <>
struct std::formatter<program> : formatter<std::string> {
    auto format(const program &p, format_context &ctx) const {
        string str;
        for (auto &&v : p) {
            if (!str.empty()) str.append(",");
            str.append(to_string(v));
        }
        return format_to(ctx.out(), "{}", str);
    }
};

struct cpu {
    reg A = 0;
    reg B = 0;
    reg C = 0;
    reg IP = 0;

    bool output_used = false;
    program output;

    reg opcode_to_reg(reg opcode) {
        switch (opcode) {
            case 0:
            case 1:
            case 2:
            case 3:
                return opcode;
            case 4:
                return A;
            case 5:
                return B;
            case 6:
                return C;
            default:
                assert(false);
        }
    }

    function<void(reg)> opcode_to_instruction(reg opcode) {
        switch (opcode) {
            case 0:
                return [this](reg opcode) { adv(opcode); };
            case 1:
                return [this](reg opcode) { bxl(opcode); };
            case 2:
                return [this](reg opcode) { bst(opcode); };
            case 3:
                return [this](reg opcode) { jnz(opcode); };
            case 4:
                return [this](reg opcode) { bxc(opcode); };
            case 5:
                return [this](reg opcode) { out(opcode); };
            case 6:
                return [this](reg opcode) { bdv(opcode); };
            case 7:
                return [this](reg opcode) { cdv(opcode); };
            default:
                assert(false);
        }
    }

    void adv(reg opcode) {
        reg reg = opcode_to_reg(opcode);
        A = (A >> reg);
        IP += 2;
    }

    void bxl(reg opcode) {
        B = (B ^ opcode);
        IP += 2;
    }

    void bst(reg opcode) {
        reg reg = opcode_to_reg(opcode);
        B = (reg % 8);
        IP += 2;
    }
    void jnz(reg opcode) {
        if (A == 0) {
            IP += 2;
            return;
        }
        IP = opcode;
    }

    void bxc(reg opcode) {
        B = (B ^ C);
        IP += 2;
    }

    void out(reg opcode) {
        reg r = opcode_to_reg(opcode) % 8;
        output.emplace_back(r);
        IP += 2;
    }

    void bdv(reg opcode) {
        reg reg = opcode_to_reg(opcode);
        B = (A >> reg);
        IP += 2;
    }

    void cdv(reg opcode) {
        reg reg = opcode_to_reg(opcode);
        C = (A >> reg);
        IP += 2;
    }

    void run(const program &program) {
        output.clear();
        while ((program.size() - IP) > 1) {
            reg opcode = program[IP];
            reg operand = program[IP + 1];
            opcode_to_instruction(opcode)(operand);
        }
    }

    reg find_A(const program &program) {
        reg a = 1;
        while (true) {
            A = a;
            IP = 0;
            run(program);
            if (program == output) break;
            bool ends_with = true;
            int diff = program.size() - output.size();
            // check if program ends with output
            for (int i = output.size() - 1; i >= 0; --i) {
                if (output[i] != program[diff + i]) {
                    ends_with = false;
                }
            }
            if (ends_with)
                a = a * 8;
            else
                a++;
        }

        return a;
    }
};

tuple<cpu, program> parse_file(const string &file_name) {
    ifstream file{file_name};
    cpu cpu;
    string line;
    getline(file, line);
    cpu.A = stoi(line.substr(line.find_last_of(' ')));
    getline(file, line);
    cpu.B = stoi(line.substr(line.find_last_of(' ')));
    getline(file, line);
    cpu.C = stoi(line.substr(line.find_last_of(' ')));
    getline(file, line);
    getline(file, line);
    program program = line.substr(line.find_first_of(' ')) | views::split(',') |
                      views::transform([](auto v) {
                          return stoi(string{v.begin(), v.end()});
                      }) |
                      ranges::to<vector<reg>>();
    return {cpu, program};
}

int main(int argc, char **argv) {
    auto [cpu, program] = parse_file(argv[1]);
    timeit([&]() {
        cpu.run(program);
        println("Part 1: {}", cpu.output);
        return 0;
    });
    timeit([&]() { return cpu.find_A(program); });
    return 0;
}
