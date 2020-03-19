#include "file_opt.hxx"
#include "path_opt.hxx"
#include "directoryiterator.hxx"

#include <thread>
#include <chrono>


NAMESPACE_PREFIX

File::File(const Path& path): FileImpl(path.toString())
{
}


File& File::operator = (const Path& path)
{
	setPathImpl(path.toString());
	return *this;
}



void File::copyTo(const std::string& path) const
{
	Path src(getPathImpl());
	Path dest(path);
	File destFile(path);
	if ((destFile.exists() && destFile.isDirectory()) || dest.isDirectory())
	{
		dest.makeDirectory();
		dest.setFileName(src.getFileName());
	}
	if (isDirectory())
		copyDirectory(dest.toString());
	else
		copyToImpl(dest.toString());
}


void File::copyDirectory(const std::string& path) const
{
	File target(path);
	target.createDirectories();

	Path src(getPathImpl());
	src.makeFile();
	DirectoryIterator it(src);
	DirectoryIterator end;
	for (; it != end; ++it)
	{
		it->copyTo(path);
	}
}


void File::remove(bool recursive)
{
	if (recursive && !isLink() && isDirectory())
	{
		std::vector<File> files;
		list(files);
		for (std::vector<File>::iterator it = files.begin(); it != files.end(); ++it)
		{
			it->remove(true);
		}

		// Note: On Windows, removing a directory may not succeed at first
		// try because deleting files is not a synchronous operation. Files
		// are merely marked as deleted, and actually removed at a later time.
		//
		// An alternate strategy would be moving files to a different directory
		// first (on the same drive, but outside the deleted tree), and marking
		// them as hidden, before deleting them, but this could lead to other issues.
		// So we simply retry after some time until we succeed, or give up.

		int retry = 8;
		long sleep = 10;
		while (retry > 0)
		{
			try
			{
				removeImpl();
				retry = 0;
			}
			catch (DirectoryNotEmptyException&)
			{
				if (--retry == 0) throw;
				std::this_thread::sleep_for(std::chrono::seconds(sleep));
				sleep *= 2;
			}
		}
	}
	else
	{
		removeImpl();
	}
}


bool File::createFile()
{
	return createFileImpl();
}


bool File::createDirectory()
{
	return createDirectoryImpl();
}


void File::createDirectories()
{
	if (!exists())
	{
		Path p(getPathImpl());
		p.makeDirectory();
		if (p.depth() > 1)
		{
			p.makeParent();
			File f(p);
			f.createDirectories();
		}
		try
		{
			createDirectoryImpl();
		}
		catch (FileExistsException&)
		{
		}
	}
}


void File::list(std::vector<std::string>& files) const
{
	files.clear();
	DirectoryIterator it(*this);
	DirectoryIterator end;
	while (it != end)
	{
		files.push_back(it.name());
		++it;
	}
}


File::FileSize File::totalSpace() const
{
	return totalSpaceImpl();
}


File::FileSize File::usableSpace() const
{
	return usableSpaceImpl();
}


File::FileSize File::freeSpace() const
{
	return freeSpaceImpl();
}


void File::list(std::vector<File>& files) const
{
	files.clear();
	DirectoryIterator it(*this);
	DirectoryIterator end;
	while (it != end)
	{
		files.push_back(*it);
		++it;
	}
}


void File::handleLastError(const std::string& path)
{
	handleLastErrorImpl(path);
}


NAMESPACE_SUBFIX
