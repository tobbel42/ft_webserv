#include "CGI.hpp"


CGI::CGI(const CGI& other):
	m_filename(other.m_filename),
	m_content(other.m_content),
	p_env(other.p_env),
	m_status_code(other.m_status_code)
{}

CGI::CGI(const std::string& filename, char** envp):
	m_filename(filename),
	m_content(),
	p_env(envp),
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
		p_env = other.p_env;
		m_status_code = other.m_status_code;
	}
	return *this;
}

std::string
CGI::run(e_FileType file_type, const std::string& input)
{
	char* argv[3];
	FileWrap infile, outfile;

	if (!prep_files(infile, outfile, input))
		return std::string();

	switch (file_type)
	{
		case PHP:
			argv[0] = (char*)"/usr/bin/php";
			break;
		case PYTHON:
			argv[0] = (char*)"/usr/bin/python";
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
		execve(argv[0], argv, p_env);
		perror("cgi execve");
		std::exit(errno);
	}
	else // parent
	{
		int stat_loc = 0;
		// TODO: protection against infinate scripts
		if (waitpid(pid, &stat_loc, 0) == -1)
		{
			m_status_code = 500;
			perror("cgi wait");
			return false;
		}
		// temporarily disabled
		// if (WEXITSTATUS(stat_loc) != 0)
		// {
		// 	EPRINT("exit status was not 0" << WEXITSTATUS(stat_loc));
		// 	m_status_code = 500;
		// 	return false;
		// }
	}
	return true;
}

std::string
CGI::read_output(FileWrap& outfile)
{
	char buf[1000];
	std::string output;

	rewind(outfile); // == fseek(outfile, 0, SEEK_SET)
	while (fgets(buf, sizeof(buf), outfile) != nullptr)
		output += buf;

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
