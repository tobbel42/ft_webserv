#include "CGI.hpp"


CGI::CGI(const CGI& other):
	m_filename(other.m_filename),
	m_content(other.m_content),
	m_req(other.m_req),
	m_env(other.m_env),
	m_cgi_header(other.m_cgi_header),
	m_status_code(other.m_status_code)
{}

CGI::CGI(const std::string& filename, const Request & req):
	m_filename(filename),
	m_content(),
	m_req(req),
	m_status_code(200)
{}

CGI::~CGI() {}

CGI&
CGI::operator=(const CGI& other)
{
	if (this != &other)
	{
		m_filename = other.m_filename;
		m_content = other.m_content;
		// m_req = other.m_req;
		m_env = other.m_env;
		m_cgi_header = other.m_cgi_header;
		m_status_code = other.m_status_code;
	}
	return *this;
}

const std::map<std::string, std::string> &
CGI::get_cgi_header() { return m_cgi_header; }

bool
CGI::prep_env() //toDo prep some env
{
	if (m_req.get_header_entry("Content-length").first)
		m_env["CONTENT_LENGTH"] = m_req.get_header_entry("Content-length").second;
	if (m_req.get_header_entry("Content-type").first)
		m_env["CONTENT_TYPE"] = m_req.get_header_entry("Content-type").second;
	m_env["GATEWAY_INTERFACE"] = "CGI/1.1";

	m_env["QUERY_STRING"] = m_req.get_query();
	m_env["REQUEST_METHOD"] = m_req.get_method();

	m_env["SERVER_NAME"] = "lil l and the beachboys 1.0";
	m_env["SERVER_PORT"] =  utils::to_string(m_req.get_port());
	m_env["SERVER_PROTOCOL"] = "HTTP/" + utils::to_string(HTTP_VERSION);

	m_env["PATH_INFO"] = utils::get_abs_path(m_filename);
	m_env["PATH_TRANSLATED"] = utils::get_abs_path(m_filename);

	for (std::map<std::string, std::string>::const_iterator iter = m_req.get_header().begin();
		iter != m_req.get_header().end(); ++iter) {
		m_env["HTTP_" + utils::cgi_str_toupper((*iter).first)] = (*iter).second;
	}
	return true;
}

std::string
CGI::run(e_FileType file_type, const std::string& input,
		const std::string& executable)
{
	char* argv[3];
	FileWrap infile, outfile;

	if (!prep_files(infile, outfile, input))
		return std::string();

	if (!prep_env())
		return std::string();

	switch (file_type)
	{
		case PHP:
			argv[0] = (char *)executable.c_str(); //cpp is killing me ;_;
			break;
		case PYTHON:
			argv[0] = (char *)executable.c_str();
			break;
		default:
			m_status_code = 501; // not implemented
			return std::string();
	}
	argv[1] = (char*)m_filename.c_str();
	argv[2] = nullptr;

	if (!exec_cgi(infile, outfile, argv))
		return std::string();

	return read_output(outfile);
}

bool
CGI::prep_files(FileWrap& infile, FileWrap& outfile, const std::string& input)
{
	if (!infile.open_tmpfile() ||
		!outfile.open_tmpfile())
	{
		m_status_code = 500;
		perror("cgi open tmpfile");
		return false;
	}

	if (!input.empty())
	{
		if (fwrite(input.c_str(), sizeof(std::string::value_type)/* = char */,
					input.size(), infile) != input.size())
		{
			m_status_code = 500;
			perror("cgi fwrite infile");
			return false;
		}
		rewind(infile);
	}
	return true;
}

char **
CGI::map_to_env() //todo protect
{
	char ** env = (char **)calloc(m_env.size() + 1, sizeof(char *));
	
	char * ptr;

	int i = 0;
	for (std::map<std::string, std::string>::iterator iter = m_env.begin();
		iter != m_env.end(); ++iter, ++i)
	{
		ptr = (char *)calloc((*iter).first.size() + (*iter).second.size() + 2, sizeof(char));
		if (!ptr)
			exit(EXIT_FAILURE);
		strncpy(ptr, (*iter).first.c_str(), (*iter).first.size());
		ptr[(*iter).first.size()] = '=';
		strncpy(ptr + (*iter).first.size() + 1, (*iter).second.c_str(), (*iter).second.size());
		env[i] = ptr;
 	}
	env[i] = nullptr;
	return env;
}


bool
CGI::exec_cgi(FileWrap& infile, FileWrap& outfile, char* argv[])
{
	pid_t pid = fork();
	if (pid < 0)
	{
		m_status_code = 500;
		perror("cgi fork");
		return false;
	}
	else if (pid == 0) // child
	{
		if (dup2(fileno(infile), STDIN_FILENO) < 0 ||
			dup2(fileno(outfile), STDOUT_FILENO) < 0)
		{
			perror("cgi dup2");
			std::exit(errno);
		}
		char ** envs = map_to_env();
		execve(argv[0], argv, envs);
		perror("cgi execve");
		std::exit(errno);
	}
	else // parent
	{
		int stat_loc = 0;
		PRINT("stat" << stat_loc);
		// TODO: protection against infinate scripts
		if (waitpid(pid, &stat_loc, 0) == -1)
		{
			m_status_code = 500;
			perror("cgi wait");
			return false;
		}
		if (WEXITSTATUS(stat_loc) != 0)
		{
			EPRINT("exit status was not 0" << WEXITSTATUS(stat_loc));
			m_status_code = 500;
			return false;
		}
	}
	return true;
}

void
CGI::parse_header(std::string & output) {
	size_t offset = 0;
	size_t pos = output.find("\r\n");
	std::string line;
	std::string key, value;
	while(pos != std::string::npos) {
		line = output.substr(offset, pos - offset);
		if (line == "")
			break;
		key = line.substr(0, line.find(":"));
		value = line.substr(line.find(":") + 2);
		m_cgi_header[key] = value;
		pos += 3;
		offset = pos;
		pos = output.find("\r\n", offset);
	}
	output = output.substr(offset);
}

std::string
CGI::read_output(FileWrap& outfile)
{
	char buf[1000];
	std::string output;

	rewind(outfile); // == fseek(outfile, 0, SEEK_SET)
	while (fgets(buf, sizeof(buf), outfile) != nullptr)
		output += buf;


	//we truncate the cgi response header and storing the key value
	//pairs inside the gci_header map 
	parse_header(output);


	if (ferror(outfile))
	{
		m_status_code = 500;
		perror("cgi reading outfile");
		return std::string();
	}
	return output;
}

//-------------------------------------------------------------------------
//		FileWrap

CGI::FileWrap::FileWrap(): p_file() {}

CGI::FileWrap::FileWrap(FILE* file): p_file(file) {}

CGI::FileWrap::~FileWrap()
{
	fclose(p_file);
}

CGI::FileWrap::operator FILE* () const
{
	return p_file;
}

bool
CGI::FileWrap::open(const std::string& filename, const char* mode)
{
	set_file(fopen(filename.c_str(), mode));
	if (p_file == nullptr)
		return false;
	return true;
}

bool
CGI::FileWrap::open_tmpfile()
{
	set_file(tmpfile());
	if (p_file == nullptr)
		return false;
	return true;
}

void
CGI::FileWrap::close()
{
	fclose(p_file);
	p_file = nullptr;
}

void
CGI::FileWrap::set_file(FILE* file)
{
	if (p_file != nullptr)
		fclose(p_file);
	p_file = file;
}
