#include<bits/stdc++.h>
#include<filesystem>
using namespace std;
namespace fs=filesystem;

class Logger
{
	enum class level_t{debug,info,warning,error,critical};
	level_t level=level_t::info;

	static level_t parse_level(string s)
	{
		for(char& c:s)c=toupper(static_cast<unsigned char>(c));
		if(s=="DEBUG")return level_t::debug;
		if(s=="WARNING"||s=="WARN")return level_t::warning;
		if(s=="ERROR")return level_t::error;
		if(s=="CRITICAL")return level_t::critical;
		return level_t::info;
	}

	void log(level_t message_level,const char* name,const string& message) const
	{
		if(message_level<level)return;
		time_t now=time(nullptr);
		tm local_time{};
#ifdef _WIN32
		localtime_s(&local_time,&now);
#else
		localtime_r(&now,&local_time);
#endif
		cerr<<put_time(&local_time,"%H:%M:%S")<<" ["<<name<<"] "<<message<<'\n';
	}

public:
	Logger()
	{
		if(const char* value=getenv("LOG_LEVEL"))level=parse_level(value);
	}

	void info(const string& message) const
	{
		log(level_t::info,"INFO",message);
	}

	void error(const string& message) const
	{
		log(level_t::error,"ERROR",message);
	}
};

const Logger logger;

vector<string> splitlines(const string& source)
{
	vector<string> result;
	size_t begin=0;
	while(begin<source.size())
	{
		size_t end=source.find_first_of("\r\n",begin);
		if(end==string::npos)
		{
			result.push_back(source.substr(begin));
			break;
		}
		result.push_back(source.substr(begin,end-begin));
		if(source[end]=='\r'&&end+1<source.size()&&source[end+1]=='\n')end++;
		begin=end+1;
	}
	return result;
}

string read_file(const fs::path& file_path)
{
	ifstream file(file_path,ios::binary);
	if(!file)throw runtime_error("cannot open: "+file_path.string());
	return string(istreambuf_iterator<char>(file),istreambuf_iterator<char>());
}

void write_file(const fs::path& file_path,const string& source)
{
	ofstream file(file_path,ios::binary);
	if(!file)throw runtime_error("cannot open: "+file_path.string());
	file.write(source.data(),static_cast<streamsize>(source.size()));
}

class Expander
{
	const regex spongelib_include{
		R"(#include\s*["<](sponge/[a-z0-9_]*(?:\.hpp)?)[">]\s*)"};
	vector<fs::path> lib_paths;
	set<fs::path> included;

	bool match_include(const string& line,string& name) const
	{
		smatch match;
		if(!regex_search(line,match,spongelib_include,
			regex_constants::match_continuous))return false;
		name=match[1].str();
		return true;
	}

	bool is_ignored_line(const string& line) const
	{
		size_t l=line.find_first_not_of(" \t\r\n");
		if(l==string::npos)return false;
		size_t r=line.find_last_not_of(" \t\r\n");
		string stripped=line.substr(l,r-l+1);
		return stripped=="#pragma once"||stripped.starts_with("//");
	}

	fs::path find_spongelib(const string& name) const
	{
		for(const fs::path& lib_path:lib_paths)
		{
			fs::path file_path=lib_path/name;
			if(fs::exists(file_path))return file_path;
		}
		logger.error("cannot find: "+name);
		throw fs::filesystem_error(
			"cannot find: "+name,make_error_code(errc::no_such_file_or_directory));
	}

	vector<string> expand_spongelib(const fs::path& file_path)
	{
		if(included.contains(file_path))
		{
			logger.info("already included: "+file_path.filename().string());
			return {};
		}
		included.insert(file_path);
		logger.info("include: "+file_path.filename().string());

		vector<string> result;
		for(const string& line:splitlines(read_file(file_path)))
		{
			if(is_ignored_line(line))continue;
			string name;
			if(match_include(line,name))
			{
				auto expanded=expand_spongelib(find_spongelib(name));
				result.insert(result.end(),expanded.begin(),expanded.end());
			}
			else result.push_back(line);
		}
		return result;
	}

public:
	explicit Expander(vector<fs::path> lib_paths):lib_paths(move(lib_paths)){}

