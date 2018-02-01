#include "Mapper.h"

#ifdef __linux__ 
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <dwarf.h>
    #include <libdwarf.h>
#elif _WIN32
    // windows code goes here
#else
    //unsupported platform
#endif

#include <iostream>


const int APPROX_ELEMENTS = 512;

class Mapper::MapperImpl
{
public:
    MapperImpl(std::string&& fileName):
    m_fileName(std::move(fileName))
	{
	}

    StrVector collectFunctions() const;
private:
#ifdef __linux__ 
    bool analyzeDie(Dwarf_Debug dgb, Dwarf_Die the_die, StrVector& funcVector) const;
#endif
    std::string m_fileName;
};

#ifdef __linux__
bool Mapper::MapperImpl::analyzeDie(Dwarf_Debug dgb, Dwarf_Die the_die, StrVector& funcVector) const
{
    char* die_name = 0;
    Dwarf_Error err;
    int rc = dwarf_diename(the_die, &die_name, &err);

    if (rc == DW_DLV_ERROR)
    {
        std::cout << "Error reading DWARF_diename - " << dwarf_errmsg(err) << std::endl;
        return false;
    }
    else if (rc == DW_DLV_NO_ENTRY)
        return true;

    Dwarf_Half tag;
    if (dwarf_tag(the_die, &tag, &err) != DW_DLV_OK)
    {
        std::cout << "Error reading dwarf_tag - " << dwarf_errmsg(err) << std::endl;
        return false;
    }

    if (tag == DW_TAG_subprogram)
    {
        funcVector.emplace_back(die_name);
    }

    return true;
}
#endif

Mapper::StrVector Mapper::MapperImpl::collectFunctions() const
{
    StrVector collectedFunc;
#ifdef __linux__ 
    int fd = open(m_fileName.c_str(), O_RDONLY);
    if (fd < 0) 
    {
        std::cout << "Failed to open file. Error is - " << errno << std::endl;
        return collectedFunc;
    }   

    Dwarf_Debug dbg = 0;
    Dwarf_Error err;
    auto ret = dwarf_init(fd, DW_DLC_READ, 0, 0, &dbg, &err); 
    if (ret != DW_DLV_OK) 
    {
        std::cout << "Failed DWARF initialization, return code - " << ret << " error - " << dwarf_errmsg(err) << std::endl;
        return collectedFunc;
    }

    Dwarf_Unsigned cu_header_length, abbrev_offset, next_cu_header;
    Dwarf_Half version_stamp, address_size;
    Dwarf_Die no_die = 0, cu_die, child_die;

    /* Find compilation unit header */
    if (dwarf_next_cu_header(
                dbg,
                &cu_header_length,
                &version_stamp,
                &abbrev_offset,
                &address_size,
                &next_cu_header,
                &err) == DW_DLV_ERROR)
    {
        std::cout << "Error reading DWARF cu header - " << dwarf_errmsg(err) << std::endl;
        return collectedFunc;
    }

    /* Expect the CU to have a single sibling - a DIE */
    if (dwarf_siblingof(dbg, no_die, &cu_die, &err) == DW_DLV_ERROR)
    {
        std::cout << "Error getting sibling of CU - " << dwarf_errmsg(err) << std::endl;
        return collectedFunc;
    }

    /* Expect the CU DIE to have children */
    if (dwarf_child(cu_die, &child_die, &err) == DW_DLV_ERROR)
    {
        std::cout << "Error getting child of CU DIE - " << dwarf_errmsg(err) << std::endl;
        return collectedFunc;
    }

    collectedFunc.reserve(APPROX_ELEMENTS);
    while (true)
    {
        if (!analyzeDie(dbg, child_die, collectedFunc))
            break;

        int rc = dwarf_siblingof(dbg, child_die, &child_die, &err);

        if (rc == DW_DLV_ERROR)
        {
            std::cout << "Error getting sibling of DIE - " << dwarf_errmsg(err) << std::endl;
            break;
        }
        else if (rc == DW_DLV_NO_ENTRY)
            break; /* done */
    }

#endif

    return collectedFunc;
}

Mapper::Mapper(std::string&& fileName):
 	m_impl(new Mapper::MapperImpl(std::move(fileName)))
{
}

Mapper::~Mapper() = default;

bool Mapper::collectFunctions()
{
    m_collectedFunctions = m_impl->collectFunctions();
    return !m_collectedFunctions.empty();
}

void Mapper::print() const
{
    if (!m_collectedFunctions.empty())
    {
        std::cout << m_collectedFunctions.size() << " functions collected: \n";
        for (const auto& func: m_collectedFunctions)
            std::cout << func << std::endl;
    }
    else
    {
        std::cout << "No functions collected\n";
    }
}
