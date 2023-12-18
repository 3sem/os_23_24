#include <unistd.h>

#include <fstream>

#include "Executor.h"
#include "TokenParser.h"
#include "gtest/gtest.h"

#include <fstream>

#include "TokenParser.h"
#include "gtest/gtest.h"

static const char* ExecutorDataPath = "../../1-shell-emulator/Tests/Data/Executor/";
static const char* TParserDataPath = "../../1-shell-emulator/Tests/Data/TokenParser/";

class ExecutorTestEnvironment {
  ExecutorContext Ctx;
  TokenParser Tp;
  bool IsInit = false;

  FILE* InputFile = NULL;
  FILE* OutputFile = NULL;

  char* Buf;
  size_t Length;

 public:
  ExecutorTestEnvironment() = default;

  bool SetInputFile(const std::string& inputName) {
    FILE* f = fopen(inputName.c_str(), "r");
    if (!f) {
      perror("Failed to open input file");
      return false;
    }

    InputFile = f;
    return true;
  }

  bool SetOutputFile(const std::string& outputName) {
    FILE* f = fopen(outputName.c_str(), "w");
    if (!f) {
      perror("Failed to open output file");
      return false;
    }

    OutputFile = f;
    return true;
  }

  ShellStatus Init(char** env) {
    assert(InputFile);
    assert(OutputFile);
    assert(TokenParserInit(&Tp) == SH_SUCCESS);

    IsInit = true;
    return ExecutorContextInit(&Ctx, env, fileno(InputFile),
                               fileno(OutputFile));
  }

  ShellStatus ExecuteLine(std::string line) {
    ShellStatus status;

    status = ParseTokens(&Tp, &line[0], " ");
    if (status != SH_SUCCESS) return status;

    return Execute(&Ctx, Tp.Tokens);
  }

  const ExecutorContext& Get() const { return Ctx; }

  void Stop() {
    fclose(InputFile);
    fclose(OutputFile);
    if (IsInit) {
      ExecutorDestroy(&Ctx);
    }
  }
};

struct TestResult {
  enum class TestError { Success, Failure, FailedToOpen, FailedToRead } Error;
  ShellStatus Result;
};

class TokenParserTestEnvironment {
  std::string TestData;
  TokenParser Tp;
  const char* Delimeters;

  bool HasRun = false;

 public:
  explicit TokenParserTestEnvironment(const char* delim = " ")
      : Delimeters{delim} {}

  TestResult::TestError LoadString(const std::string& data) {
    TestData = data;
    return TestResult::TestError::Success;
  }

  TestResult Run() {
    ShellStatus status;

    status = TokenParserInit(&Tp);
    if (status != SH_SUCCESS) return {TestResult::TestError::Failure, status};

    status = ParseTokens(&Tp, &TestData[0], Delimeters);
    if (status != SH_SUCCESS) return {TestResult::TestError::Failure, status};

    HasRun = true;
    return {TestResult::TestError::Success, SH_SUCCESS};
  }

  const TokenParser& Get() const {
    assert(HasRun);
    return Tp;
  }

  size_t GetSize() const {
    assert(HasRun);
    int i = 0;
    for (; Tp.Tokens[i] != NULL; ++i)
      ;

    return i + 1;
  }

  std::vector<std::string> ToVec() const {
    assert(HasRun);

    std::vector<std::string> tokens;
    for (int i = 0; Tp.Tokens[i] != NULL; ++i) tokens.push_back(Tp.Tokens[i]);

    return tokens;
  }

  ~TokenParserTestEnvironment() {
    if (HasRun) {
      TokenParserDestroy(&Tp);
    }
  }
};

using TPTE = TokenParserTestEnvironment;
using ETE = ExecutorTestEnvironment;

void SimpleInit(ETE& ete) {
  ete.SetInputFile("/dev/null");
  ete.SetOutputFile("/dev/null");
  ete.Init(environ);
}

void RunFullTest(const std::string& commandsName, const std::string& inputName,
                 const std::string& expectedName) {
  static const char* outputName = "tmp";

  std::ifstream commandsStream{ExecutorDataPath + commandsName};
  ASSERT_TRUE(commandsStream.good());

  std::ifstream expectedStream{ExecutorDataPath + expectedName};
  ASSERT_TRUE(commandsStream.good());

  ETE ete{};
  ASSERT_TRUE(ete.SetInputFile(ExecutorDataPath + inputName));
  ASSERT_TRUE(ete.SetOutputFile(outputName));

  std::string line;
  while (std::getline(commandsStream, line)) {
    ShellStatus status = ete.ExecuteLine(line);
    ASSERT_EQ(status, SH_SUCCESS);
  }

  ete.Stop();

  std::ifstream outputStream{outputName};
  assert(outputStream.good());

  std::stringstream output;
  output << outputStream.rdbuf();

  std::stringstream expected;
  expected << expectedStream.rdbuf();

  ASSERT_EQ(output.str(), expected.str());
}

void RunStringTest(const std::string& input,
                   const std::vector<std::string>& expected) {
  auto tpte = TPTE{};

  tpte.LoadString(input);

  TestResult res = tpte.Run();

  ASSERT_EQ(res.Error, TestResult::TestError::Success);
  ASSERT_EQ(res.Result, SH_SUCCESS);

  EXPECT_EQ(tpte.GetSize(), expected.size() + 1);
  ASSERT_EQ(tpte.ToVec(), expected);
}

