#include "../classes/MyFrontendAction.hpp"
#include <gtest/gtest.h>
#include <string>

using namespace std;

// Code should run and return correct structure
TEST(Tests, shouldRun) {
    testing::internal::CaptureStdout();

    ofstream outFile("mock.dot");

    string code = 
    "using namespace std;"
    "int add(int a, int b) {"
        "return a + b;"
    "}";

    string expectedOutput = 
    "  Block1 -> Block0;\n"
    "  Block2 -> Block1;\n";
    
    clang::tooling::runToolOnCode(make_unique<MyFrontendAction>(code, outFile), code);
    string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, expectedOutput);
}

TEST(Tests, shouldntRun) {
    testing::internal::CaptureStderr();

    ofstream outFile("mock.dot");

    //python code
    string code = 
    "def add_numbers(a, b):"
        "return a + b";

    string expectedOutputSubstr = "error: unknown type name 'def'";
    
    clang::tooling::runToolOnCode(make_unique<MyFrontendAction>(code, outFile), code);
    string output = testing::internal::GetCapturedStderr();
    EXPECT_NE(output.find(expectedOutputSubstr), std::string::npos);
}
