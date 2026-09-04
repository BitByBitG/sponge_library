#include<bits/stdc++.h>
#include<filesystem>
#include<process.h>
#include<conio.h>
using namespace std;
using namespace chrono;
namespace fs=filesystem;

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
	vector<const char*> args;
	args.push_back(program_string.c_str());
	for(int i=2;i<argc;i++)args.push_back(argv[i]);
	args.push_back(nullptr);

	cerr<<"Running \""<<program_string<<'"';
	for(int i=2;i<argc;i++)cerr<<" \""<<argv[i]<<'"';
	cerr<<".\n";

	auto start=steady_clock::now();
	int ret_val=_spawnv(_P_WAIT,program_string.c_str(),args.data());
	double run_time=duration<double>(steady_clock::now()-start).count();

	cerr<<"\n--------------------------------\n";
	if(ret_val==-1)
		cerr<<"Failed to start process.\n";
	else
		cerr<<fixed<<setprecision(6)
			<<"Process exited after "<<run_time
			<<" seconds with return value "<<ret_val<<".\n";

	system("pause");
	return ret_val==-1?1:0;
}