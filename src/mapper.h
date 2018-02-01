#include <memory>
#include <string>
#include <vector>

 class Mapper
 {
 	typedef std::vector<std::string> StrVector;
 	class MapperImpl;
 
 public:
 	Mapper(std::string&& fileName):
 	m_impl(new MapperImpl(std::move(filename)))
 	{

 	}
 	bool collectFunctions();
 	void print() const; //if filename is passed(save to filename)

private:
	class MapperImpl;
	std::unique_ptr<MapperImpl> m_impl;

	std::vector<std::string> m_collectedFunctions;
};






