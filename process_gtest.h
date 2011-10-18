#ifndef _HPS_SYS_PROCESS_GTEST_H_
#define _HPS_SYS_PROCESS_GTEST_H_
#include "process.h"
#include "gtest/gtest.h"
#include <vector>

namespace _hps_sys_process_gtest_h_
{
using namespace hps;

TEST(SafeCoverage, Process)
{
  Process process;
  EXPECT_EQ(0, process.Join());
  process.Kill();
  std::string str;
  process.ReadStdout(&str);
  EXPECT_TRUE(str.empty());
}

TEST(StartAgain, Process)
{
  Process process;
#if WIN32
  std::vector<std::string> args;
  {
    args.push_back("cmd");
    args.push_back("/C");
    args.push_back("date /T");
  }
  EXPECT_TRUE(process.Start(args));
  EXPECT_TRUE(process.Start(args));
#else
  EXPECT_TRUE(process.Start("date"));
  EXPECT_TRUE(process.Start("date"));
#endif

}

TEST(Python, Process)
{
  Process process;
  std::vector<std::string> args(3);
  {
    args[0] = "python";
    args[1] = "-c";
    args[2] = "print 2 * 2";
  }
  ASSERT_TRUE(process.Start(args));
  std::string out;
  std::cout << "Reading from child STDOUT." << std::endl;
  process.ReadStdout(&out);
  std::cout << "Waiting for child to terminate." << std::endl;
  int status = process.Join();
  std::cout << "Child terminated with status " << status << "." << std::endl;
  std::cout << "Child output: " << out << std::endl;
  ASSERT_FALSE(out.empty());
  EXPECT_EQ('4', out[0]);
}

}

#endif //_HPS_SYS_PROCESS_GTEST_H_
