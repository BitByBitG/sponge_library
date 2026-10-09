#define NOMINMAX
#include<bits/stdc++.h>
#include<filesystem>
#include<windows.h>
using namespace std;
using namespace chrono;
namespace fs=filesystem;

string quote_arg(const string& s)
{
	string t="\"";
	size_t cnt=0;
	for(char c:s)
	{
		if(c=='\\')++cnt;
		else
		{
			t.append(c=='"'?cnt*2+1:cnt,'\\');
			t+=c;
			cnt=0;
		}
	}
	t.append(cnt*2,'\\');
	t+='"';
	return t;
}

int main(int argc,char* argv[])
{
	if(argc<2)
	{
		cerr<<"Usage: run <program> [arguments...]\n";
		return 1;
	}

	fs::path program=argv[1];
	if(!program.has_extension())program+=".exe";
	program=fs::absolute(program).lexically_normal();

	if(!fs::is_regular_file(program))
	{
		cerr<<"Program not found: "<<program.string()<<'\n';
		return 1;
	}

	string program_string=program.string();
	string command=quote_arg(program_string);
	for(int i=2;i<argc;i++)
		command+=' ',command+=quote_arg(argv[i]);

	cerr<<"Running "<<command<<".\n";

	HANDLE job=CreateJobObjectA(nullptr,nullptr);
	if(!job)
	{
		cerr<<"Failed to create job: "<<GetLastError()<<'\n';
		return 1;
	}

	STARTUPINFOA si{};
	si.cb=sizeof(si);
	PROCESS_INFORMATION pi{};

	auto start=steady_clock::now();
	if(!CreateProcessA(
		program_string.c_str(),command.data(),
		nullptr,nullptr,TRUE,CREATE_SUSPENDED,
		nullptr,nullptr,&si,&pi))
	{
		DWORD err=GetLastError();
		CloseHandle(job);
		cerr<<"Failed to start process: "<<err<<'\n';
		return 1;
	}

	auto cleanup=[&]
	{
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		CloseHandle(job);
	};

	auto fail=[&](const char* message,DWORD err)
	{
		TerminateProcess(pi.hProcess,1);
		WaitForSingleObject(pi.hProcess,INFINITE);
		cleanup();
		cerr<<message<<": "<<err<<'\n';
		return 1;
	};

	if(!AssignProcessToJobObject(job,pi.hProcess))
		return fail("Failed to assign process to job",GetLastError());

	if(ResumeThread(pi.hThread)==DWORD(-1))
		return fail("Failed to resume process",GetLastError());

	if(WaitForSingleObject(pi.hProcess,INFINITE)!=WAIT_OBJECT_0)
		return fail("Failed to wait for process",GetLastError());

	double run_time=duration<double>(steady_clock::now()-start).count();

	DWORD ret_val=0;
	BOOL exit_ok=GetExitCodeProcess(pi.hProcess,&ret_val);
	DWORD exit_err=exit_ok?0:GetLastError();

	JOBOBJECT_EXTENDED_LIMIT_INFORMATION info{};
	BOOL memory_ok=QueryInformationJobObject(
		job,JobObjectExtendedLimitInformation,
		&info,sizeof(info),nullptr);
	DWORD memory_err=memory_ok?0:GetLastError();

	cleanup();

	cerr<<"\n--------------------------------\n";
	cerr<<fixed<<setprecision(6);
	if(exit_ok)
		cerr<<"Process exited after "<<run_time
			<<" seconds with return value "<<ret_val<<".\n";
	else
		cerr<<"Failed to get exit code: "<<exit_err<<'\n';

	if(memory_ok)
		cerr<<"Peak committed memory usage: "
			<<double(info.PeakProcessMemoryUsed)/(1024*1024)
			<<" MiB.\n";
	else
		cerr<<"Failed to get memory usage: "<<memory_err<<'\n';

	system("pause");
	return exit_ok&&memory_ok?0:1;
}