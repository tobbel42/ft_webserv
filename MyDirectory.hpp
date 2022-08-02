#ifndef MYDIRECTORY_HPP
#define MYDIRECTORY_HPP

#include <iostream>
#include <vector>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>

class MyDirectory
{
private:
	std::string _directory_path;
	std::string _current_url;


	std::string get_mod_date(std::string element)
	{
		struct stat result;

		#ifdef VERBOSE
		std::cout << "element: " << _directory_path + "/" + element << std::endl;
		#endif
		element = _directory_path + "/" + element; // Achtung Hardgecoded
		if(stat(element.c_str(), &result) == 0)
		{
			char time[500];
			strftime(time, sizeof(time), "%Y-%m-%d %H:%M:%S", localtime(&result.st_mtime));
			std::string mod_date(time);
			return "<td>" + mod_date + "</td> </tr>";
		}
		else
			return "<td> - </td> </tr>";
	}

	std::string generate_table_row(std::string element)
	{
		std::string content;

		PRINT("URL: " << _current_url);
		PRINT("ELEMENT: " << element);

		if (element == ".." || element == ".")
			return "";
		content = content + 
			"<tr> \
				<td> <a href =\"" + _current_url + "/" + element + "\">" + element + "</a> </td>";
		content = content + get_mod_date(element);
		return (content);
	}
	std::string get_list_content_helper()
	{
		DIR *dir;
		struct dirent *diread;
		std::vector<char *> files;
		std::string content = 
			"<table> \
				<tr> \
					<th>Name</th> \
					<th>Moddate</th> \
				</tr>";

		if ((dir = opendir(_directory_path.c_str())) != nullptr) {
			while ((diread = readdir(dir)) != nullptr) {
				files.push_back(diread->d_name);
			}
			closedir (dir);
			for (size_t i = 0; i < files.size(); i++)
			{
				std::string element(files[i]);
				content = content + generate_table_row(element);
			}
			content = content + "</table>";
			return (content);
		}
		else
		{
			perror("opendir");
			return "";
		}
	}

public:
	MyDirectory(const std::string& directory_path, const std::string& current_url):
		_directory_path(directory_path),
		_current_url(current_url)
	{
		//if we access a dir both /dirname or /dirname/ are valid targets
		//so we have to normalize the input
		if (*(--_current_url.end()) == '/')
			_current_url.erase(_current_url.length() - 1);
		#ifdef VERBOSE
		std::cout << "current url: " << _current_url << std::endl;
		#endif
	}

	std::string list_content()
	{
		std::string dir_content =  get_list_content_helper();
		if (dir_content != "")
			return "<h2>" + _directory_path + "</h2>" + dir_content;
		else
			return "page not found";
	}
};


#endif