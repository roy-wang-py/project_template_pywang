
#include "path_unix.hxx"
#include "Exception.hxx"
#include "environment_unix.hxx"
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
#include <climits>


#ifndef PATH_MAX
#define PATH_MAX 1024 // fallback
#endif


NAMESPACE_PREFIX

std::string PathImpl::currentImpl()
{
	std::string path;
	char cwd[PATH_MAX];
	if (getcwd(cwd, sizeof(cwd)))
		path = cwd;
	else
		throw SystemException("cannot get current directory");
	std::string::size_type n = path.size();
	if (n > 0 && path[n - 1] != '/') 
		path.append("/");
	return path;
}


std::string PathImpl::homeImpl()
{

	std::string path;
	struct passwd* pwd = getpwuid(getuid());
	if (pwd)
		path = pwd->pw_dir;
	else
	{
		pwd = getpwuid(geteuid());
		if (pwd)
			path = pwd->pw_dir;
		else
			path = EnvironmentImpl::getImpl("HOME");
	}
	std::string::size_type n = path.size();
	if (n > 0 && path[n - 1] != '/') path.append("/");
	return path;

}


std::string PathImpl::configHomeImpl()
{
	std::string path = PathImpl::homeImpl();
	std::string::size_type n = path.size();
	if (n > 0 && path[n - 1] == '/') 
	  path.append(".config/");

	return path;
}


std::string PathImpl::dataHomeImpl()
{

	std::string path = PathImpl::homeImpl();
	std::string::size_type n = path.size();
	if (n > 0 && path[n - 1] == '/') 
	  path.append(".local/share/");

	return path;
}


std::string PathImpl::cacheHomeImpl()
{
	std::string path = PathImpl::homeImpl();
	std::string::size_type n = path.size();
	if (n > 0 && path[n - 1] == '/') 
		path.append(".cache/");

	return path;
}


std::string PathImpl::tempHomeImpl()
{
	std::string path = PathImpl::homeImpl();
	std::string::size_type n = path.size();
	if (n > 0 && path[n - 1] == '/')
	  path.append(".local/tmp/");

	return path;

}


std::string PathImpl::tempImpl()
{
	std::string path;
	char* tmp = getenv("TMPDIR");
	if (tmp)
	{
		path = tmp;
		std::string::size_type n = path.size();
		if (n > 0 && path[n - 1] != '/') path.append("/");
	}
	else
	{
		path = "/tmp/";
	}
	return path;
}



std::string PathImpl::expandImpl(const std::string& path)
{
	std::string result;
	std::string::const_iterator it  = path.begin();
	std::string::const_iterator end = path.end();
	if (it != end && *it == '~')
	{
		++it;
		if (it != end && *it == '/')
		{
			const char* homeEnv = getenv("HOME");
			if (homeEnv)
			{
				result += homeEnv;
				std::string::size_type resultSize = result.size();
				if (resultSize > 0 && result[resultSize - 1] != '/')
					result.append("/");
			}
			else
			{
				result += homeImpl();
			}
			++it;
		}
		else result += '~';
	}
	while (it != end)
	{
		if (*it == '$')
		{
			std::string var;
			++it;
			if (it != end && *it == '{')
			{
				++it;
				while (it != end && *it != '}') var += *it++;
				if (it != end) ++it;
			}
			else
			{
				while (it != end && (std::isalnum(*it) || *it == '_')) var += *it++;
			}
			char* val = getenv(var.c_str());
			if (val) result += val;
		}
		else result += *it++;
	}
	return result;
}
NAMESPACE_SUBFIX
