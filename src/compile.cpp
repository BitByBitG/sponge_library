#include<bits/stdc++.h>
#include<process.h>
#include<windows.h>
using namespace std;
namespace fs=filesystem;

constexpr array<string_view,5> default_args=
{
	"-std=c++26",
	"-O2",
	"-Wall",
	"-Wextra",
	"-DLOCAL"
};

fs::path executable_path()
{
	wstring buf(32768,L'\0');
	DWORD n=GetModuleFileNameW(nullptr,buf.data(),(DWORD)buf.size());
	if(!n||n==buf.size())
		throw runtime_error("cannot get compile.exe path");
	buf.resize(n);
	return fs::path(buf);
}

int main(int argc,char* argv[])
{
	if(argc<2)
	{
		cerr<<"usage: compile <source-or-stem> [g++ arguments...]\n";
		return 1;
	}

	fs::path src=argv[1];
	if(src.extension().empty())src+=".cpp";

	fs::path out=src;
	out.replace_extension(".exe");

	fs::path exe=executable_path();
	fs::path root=exe.parent_path().parent_path();
	fs::path include=root/"include";

	if(!fs::exists(include/"sponge"/"core.hpp"))
	{
		cerr<<"cannot find sponge library include directory:\n";
		cerr<<include.string()<<'\n';
		return 1;
	}

	vector<string> args;
	args.reserve(default_args.size()+argc+5);

	args.emplace_back("g++");

	for(string_view s:default_args)
		args.emplace_back(s);

	args.emplace_back("-I"+include.string());
	args.emplace_back("-Wl,--stack,536870912");

	for(int i=2;i<argc;i++)
		args.emplace_back(argv[i]);

	args.emplace_back(src.string());
	args.emplace_back("-o");
	args.emplace_back(out.string());

	vector<const char*> cmd;
	cmd.reserve(args.size()+1);

	for(const string&s:args)
		cmd.push_back(s.c_str());

	cmd.push_back(nullptr);

	cerr<<"g++";
	for(int i=1;i<(int)args.size();i++)
		cerr<<' '<<args[i];
	cerr<<'\n';

	int ret=_spawnvp(_P_WAIT,cmd[0],cmd.data());

	if(ret==-1)
	{
		perror("cannot start g++");
		return 1;
	}

	cerr<<"g++ returned "<<ret<<".\n";
	return ret;
}