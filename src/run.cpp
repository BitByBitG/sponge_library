#include<bits/stdc++.h>
using namespace std;
using namespace chrono;
int main(int argc,char* argv[])
{
	string cmd;
    for(int i=1;i<argc;i++)cmd+=" "+string(argv[i]);
    cerr<<"Running"<<cmd<<".\n";
	auto start=high_resolution_clock::now();
	int ret_val=system(cmd.c_str());
	auto end=high_resolution_clock::now();
	auto duration=duration_cast<microseconds>(end-start);
	double run_time=(double)duration.count()/1e6;
	cerr<<"\n--------------------------------\n";
	cerr<<format("Process exited after {} seconds with return value {}.\n",run_time,ret_val);
	system("pause");
	return 0;
}