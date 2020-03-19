#ifndef __SQHAO_DIRECTORYITERATOR__HXX__
#define __SQHAO_DIRECTORYITERATOR__HXX__

#include "file_opt.hxx"
#include "path_opt.hxx"
#include "default_config.hxx"

NAMESPACE_PREFIX
typedef unsigned short int UInt16;
class DirectoryIteratorImpl;
class DirectoryIterator
{
public:
	DirectoryIterator(): _pImpl(0){}
	DirectoryIterator(const std::string& path);
		/// Creates a directory iterator for the given path.

	DirectoryIterator(const DirectoryIterator& iterator);
	DirectoryIterator(const File& file);
	DirectoryIterator(const Path& path);
	virtual ~DirectoryIterator();
		/// Destroys the DirectoryIterator.

	const std::string& name() const{
		return _path.getFileName();
	}
		
	const Path& path() const{
		return _path;
	}
		/// Returns the current path.

	DirectoryIterator& operator = (const DirectoryIterator& it);
	DirectoryIterator& operator = (const File& file);
	DirectoryIterator& operator = (const Path& path);
	DirectoryIterator& operator = (const std::string& path);
	
	virtual DirectoryIterator& operator ++ ();   // prefix
	
	//@ deprecated
	DirectoryIterator operator ++ (int); // postfix
	
	const File& operator * () const{return _file;}
	File& operator * (){return _file;}
	const File* operator -> () const{return &_file;}
	File* operator -> (){return &_file;}
	
	bool operator == (const DirectoryIterator& iterator) const{
		return name() == iterator.name();
	}
	bool operator != (const DirectoryIterator& iterator) const{
		return name() != iterator.name();
	}

protected:
	Path _path;
	File _file;

private:
	DirectoryIteratorImpl* _pImpl;
};

NAMESPACE_SUBFIX


#endif // __SQHAO_DIRECTORYITERATOR__HXX__
