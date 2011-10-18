#ifndef _HPS_SYS_CHILD_PROCESS_H_
#define _HPS_SYS_CHILD_PROCESS_H_
#if WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif
#include <cstring>
#include <string>
#include <algorithm>
#include <iostream>

namespace hps
{
namespace sys
{

class Process;

namespace detail
{
inline void CleanProcess(Process* process);
}

/// <summary> A child process. </summary>
class Process
{
  friend void detail::CleanProcess(Process* process);
public:
  enum { Fd_Read = 0, Fd_Write, Fd_Count, };

  Process();
  ~Process();

  bool Start(const std::string& commandLine);
  int Join();
  void Kill();
  int ReadStdout(std::string* str);

protected:

private:
  Process(const Process&);
  Process& operator=(const Process&);

#if WIN32
  HANDLE m_process;
  HANDLE m_stdin[Fd_Count];
  HANDLE m_stdout[Fd_Count];
  HANDLE m_stderr[Fd_Count];
#else
#endif
};

#if WIN32
namespace detail
{
  inline void CloseHandles(HANDLE handles[])
  {
    for (int handleIdx = 0; handleIdx < Process::Fd_Count; ++handleIdx)
    {
      CloseHandle(handles[handleIdx]);
      handles[handleIdx] = NULL;
    }
  }

