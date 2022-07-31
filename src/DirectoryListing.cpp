#include "DirectoryListing.hpp"


DirectoryListing::DirectoryListing(const std::string& dirpath, const std::string& url):
	m_dirpath(dirpath),
	m_url(url),
	m_status_code(200)
{}

DirectoryListing::DirectoryListing(const DirectoryListing& other):
	m_dirpath(other.m_dirpath),
	m_url(other.m_url),
	m_status_code(other.m_status_code)
{}

DirectoryListing::~DirectoryListing() {}

DirectoryListing&
DirectoryListing::operator=(const DirectoryListing& other)
{
	if (this != &other)
	{
		m_dirpath = other.m_dirpath;
		m_url = other.m_url;
		m_status_code = other.m_status_code;
	}
	return *this;
}

std::string
DirectoryListing::run()
{
	DirWrap dir;

	if (!dir.open(m_dirpath))
	{
		m_status_code = 403; // or maybe 404
		perror("directory listing opendir");
		return std::string();
	}

	dirent* entry = nullptr;
	std::string table = 
		"<table>"
			"<tr>"
				"<th>Name</th>"
				"<th>Moddate</th>"
			"</tr>";

	while ((entry = readdir(dir)) != nullptr)
		table += generate_table_row(entry->d_name);

	table += "</table>";
	return "<h2>" + m_dirpath + "</h2>" + table;
}

std::string
DirectoryListing::generate_table_row(const std::string& entry_name) const
{
	if (entry_name == "." || entry_name == "..")
		return std::string();

	std::string full_path = "http://" + utils::compr_slash(m_url + "/" + entry_name);
	PRINT("URL = " << m_url);
	PRINT("entry_name = " << entry_name);
	PRINT("full path = " << full_path);
	return
		"<tr>"
			"<td>"
				"<a "
					"href =\"" + full_path + "\">"
					+ entry_name +
				"</a>"
			"</td>"
			"<td>"
				+ get_mod_date(entry_name) +
			"</td>"
		"</tr>";
}

std::string
DirectoryListing::get_mod_date(const std::string& entry_name) const
{
	struct stat entry_opts;
	std::string filename = utils::compr_slash(m_dirpath + "/" + entry_name);

	if (stat(filename.c_str(), &entry_opts) == 0)
	{
		char timestamp[1000];
		strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S",
				localtime(&entry_opts.st_mtime));
		return timestamp;
	}
	else
		return "-";
}


//-------------------------------------------------------------------------
//		DirWrap

DirectoryListing::DirWrap::DirWrap(): p_dir() {}

DirectoryListing::DirWrap::DirWrap(DIR* dir): p_dir(dir) {}

DirectoryListing::DirWrap::~DirWrap()
{
	closedir(p_dir);
}

DirectoryListing::DirWrap::operator DIR* () const
{
	return p_dir;
}

void
DirectoryListing::DirWrap::set_dir(DIR* dir)
{
	if (p_dir != nullptr)
		closedir(p_dir);
	p_dir = dir;
}

bool
DirectoryListing::DirWrap::open(const std::string& filename)
{
	set_dir(opendir(filename.c_str()));
	if (p_dir == nullptr)
		return false;
	return true;
}

void
DirectoryListing::DirWrap::close()
{
	closedir(p_dir);
	p_dir = nullptr;
}