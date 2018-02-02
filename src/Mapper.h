#include <memory>
#include <string>
#include <vector>
#include <ostream>

 class Mapper
 {
 	typedef std::vector<std::string> StrVector;
public:
 	Mapper(std::string&& fileName);
	~Mapper();
 	bool collectFunctions();
 	void print(std::string&& fileToSave = "") const; //if filename is passed(save to filename)
private:
	void printToStream(std::ostream& output) const;
private:
	class MapperImpl;
	std::unique_ptr<MapperImpl> m_impl;

	std::vector<std::string> m_collectedFunctions;
};

