/* 
 * Get address from symbol (libdwarf version)
 * Based on code by : Eli Bendersky (http://eli.thegreenplace.net) 
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <dwarf.h>
#include <libdwarf.h>


void die(char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    exit(EXIT_FAILURE);
}

/* Find symbols from the DIE and check if its the one we want */
Dwarf_Die find_symbol_in_die(Dwarf_Debug dgb, Dwarf_Die the_die, char* sym){

    char* die_name = 0;
    const char* tag_name = 0;
    Dwarf_Error err;
    Dwarf_Half tag;
    int rc = dwarf_diename(the_die, &die_name, &err);

    if (rc == DW_DLV_ERROR)
        die("Error in dwarf_diename\n");
    else if (rc == DW_DLV_NO_ENTRY)
        return 0;

    if (dwarf_tag(the_die, &tag, &err) != DW_DLV_OK)
        die("Error in dwarf_tag\n");

    /* Only interested in subprogram DIEs here */
    if (tag != DW_TAG_subprogram)
        printf("We ony want functions for now!\n");
        return 0;

    if (dwarf_get_TAG_name(tag, &tag_name) != DW_DLV_OK)
        die("Error in dwarf_get_TAG_name\n");

    if (!strcmp(die_name, sym)){
        printf("Found DW_TAG_subprogram: '%s'\n", die_name);
        return the_die;
    }
    return 0;
}

/* Get the addr of the symbol we want */
unsigned int get_symbol_addr(Dwarf_Debug dgb, Dwarf_Die the_die)
{
    Dwarf_Error err;
    Dwarf_Attribute* attrs;
    Dwarf_Addr lowpc, highpc;
    Dwarf_Signed attrcount, i;

    if (dwarf_attrlist(the_die, &attrs, &attrcount, &err) != DW_DLV_OK)
        die("Error in dwarf_attlist\n");

    for (i = 0; i < attrcount; ++i) {
        Dwarf_Half attrcode;
        if (dwarf_whatattr(attrs[i], &attrcode, &err) != DW_DLV_OK)
            die("Error in dwarf_whatattr\n");

        /* Take lowpc (function entry) */
        if (attrcode == DW_AT_low_pc)
            dwarf_formaddr(attrs[i], &lowpc, 0);
        /* Take highpc just for fun!*/
        else if (attrcode == DW_AT_high_pc)
            dwarf_formaddr(attrs[i], &highpc, 0);
    }

    return lowpc;
}


/* List all the functions from the file represented by the given descriptor. */
void list_sym_in_file(Dwarf_Debug dbg, char *sym)
{
    Dwarf_Unsigned cu_header_length, abbrev_offset, next_cu_header;
    Dwarf_Half version_stamp, address_size;
    Dwarf_Error err;
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
        die("Error reading DWARF cu header\n");

    /* Expect the CU to have a single sibling - a DIE */
    if (dwarf_siblingof(dbg, no_die, &cu_die, &err) == DW_DLV_ERROR)
        die("Error getting sibling of CU\n");

    /* Expect the CU DIE to have children */
    if (dwarf_child(cu_die, &child_die, &err) == DW_DLV_ERROR)
        die("Error getting child of CU DIE\n");


    Dwarf_Die symbol_die;
    Dwarf_Addr sym_addr;
    unsigned int found = 0; /* we set this flag to check if we got the sym */ 

    /* Now go over all children DIEs */
    while (1) {
        int rc;

        symbol_die = find_symbol_in_die(dbg, child_die, sym);
        if (symbol_die){
            sym_addr = get_symbol_addr(dbg, symbol_die);
            printf("Symbol address is : 0x%08llx\n", sym_addr);
            found = 1;
        }

        rc = dwarf_siblingof(dbg, child_die, &child_die, &err);

        if (rc == DW_DLV_ERROR)
            die("Error getting sibling of DIE\n");
        else if (rc == DW_DLV_NO_ENTRY)
            break; /* done */
    }
    if (!found){
        printf("Symbol not found!\n");
        exit(EXIT_FAILURE);
    }
}


int mainForMapper(int argc, char* argv[])
{
    Dwarf_Debug dbg = 0;
    Dwarf_Error err;
    const char* progname;
    int fd = -1;

    if (argc < 2) {
        fprintf(stderr, "Expected a program name as argument\n");
        return 1;
    }

    progname = argv[1];
    if ((fd = open(progname, O_RDONLY)) < 0) {
        perror("open");
        return 1;
    }

    if (dwarf_init(fd, DW_DLC_READ, 0, 0, &dbg, &err) != DW_DLV_OK) {
        fprintf(stderr, "Failed DWARF initialization\n");
        return 1;
    }

    list_sym_in_file(dbg, argv[2]);

    if (dwarf_finish(dbg, &err) != DW_DLV_OK) {
        fprintf(stderr, "Failed DWARF finalization\n");
        return 1;
    }

    close(fd);
    return 0;
}
