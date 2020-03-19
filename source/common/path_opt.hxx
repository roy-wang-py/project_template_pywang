#ifndef __SQHAO_PATH_HXX__
#define __SQHAO_PATH_HXX__

#include <vector>
#include <string>
#include <iostream>
#include "default_config.hxx"
NAMESPACE_PREFIX
class  Path {
public:
	enum class Style
	{
		PATH_UNIX,    /// Unix-style path
		PATH_WINDOWS, /// Windows-style path
		PATH_NATIVE,  /// The current platform's native style
	};
	
	typedef std::vector<std::string> StringVec;
	Path(bool absolute = false): _absolute(absolute){}

	Path(const char* path);
	Path(const char* path, Style style);
	Path(const std::string& path);
	Path(const std::string& path, Style style);
	Path(const Path& path);
	Path(const Path& parent, const std::string& fileName);
	Path(const Path& parent, const char* fileName);
	Path(const Path& parent, const Path& relative);
	~Path()=default;
	Path& operator = (const Path& path);
	Path& operator = (const std::string& path);
	Path& operator = (const char* path);

	friend std::ostream& operator << (std::ostream&os,const Path& path){
		return os<<path.toString();
	}
	void swap(Path& path);
		/// Swaps the path with another one.

	Path& assign(const std::string& path);
	Path& assign(const std::string& path, Style style);
	Path& assign(const Path& path);
	Path& assign(const char* path);
	std::string toString() const;
	std::string toString(Style style) const;
	Path& parse(const std::string& path){
		return assign(path);
	}
	Path& parse(const std::string& path, Style style){
		return assign(path,style);
	}
	bool tryParse(const std::string& path);
		/// Tries to interpret the given string as a path
		/// in native format.
		/// If the path is syntactically valid, assigns the
		/// path and returns true. Otherwise leaves the 
		/// object unchanged and returns false.

	bool tryParse(const std::string& path, Style style);
	Path& parseDirectory(const std::string& path);
	Path& parseDirectory(const std::string& path, Style style);
	Path& makeDirectory();
		/// If the path contains a filename, the filename is appended
		/// to the directory list and cleared. Thus the resulting path
		/// always refers to a directory.

	Path& makeFile();
		/// If the path contains no filename, the last directory
		/// becomes the filename.

	Path& makeParent();
		/// Makes the path refer to its parent.
		
	Path& makeAbsolute();

	Path& makeAbsolute(const Path& base);
	Path& append(const Path& path);		
	Path& resolve(const Path& path);
	bool isAbsolute() const{return _absolute;}		
	bool isRelative() const{return !_absolute;}
	bool isDirectory() const{
		return _name.empty();
	}
	bool isFile() const{
		return !_name.empty();
	}
	Path& setNode(const std::string& node);		
	const std::string& getNode() const{ return _node;}
		
	Path& setDevice(const std::string& device);
		/// Sets the device name.
		/// Setting a non-empty device automatically makes
		/// the path an absolute one.
		
	const std::string& getDevice() const{return _device;}
		/// Returns the device name.
	
	int depth() const{ return int(_dirs.size());}
	const std::string& directory(size_t n) const;
		/// Returns the n'th directory in the directory list.
		/// If n == depth(), returns the filename.
		
	const std::string& operator [] (size_t n) const;
		/// Returns the n'th directory in the directory list.
		/// If n == depth(), returns the filename.
		
	Path& pushDirectory(const std::string& dir);
		/// Adds a directory to the directory list.
		
	Path& popDirectory(){
		_dirs.pop_back();
		return *this;
	}
	Path& deleteFile(){
		_name.clear();
		return *this;
	}
	Path& popFrontDirectory(){
		StringVec::iterator it = _dirs.begin();
		_dirs.erase(it);
		return *this;
	}
		
	Path& setFileName(const std::string& name);
		
	const std::string& getFileName() const{return _name;}

	Path& setBaseName(const std::string& name);

	std::string getBaseName() const;

	Path& setExtension(const std::string& extension);
	std::string getExtension() const;
	Path& clear();

	Path parent() const;
	Path absolute() const;
	Path absolute(const Path& base) const;
	static Path forDirectory(const std::string& path){
		Path p;
		return p.parseDirectory(path);
	}
	static Path forDirectory(const std::string& path, Style style){
		Path p;
		return p.parseDirectory(path, style);
	}
	static char separator(){
		return '/';
	}	
	static char pathSeparator(){
		return '/';
	}
	static std::string current();
		
	static std::string home();

	static std::string configHome();
		/// On Unix systems, this is the '~/.config/'. On Windows systems,
		/// this is '%APPDATA%'.

	static std::string dataHome();
		/// On Unix systems, this is the '~/.local/share/'. On Windows systems,
		/// this is '%APPDATA%'.

	static std::string tempHome();
		/// On Unix systems, this is the '~/.local/temp/'.

	static std::string cacheHome();
		/// On Unix systems, this is the '~/.cache/'. On Windows systems,
		/// this is '%APPDATA%'.

	static std::string temp();
	static std::string config();
		/// On Unix systems, this is the '/etc/'.
		
	static std::string null();		
	static std::string expand(const std::string& path);
		/// Expands all environment variables contained in the path.
		///
		/// On Unix, a tilde as first character in the path is
		/// replaced with the path to user's home directory.

	static void listRoots(std::vector<std::string>& roots);
	
		
	static bool find(StringVec::const_iterator it, StringVec::const_iterator end, const std::string& name, Path& path);
		/// Searches the file with the given name in the locations (paths) specified
		/// by it and end. A relative path may be given in name.
		///
		/// If the file is found in one of the locations, the complete
		/// path of the file is stored in the path given as argument and true is returned. 
		/// Otherwise false is returned and the path argument remains unchanged.

	static bool find(const std::string& pathList, const std::string& name, Path& path);
		/// Searches the file with the given name in the locations (paths) specified
		/// in pathList. The paths in pathList must be delimited by the platform's
		/// path separator (see pathSeparator()). A relative path may be given in name.
		///
		/// If the file is found in one of the locations, the complete
		/// path of the file is stored in the path given as argument and true is returned. 
		/// Otherwise false is returned and the path argument remains unchanged.
		
	static std::string transcode(const std::string& path);

protected:
	void parseUnix(const std::string& path);
	void parseWindows(const std::string& path);
	std::string buildUnix() const;
	std::string buildWindows() const;
private:
	std::string _node;
	std::string _device;
	std::string _name;
	StringVec   _dirs;
	bool        _absolute;
};


inline void swap(Path& p1, Path& p2)
{
	p1.swap(p2);
}


NAMESPACE_SUBFIX


#endif // Foundation_Path_INCLUDED
