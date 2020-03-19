
#include "path_opt.hxx"
#include "file_opt.hxx"
#include "stringtokenizer.hxx"
#include <algorithm>
#include "bugcheck.hxx"
#include "path_unix.hxx"

NAMESPACE_PREFIX
Path::Path(const std::string& path)
{
	assign(path);
}


Path::Path(const std::string& path, Style style)
{
	assign(path, style);
}


Path::Path(const char* path)
{
	common_check_ptr(path);
	assign(path);
}

Path::Path(const char* path, Style style)
{
	common_check_ptr(path);
	assign(path, style);
}

Path::Path(const Path& path):
	_node(path._node),
	_device(path._device),
	_name(path._name),
	_dirs(path._dirs),
	_absolute(path._absolute)
{
}

Path::Path(const Path& parent, const std::string& fileName):
	_node(parent._node),
	_device(parent._device),
	_name(parent._name),
	_dirs(parent._dirs),
	_absolute(parent._absolute)
{
	makeDirectory();
	_name = fileName;
}


Path::Path(const Path& parent, const char* fileName):
	
	_node(parent._node),
	_device(parent._device),
	_name(parent._name),
	_dirs(parent._dirs),
	_absolute(parent._absolute)
{	
	common_check_ptr(fileName);
	
	makeDirectory();
	_name = fileName;
}


Path::Path(const Path& parent, const Path& relative):
	_node(parent._node),
	_device(parent._device),
	_name(parent._name),
	_dirs(parent._dirs),
	_absolute(parent._absolute)
{
	resolve(relative);
}


Path& Path::operator = (const Path& path)
{
	return assign(path);
}


Path& Path::operator = (const std::string& path)
{
	return assign(path);
}


Path& Path::operator = (const char* path)
{
	common_check_ptr(path);
	return assign(path);
}


void Path::swap(Path& path)
{
	std::swap(_node, path._node);
	std::swap(_device, path._device);
	std::swap(_name, path._name);
	std::swap(_dirs, path._dirs);
	std::swap(_absolute, path._absolute);
}


Path& Path::assign(const Path& path)
{
	if (&path != this)
	{
		_node     = path._node;
		_device   = path._device;
		_name     = path._name;
		_dirs     = path._dirs;
		_absolute = path._absolute;
	}
	return *this;
}


Path& Path::assign(const std::string& path)
{

	parseUnix(path);
	return *this;
}


Path& Path::assign(const std::string& path, Style style)
{
	switch (style)
	{
	case Style::PATH_UNIX:
		parseUnix(path);
		break;
	case Style::PATH_WINDOWS:
		parseWindows(path);
		break;
	case Style::PATH_NATIVE:
		assign(path);
		break;
	default:
		common_bugcheck();
		break;;
	}
	return *this;
}


Path& Path::assign(const char* path)
{
	common_check_ptr(path);
	return assign(std::string(path));
}


std::string Path::toString() const
{
	return buildUnix();
}


std::string Path::toString(Style style) const
{
	switch (style)
	{
	case Style::PATH_UNIX:
		return buildUnix();
	case Style::PATH_WINDOWS:
		return buildWindows();
	case Style::PATH_NATIVE:
		return toString();
	default:
		break;
	}
	return std::string();
}


bool Path::tryParse(const std::string& path)
{
	try
	{
		Path p;
		p.parse(path);
		assign(p);
		return true;
	}
	catch (...)
	{
		return false;
	}
}


bool Path::tryParse(const std::string& path, Style style)
{
	try
	{
		Path p;
		p.parse(path, style);
		assign(p);
		return true;
	}
	catch (...)
	{
		return false;
	}
}


Path& Path::parseDirectory(const std::string& path)
{
	assign(path);
	return makeDirectory();
}


Path& Path::parseDirectory(const std::string& path, Style style)
{
	assign(path, style);
	return makeDirectory();
}


Path& Path::makeDirectory()
{
	pushDirectory(_name);
	_name.clear();
	return *this;
}


Path& Path::makeFile()
{
	if (!_dirs.empty() && _name.empty())
	{
		_name = _dirs.back();
		_dirs.pop_back();
	}
	return *this;
}


Path& Path::makeAbsolute()
{
	return makeAbsolute(current());
}


