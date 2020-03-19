#include "configoperation.hxx"  

NAMESPACE_PREFIX

	using std::string;
	using std::ostream;
	using std::istream;
	
	CConfig::CConfig()
	{
		m_Delimiter = string(1,'=');
		m_Comment = string(1,'#');
	}

	std::ostream& operator<<(std::ostream& os, const CConfig& cf)
	{
		for (CConfig::mapci p = cf.m_Contents.begin(); p != cf.m_Contents.end(); ++p) {
			os << p->first << " " << cf.m_Delimiter << " ";
			os << p->second << std::endl;
		}
		return os;
	}

	std::istream& operator>>(std::istream& is, CConfig& cf)
	{
		typedef string::size_type pos;
		const string& delim = cf.m_Delimiter;
		const string& comm = cf.m_Comment;
		const pos skip = delim.length();

		string nextline = "";

		while (is || nextline.length() > 0) {
			string line;
			if (nextline.length() > 0) {
				line = nextline;
				nextline = "";
			}
			else {
				std::getline(is, line);
			}
			//去掉注释  
			line = line.substr(0, line.find(comm));
			pos delimPos = line.find(delim);
			if (delimPos < string::npos) {
				//提取key 
				string key = line.substr(0, delimPos);
				line.replace(0, delimPos + skip, "");

				// value 是否在下行，如果空白行，有key value，或者os end，停止
				bool terminate = false;
				while (!terminate && is) {
					std::getline(is, nextline);
					terminate = true;

					string nlcopy = nextline;
					trimInPlace(nlcopy);
					// 下行没有内容
					if (nlcopy == "")
						continue;
					//去注释
					nextline = nextline.substr(0, nextline.find(comm));
					//没有key value
					if (nextline.find(delim) != string::npos)
						continue;
					nlcopy = nextline;
					trimInPlace(nlcopy);
					// 有value内容，先换行
					if (nlcopy != "")
						line += "\n";
					line += nextline;
					terminate = false;
					nextline = "";
				}

				// Store key and value  
				trimInPlace(key);
				trimInPlace(line);

				cf.m_Contents[key] = line;
			}
		}
		return is;
	}
	bool CConfig::FileExist(std::string filename)
	{
		bool exist = false;
		std::ifstream in(filename.c_str());
		if (in)
			exist = true;
		return exist;
	}

	void CConfig::ReadFile(string filename, string delimiter, string comment)
	{
		m_Delimiter = delimiter;
		m_Comment = comment;
		std::ifstream in(filename.c_str());

		if (!in) throw File_not_found(filename);

		in >> (*this);
	}
NAMESPACE_SUBFIX
