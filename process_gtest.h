#ifndef _HPS_SYS_PROCESS_GTEST_H_
#define _HPS_SYS_PROCESS_GTEST_H_
#include "process.h"
#include "gtest/gtest.h"

namespace _hps_sys_process_gtest_h_
{
using namespace hps;

TEST(Python, Process)
{
  Process process;
  process.Start("python -c \"print 2 * 2\"");
  //process.Start("cmd /C date /T");
  std::string out;
  process.ReadStdout(&out);
  process.Join();
  ASSERT_FALSE(out.empty());
  EXPECT_EQ('4', out[0]);
}

}

#endif //_HPS_SYS_PROCESS_GTEST_H_