Path& Path::makeAbsolute(const Path& base)
{
	if (!_absolute)
	{
		Path tmp = base;
		tmp.makeDirectory();
		for (StringVec::const_iterator it = _dirs.begin(); it != _dirs.end(); ++it)
		{
			tmp.pushDirectory(*it);
		}
		_node     = tmp._node;
		_device   = tmp._device;
		_dirs     = tmp._dirs;
		_absolute = base._absolute;
	}
	return *this;
}


Path Path::absolute() const
{
	Path result(*this);
	if (!result._absolute)
	{
		result.makeAbsolute();
	}
	return result;
}


Path Path::absolute(const Path& base) const
{
	Path result(*this);
	if (!result._absolute)
	{
		result.makeAbsolute(base);
	}
	return result;
}


Path Path::parent() const
{
	Path p(*this);
	return p.makeParent();
}


Path& Path::makeParent()
{
	if (_name.empty())
	{
		if (_dirs.empty())
		{
			if (!_absolute)
				_dirs.push_back("..");
		}
		else
		{
			if (_dirs.back() == "..")
				_dirs.push_back("..");
			else
				_dirs.pop_back();
		}
	}
	else
	{
		_name.clear();
	}
	return *this;
}


Path& Path::append(const Path& path)
{
	makeDirectory();
	_dirs.insert(_dirs.end(), path._dirs.begin(), path._dirs.end());
	_name = path._name;
	return *this;
}


Path& Path::resolve(const Path& path)
{
	if (path.isAbsolute())
	{
		assign(path);
	}
	else
	{
		for (int i = 0; i < path.depth(); ++i)
			pushDirectory(path[i]);
		_name = path._name;
	}
	return *this;
}


Path& Path::setNode(const std::string& node)
{
	_node     = node;
	_absolute = _absolute || !node.empty();
	return *this;
}


Path& Path::setDevice(const std::string& device)
{
	_device   = device;
	_absolute = _absolute || !device.empty();
	return *this;
}


const std::string& Path::directory(size_t n) const
{
	common_assert(n <= _dirs.size());
	if (n < _dirs.size())
		return _dirs[n];
	else
		return _name;
}


const std::string& Path::operator [] (size_t n) const
{
	common_assert(n <= _dirs.size());
	if (n < _dirs.size())
		return _dirs[n];
	else
		return _name;
}


Path& Path::pushDirectory(const std::string& dir)
{
	if (!dir.empty() && dir != ".")
	{
		if (dir == "..")
		{
			if (!_dirs.empty() && _dirs.back() != "..")
				_dirs.pop_back();
			else if (!_absolute)
				_dirs.push_back(dir);
		}
		else _dirs.push_back(dir);
	}
	return *this;
}

Path& Path::setFileName(const std::string& name)
{
	_name = name;
	return *this;
}


Path& Path::setBaseName(const std::string& name)
{
	std::string ext = getExtension();
	_name = name;
	if (!ext.empty())
	{
		_name.append(".");
		_name.append(ext);
	}
	return *this;
}


std::string Path::getBaseName() const
{
	std::string::size_type pos = _name.rfind('.');
	if (pos != std::string::npos)
		return _name.substr(0, pos);
	else
		return _name;
}


Path& Path::setExtension(const std::string& extension)
{
	_name = getBaseName();
	if (!extension.empty())
	{
		_name.append(".");
		_name.append(extension);
	}
	return *this;
}


std::string Path::getExtension() const
{
	std::string::size_type pos = _name.rfind('.');
	if (pos != std::string::npos)
		return _name.substr(pos + 1);
	else
		return std::string();
}


Path& Path::clear()
{
	_node.clear();
	_device.clear();
	_name.clear();
	_dirs.clear();
	_absolute = false;
	return *this;
}


std::string Path::current()
{
	return PathImpl::currentImpl();
}


std::string Path::home()
{
	return PathImpl::homeImpl();
}


std::string Path::configHome()
{

	return PathImpl::configHomeImpl();
}

	
std::string Path::dataHome()
{
	return PathImpl::dataHomeImpl();

}

	
std::string Path::tempHome()
{
	return PathImpl::tempHomeImpl();

}

	
std::string Path::cacheHome()
{
	return PathImpl::cacheHomeImpl();
}


std::string Path::temp()
{
	return PathImpl::tempImpl();
}