void RunFileTest(const std::string& inputName,
                 const std::string& expectedName) {
  std::string inputData;
  std::vector<std::string> expectedData;

  std::ifstream inputStream{TParserDataPath + inputName};
  ASSERT_TRUE(inputStream.good());

  std::getline(inputStream, inputData);

  std::ifstream expectedStream{TParserDataPath + expectedName};

  std::string buf;
  while (std::getline(expectedStream, buf, ' ')) expectedData.push_back(buf);

  RunStringTest(inputData, expectedData);
}

TEST(Executor, Exit) {
  ETE ete{};
  SimpleInit(ete);

  ASSERT_TRUE(ete.Get().Active);

  ShellStatus status = ete.ExecuteLine("exit");
  EXPECT_EQ(status, SH_SUCCESS);

  ASSERT_FALSE(ete.Get().Active);

  ete.Stop();
}

TEST(Executor, Empty) {
  ETE ete{};
  SimpleInit(ete);

  EXPECT_EQ(ete.ExecuteLine("    "), SH_SUCCESS);

  ete.Stop();
}

TEST(Executor, ToFile) {
  std::string content = "ABC";
  std::string file = "tmp";

  ETE ete{};
  SimpleInit(ete);

  EXPECT_EQ(ete.ExecuteLine("echo " + content + " > " + file), SH_SUCCESS);

  std::ifstream outputStream{file};
  assert(outputStream.good());

  std::stringstream output;
  output << outputStream.rdbuf();

  ASSERT_EQ(content + "\n", output.str());

  ete.Stop();
}

TEST(Executor, FromFile) {
  std::string content = "Hello DEF";
  std::string fileIn = "Text.input";
  std::string fileOut = "tmp";

  ETE ete{};
  SimpleInit(ete);

  ShellStatus status =
      ete.ExecuteLine("grep DEF < " + std::string(ExecutorDataPath) + fileIn + " > tmp");
  EXPECT_EQ(status, SH_SUCCESS);

  std::ifstream outputStream{fileOut};
  assert(outputStream.good());

  std::stringstream output;
  output << outputStream.rdbuf();

  ASSERT_EQ(content + "\n", output.str());

  ete.Stop();
}

TEST(Executor, InputOverride) {
  ETE ete{};
  SimpleInit(ete);

  ShellStatus status = ete.ExecuteLine("ls | ls < tmp");
  ASSERT_EQ(status, SH_INPUT_OVERRIDE);

  ete.Stop();
}

TEST(Executor, OutputOverride) {
  ETE ete{};
  SimpleInit(ete);

  ShellStatus status = ete.ExecuteLine("ls > tmp | grep abc");
  ASSERT_EQ(status, SH_OUTPUT_OVERRIDE);

  ete.Stop();
}

TEST(Executor, FileNotFound) {
  ETE ete{};
  SimpleInit(ete);

  ShellStatus status =
      ete.ExecuteLine("ls < SomeFileThatDoesNotExist | grep abd");
  ASSERT_EQ(status, SH_ERRNO_ERROR);
  ASSERT_EQ(errno, ENOENT);

  ete.Stop();
}

TEST(Executor, SyntaxError) {
  ETE ete{};
  SimpleInit(ete);

  ShellStatus status =
      ete.ExecuteLine("ls | | |");
  ASSERT_EQ(status, SH_SYNTAX_ERROR);

  ete.Stop();
}

TEST(TokenParser, EmptyString) {
  auto tpte = TPTE{};

  tpte.LoadString("");
  tpte.Run();

  ASSERT_EQ(tpte.GetSize(), 1);
}

TEST(TokenParser, EmptyStringWithSpaces) {
  auto tpte = TPTE{};

  tpte.LoadString("                 ");
  tpte.Run();

  ASSERT_EQ(tpte.GetSize(), 1);
}

TEST(TokenParser, EasyString) {
  std::string input = "lorem ipsum sir dolor amet";
  std::vector<std::string> expected = {"lorem", "ipsum", "sir", "dolor",
                                       "amet"};

  RunStringTest(input, expected);
}

TEST(TokenParser, EasyStringWithSpaces) {
  std::string input = "    lorem      ipsum       sir dolor  amet ";
  std::vector<std::string> expected = {"lorem", "ipsum", "sir", "dolor",
                                       "amet"};

  RunStringTest(input, expected);
}

TEST(TokenParser, EasyFile) {
  std::string inputName = "Easy.input";
  std::string expectedName = "Easy.expect";

  RunFileTest(inputName, expectedName);
}

TEST(TokenParser, EasyFileWithSpaces) {
  std::string inputName = "EasyWithSpaces.input";
  std::string expectedName = "Easy.expect";

  RunFileTest(inputName, expectedName);
}

TEST(TokenParser, 4KBOfSpaces) {
  std::string inputName = "4KBOfSpaces.input";
  std::string expectedName = "4KBOfSpaces.expect";

  RunFileTest(inputName, expectedName);
}

TEST(TokenParser, LargeFile) {
  std::string inputName = "LargeFile.input";
  std::string expectedName = "LargeFile.expect";

  RunFileTest(inputName, expectedName);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}