  inline void CleanProcess(Process* process)
  {
    CloseHandles(process->m_stdin);
    CloseHandles(process->m_stdout);
    CloseHandles(process->m_stderr);
  }
}

int Process::ReadStdout(std::string* str)
{
  assert(str);
  str->clear();
  char strBuff[4096] = {0};
  DWORD numRead;
  ReadFile(m_stdout[Fd_Read], strBuff, sizeof(strBuff), &numRead, NULL);
  str->assign(strBuff);
  return static_cast<int>(numRead);
}

#if WIN32
#pragma warning(push)
#pragma warning(disable: 4351)
#endif
Process::Process()
: m_process(NULL),
  m_stdin(),
  m_stdout(),
  m_stderr()
{
  memset(m_stdin, 0, sizeof(m_stdin));
  memset(m_stdout, 0, sizeof(m_stdout));
  memset(m_stderr, 0, sizeof(m_stderr));
}
#if WIN32
#pragma warning(pop)
#endif

Process::~Process()
{
  detail::CleanProcess(this);
}

//bool Process::Start(const std::string& commandLine)
//{
//  assert(!commandLine.empty());
//
//  // Set up the security attributes struct.
//  SECURITY_ATTRIBUTES sa;
//  {
//    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
//    sa.lpSecurityDescriptor = NULL;
//    sa.bInheritHandle = TRUE;
//  }
//  // Create the child output pipe.
//  HANDLE outputReadTmp;
//  if (!CreatePipe(&outputReadTmp, &m_stdout, &sa, 0))
//  {
//    detail::CleanProcess(this);
//    return false;
//  }
//  // Create a duplicate of the output write handle for the std error
//  // write handle. This is necessary in case the child application
//  // closes one of its std output handles.
//  if (!DuplicateHandle(GetCurrentProcess(), m_stdout,
//                       GetCurrentProcess(), &m_stderr,
//                       0, TRUE, DUPLICATE_SAME_ACCESS))
//  {
//    CloseHandle(outputReadTmp);
//    detail::CleanProcess(this);
//    return false;
//  }
//  // Create the child input pipe.
//  HANDLE inputWriteTmp;
//  if (!CreatePipe(&m_stdin, &inputWriteTmp, &sa, 0))
//  {
//    CloseHandle(outputReadTmp);
//    detail::CleanProcess(this);
//    return false;
//  }
//  // Create new output read handle and the input write handles. Set
//  // the Properties to FALSE. Otherwise, the child inherits the
//  // properties and, as a result, non-closeable handles to the pipes
//  // are created.
//  if (!DuplicateHandle(GetCurrentProcess(), outputReadTmp,
//                       GetCurrentProcess(),
//                       &m_stdout, // Address of new handle.
//                       0, FALSE, // Make it uninheritable.
//                       DUPLICATE_SAME_ACCESS))
//  {
//    CloseHandle(inputWriteTmp);
//    CloseHandle(outputReadTmp);
//    detail::CleanProcess(this);
//    return false;
//  }
//  if (!DuplicateHandle(GetCurrentProcess(), inputWriteTmp,
//                       GetCurrentProcess(),
//                       &m_stdin, // Address of new handle.
//                       0, FALSE, // Make it uninheritable.
//                       DUPLICATE_SAME_ACCESS))
//  {
//    CloseHandle(inputWriteTmp);
//    CloseHandle(outputReadTmp);
//    detail::CleanProcess(this);
//    return false;
//  }
//  // Close inheritable copies of the handles you do not want to be
//  // inherited.
//  CloseHandle(inputWriteTmp);
//  CloseHandle(outputReadTmp);
//  // Start process with connected pipes.
//  STARTUPINFO startupInfo;
//  memset(&startupInfo, 0, sizeof(startupInfo));
//  {
//    startupInfo.cb = sizeof(startupInfo);
//    startupInfo.dwFlags = STARTF_USESTDHANDLES;
//    startupInfo.hStdOutput = m_stdout;
//    startupInfo.hStdInput  = m_stdin;
//    startupInfo.hStdError  = m_stderr;
//  }
//  PROCESS_INFORMATION processInfo;
////  wchar_t wCommandLine[2048] = {0};
////  {
////    std::wstring wStrTmp(L" ", commandLine.size());
////    std::copy(commandLine.begin(), commandLine.end(), wStrTmp.begin());
////    memcpy(wCommandLine, wStrTmp.c_str(),
////           wStrTmp.size() * sizeof(std::wstring::value_type));
////  }
//  char cstrCmdLine[2048] = {0};
//  {
//    memcpy(cstrCmdLine, commandLine.c_str(),
//           commandLine.size() * sizeof(std::string::value_type));
//  }
//  if (!CreateProcess(NULL, cstrCmdLine,
//                     NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL,
//                     &startupInfo, &processInfo))
//  {
//    const DWORD err = GetLastError();
//    char strPath[4096];
//    GetEnvironmentVariable("PATH", strPath, sizeof(strPath) / sizeof(strPath[0]));
//    detail::CleanProcess(this);
//    return false;
//  }
//  m_process = processInfo.hProcess;
//  CloseHandle(processInfo.hThread);
//  return true;
//}

bool Process::Start(const std::string& commandLine)
{ 
  // Make sure this is not running.
  Kill();
  detail::CleanProcess(this);

  // Set up the security attributes struct.
  SECURITY_ATTRIBUTES sa;
  {
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
  }
  // Create a pipe for the child process's STDOUT. 
  if (!CreatePipe(&m_stdout[Fd_Read], &m_stdout[Fd_Write], &sa, 0)) 
  {
    return false;
  }
  // Ensure the read handle to the pipe for STDOUT is not inherited.
  if (!SetHandleInformation(m_stdout[Fd_Read], HANDLE_FLAG_INHERIT, 0))
  {
    detail::CleanProcess(this);
    return false;
  }
  // Create a pipe for the child process's STDIN. 
  if (!CreatePipe(&m_stdin[Fd_Read], &m_stdin[Fd_Write], &sa, 0)) 
  {
    detail::CleanProcess(this);
    return false;
  }
  // Ensure the write handle to the pipe for STDIN is not inherited. 
  if (!SetHandleInformation(m_stdin[Fd_Write], HANDLE_FLAG_INHERIT, 0))
  {
    detail::CleanProcess(this);
    return false;
  }
  // Create a pipe for the child process's STDERR.
  if (!CreatePipe(&m_stderr[Fd_Read], &m_stderr[Fd_Write], &sa, 0)) 
  {
    return false;
  }
  // Ensure the read handle to the pipe for STDERR is not inherited.
  if (!SetHandleInformation(m_stderr[Fd_Read], HANDLE_FLAG_INHERIT, 0))
  {
    detail::CleanProcess(this);
    return false;
  }
  // Set up members of the STARTUPINFO structure. 
  // This structure specifies the STDIN and STDOUT handles for redirection.
  STARTUPINFO startupInfo;
  memset(&startupInfo, 0, sizeof(startupInfo));
  {
    startupInfo.cb = sizeof(startupInfo);
    startupInfo.dwFlags = STARTF_USESTDHANDLES;
    startupInfo.hStdOutput = m_stdout[Fd_Write];
    startupInfo.hStdInput  = m_stdin[Fd_Read];
    startupInfo.hStdError  = m_stderr[Fd_Write];
  }
  // Create the child process. 
  char cstrCmdLine[2048] = {0};
  {
    memcpy(cstrCmdLine, commandLine.c_str(),
           commandLine.size() * sizeof(std::string::value_type));
  }
  PROCESS_INFORMATION processInfo; 
  if (!CreateProcess(NULL, cstrCmdLine,
                     NULL, NULL, TRUE, 0, NULL, NULL,
                     &startupInfo, &processInfo))
  {
    detail::CleanProcess(this);
    return false;
  }
  m_process = processInfo.hProcess;
  CloseHandle(processInfo.hThread);
  return true;
}

int Process::Join()
{
  DWORD exitCode = 0;
  if (NULL != m_process)
  {
    WaitForSingleObject(m_process, INFINITE);
    GetExitCodeProcess(m_process, &exitCode);
  }
  detail::CleanProcess(this);
  return static_cast<int>(exitCode);
}

void Process::Kill()
{
  TerminateProcess(m_process, 0);
  detail::CleanProcess(this);
}
#else
#endif

}
using namespace sys;
}

#endif //_HPS_SYS_CHILD_PROCESS_H_
