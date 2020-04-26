/*
 * RK8 generator 
 */


#include "appmake.h"

static char             *description  = NULL;
static char             *binname      = NULL;
static char             *outfile      = NULL;
static char             *crtfile      = NULL;
static int               origin       = -1;
static int               exec         = -1;
static char              help         = 0;



/* Options that are available for this module */
option_t rk8_options[] = {
    { 'h', "help",       "Display this help",                OPT_BOOL,  &help },
    { 'b', "binfile",    "Binary file to embed",             OPT_STR,   &binname },
    {  0 , "description","Description of the file",          OPT_STR,   &description},
    {  0 , "org",        "Origin of the embedded binary",    OPT_INT,   &origin },
    {  0 , "exec",       "Starting execution address",       OPT_INT,   &exec },
    { 'c', "crt0file",   "crt0 used to link binary",         OPT_STR,   &crtfile },
    { 'o', "output",     "Name of output file",              OPT_STR,   &outfile },
    {  0,   NULL,        NULL,                               OPT_NONE,  NULL }
};




/*
 * Execution starts here
 */

int rk8_exec(char *target)
{
    char    filename[FILENAME_MAX+1];
    struct  stat binname_sb;
    FILE   *fpin;
    FILE   *fpout;
    int     i,c;
    
    if ( help )
        return -1;

    if ( binname == NULL ) {
        return -1;
    }

    if ( outfile == NULL ) {
        strcpy(filename,binname);
        suffix_change(filename,".rk8");
    } else {
        strcpy(filename,outfile);
    }

    if ( description == NULL ) {
         description = binname;
    }

    if ((origin == -1) && ((crtfile == NULL) || ((origin = get_org_addr(crtfile)) == -1))) {
        origin = 0;
    }
    if ( exec == -1 ) {
        exec = origin;
    }
    
    if ( stat(binname, &binname_sb) < 0 ||
         ( (fpin=fopen_bin(binname, NULL) ) == NULL )) {
        exit_log(1,"Can't open input file %s\n",binname);
    }
    
    if ( ( fpout = fopen(binname, "rb")) == NULL ) {
        exit_log(1,"Can't open input file %s\n", binname);
    }
    
    if ( ( fpout = fopen(filename, "wb")) == NULL ) {
        exit_log(1,"Can't open output file %s\n", filename);
    }

    /* Header */
    // Load address
    writeword_be(origin, fpout);
    // End address
    i = binname_sb.st_size;
    if ( i % 2 ) i++;
    writeword_be(origin + i, fpout);


    uint8_t chkh = 0, chkl = 0;
    i = 0;
    while ( i < binname_sb.st_size ) {
        c = getc(fpin);
        writebyte_p(c, fpout, &chkh);
        c = getc(fpin);
        if ( feof(fpin) ) c = 0;
        writebyte_p(c, fpout, &chkl);
        i += 2;
    }
    writebyte(chkh,fpout);
    writebyte(chkl,fpout);
    
    fclose(fpin);
    fclose(fpout);
    
    return 0;
}