	string expand(const string& source,const optional<string>& origname)
	{
		included.clear();
		vector<string> result;
		int linenum=0;
		for(const string& line:splitlines(source))
		{
			linenum++;
			string name;
			if(match_include(line,name))
			{
				auto expanded=expand_spongelib(find_spongelib(name));
				result.insert(result.end(),expanded.begin(),expanded.end());
				if(origname)result.push_back(
					"#line "+to_string(linenum+1)+" \""+*origname+"\"");
			}
			else result.push_back(line);
		}

		string output;
		for(size_t i=0;i<result.size();i++)
		{
			if(i)output+='\n';
			output+=result[i];
		}
		return output;
	}
};

struct options_t
{
	fs::path source;
	bool console=false;
	optional<fs::path> lib;
	optional<string> origname;
};

[[noreturn]] void usage(const char* program,int exit_code)
{
	ostream& out=exit_code?cerr:cout;
	out<<"usage: "<<program
		<<" [-h] [-c] [--lib LIB] [--origname ORIGNAME] source\n"
		<<"\nspongelib expander\n\n"
		<<"positional arguments:\n"
		<<"  source                source file\n\n"
		<<"options:\n"
		<<"  -h, --help            show this help message and exit\n"
		<<"  -c, --console         print to stdout\n"
		<<"  --lib LIB             include root containing sponge/\n"
		<<"  --origname ORIGNAME   report original source line numbers in GCC/Clang messages\n";
	exit(exit_code);
}

options_t parse_options(int argc,char* argv[])
{
	options_t opts;
	bool has_source=false;
	for(int i=1;i<argc;i++)
	{
		string arg=argv[i];
		if(arg=="-h"||arg=="--help")usage(argv[0],0);
		if(arg=="-c"||arg=="--console")
		{
			opts.console=true;
			continue;
		}
		auto take_value=[&](const string& option)->string
		{
			if(i+1>=argc)
			{
				cerr<<argv[0]<<": error: argument "
					<<option<<": expected one argument\n";
				usage(argv[0],2);
			}
			return argv[++i];
		};
		if(arg=="--lib")
		{
			opts.lib=take_value(arg);
			continue;
		}
		if(arg.starts_with("--lib="))
		{
			opts.lib=arg.substr(6);
			continue;
		}
		if(arg=="--origname")
		{
			opts.origname=take_value(arg);
			continue;
		}
		if(arg.starts_with("--origname="))
		{
			opts.origname=arg.substr(11);
			continue;
		}
		if(!arg.empty()&&arg[0]=='-')
		{
			cerr<<argv[0]<<": error: unrecognized arguments: "<<arg<<'\n';
			usage(argv[0],2);
		}
		if(has_source)
		{
			cerr<<argv[0]<<": error: unrecognized arguments: "<<arg<<'\n';
			usage(argv[0],2);
		}
		opts.source=arg;
		has_source=true;
	}
	if(!has_source)
	{
		cerr<<argv[0]<<": error: the following arguments are required: source\n";
		usage(argv[0],2);
	}
	return opts;
}

int main(int argc,char* argv[])
{
	try
	{
		auto opts=parse_options(argc,argv);
		vector<fs::path> lib_paths;
		if(opts.lib)
			lib_paths.push_back(*opts.lib);
		else
			lib_paths.emplace_back(
				R"(C:\Users\zhzx033\Desktop\code\sponge_library\include)");
		if(const char* value=getenv("CPLUS_INCLUDE_PATH"))
		{
#ifdef _WIN32
			constexpr char separator=';';
#else
			constexpr char separator=':';
#endif
			string paths=value;
			size_t begin=0;
			while(begin<=paths.size())
			{
				size_t end=paths.find(separator,begin);
				string path=paths.substr(begin,end-begin);
				if(!path.empty())lib_paths.emplace_back(path);
				if(end==string::npos)break;
				begin=end+1;
			}
		}
		lib_paths.push_back(fs::current_path());

		Expander expander(move(lib_paths));
		string output=expander.expand(read_file(opts.source),opts.origname);
		if(opts.console)cout<<output<<'\n';
		else write_file("combined.cpp",output);
	}
	catch(const exception& error)
	{
		cerr<<error.what()<<'\n';
		return 1;
	}
	return 0;
}