std::string Path::config()
{
	return PathImpl::configImpl();

}


std::string Path::null()
{
	return PathImpl::nullImpl();
}


std::string Path::expand(const std::string& path)
{
	return PathImpl::expandImpl(path);
}


void Path::listRoots(std::vector<std::string>& roots)
{
	PathImpl::listRootsImpl(roots);
}


bool Path::find(StringVec::const_iterator it, StringVec::const_iterator end, const std::string& name, Path& path)
{
	while (it != end)
	{

		Path p(*it);
		p.makeDirectory();
		p.resolve(Path(name));
		File f(p);
		if (f.exists())
		{
			path = p;
			return true;
		}
		++it;
	}
	return false;
}


bool Path::find(const std::string& pathList, const std::string& name, Path& path)
{
	StringTokenizer st(pathList, std::string(1, pathSeparator()), int(StringTokenizer::Options::TOK_IGNORE_EMPTY) + int(StringTokenizer::Options::TOK_TRIM));
	return find(st.begin(), st.end(), name, path);
}


void Path::parseUnix(const std::string& path)
{
	clear();

	std::string::const_iterator it  = path.begin();
	std::string::const_iterator end = path.end();

	if (it != end)
	{
		if (*it == '/')
		{
			_absolute = true; ++it;
		}
		else if (*it == '~')
		{
			++it;
			if (it == end || *it == '/')
			{
				Path cwd(home());
				_dirs = cwd._dirs;
				_absolute = true;
			}
			else --it;
		}

		while (it != end)
		{
			std::string name;
			while (it != end && *it != '/') name += *it++;
			if (it != end)
			{
				if (_dirs.empty())
				{
					if (!name.empty() && *(name.rbegin()) == ':')
					{
						_absolute = true;
						_device.assign(name, 0, name.length() - 1);
					}
					else
					{
						pushDirectory(name);
					}
				}
				else pushDirectory(name);
			}
			else _name = name;
			if (it != end) ++it;
		}
	}
}


void Path::parseWindows(const std::string& path)
{
	clear();

	std::string::const_iterator it  = path.begin();
	std::string::const_iterator end = path.end();

	if (it != end)
	{
		if (*it == '\\' || *it == '/') { _absolute = true; ++it; }
		if (_absolute && it != end && (*it == '\\' || *it == '/')) // UNC
		{
			++it;
			while (it != end && *it != '\\' && *it != '/') _node += *it++;
			if (it != end) ++it;
		}
		else if (it != end)
		{
			char d = *it++;
			if (it != end && *it == ':') // drive letter
			{
				if (_absolute || !((d >= 'a' && d <= 'z') || (d >= 'A' && d <= 'Z'))) throw PathSyntaxException(path);
				_absolute = true;
				_device += d;
				++it;
				if (it == end || (*it != '\\' && *it != '/')) throw PathSyntaxException(path);
				++it;
			}
			else --it;
		}
		while (it != end)
		{
			std::string name;
			while (it != end && *it != '\\' && *it != '/') name += *it++;
			if (it != end)
				pushDirectory(name);
			else
				_name = name;
			if (it != end) ++it;
		}
	}
	if (!_node.empty() && _dirs.empty() && !_name.empty())
		makeDirectory();
}


std::string Path::buildUnix() const
{
	std::string result;
	if (!_device.empty())
	{
		result.append("/");
		result.append(_device);
		result.append(":/");
	}
	else if (_absolute)
	{
		result.append("/");
	}
	for (StringVec::const_iterator it = _dirs.begin(); it != _dirs.end(); ++it)
	{
		result.append(*it);
		result.append("/");
	}
	result.append(_name);
	return result;
}


std::string Path::buildWindows() const
{
	std::string result;
	if (!_node.empty())
	{
		result.append("\\\\");
		result.append(_node);
		result.append("\\");
	}
	else if (!_device.empty())
	{
		result.append(_device);
		result.append(":\\");
	}
	else if (_absolute)
	{
		result.append("\\");
	}
	for (StringVec::const_iterator it = _dirs.begin(); it != _dirs.end(); ++it)
	{
		result.append(*it);
		result.append("\\");
	}
	result.append(_name);
	return result;
}

std::string Path::transcode(const std::string& path)
{
	return path;
}


NAMESPACE_SUBFIX
