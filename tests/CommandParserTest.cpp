#include "../src/command/CommandParser.hpp"
#include <iostream>
#include <cassert>

void run_tests() {
    // Passing Cases

    // Test 1: Valid SET command
    auto cmd1 = CommandParser::parse("SET mykey myvalue");
    assert(cmd1.type == CommandType::SET);
    assert(cmd1.key == "mykey");
    assert(cmd1.value == "myvalue");

    // Test 2: Valid GET command
    auto cmd2 = CommandParser::parse("GET anotherkey");
    assert(cmd2.type == CommandType::GET);
    assert(cmd2.key == "anotherkey");
    assert(cmd2.value == "");

    // Test 3: Valid DEL command
    auto cmd3 = CommandParser::parse("DEL todelete");
    assert(cmd3.type == CommandType::DEL);
    assert(cmd3.key == "todelete");
    assert(cmd3.value == "");

    // Test 4: Valid EXISTS command
    auto cmd4 = CommandParser::parse("EXISTS checkthis");
    assert(cmd4.type == CommandType::EXISTS);
    assert(cmd4.key == "checkthis");
    assert(cmd4.value == "");

    // Failing Cases

    // Test 5: SET missing value (should fail)
    auto cmd5 = CommandParser::parse("SET onlykey");
    assert(cmd5.type == CommandType::INVALID);

    // Test 6: Completely unknown command
    auto cmd6 = CommandParser::parse("HELLO world");
    assert(cmd6.type == CommandType::INVALID);

    // Test 7: Empty input
    auto cmd7 = CommandParser::parse("");
    assert(cmd7.type == CommandType::INVALID);

    // Test 8: Wrong GET usage (too many parameters)
    auto cmd8 = CommandParser::parse("GET key extra");
    assert(cmd8.type == CommandType::INVALID);

    // Test 9: Wrong DEL usage (too many parameters)
    auto cmd9 = CommandParser::parse("DEL key anotherextra");
    assert(cmd9.type == CommandType::INVALID);

    std::cout << "All CommandParser tests passed" << std::endl;
}

int main() {
    run_tests();
    return 0;
}
