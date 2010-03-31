#define LABELLENGTH 80 /* limit to length of file names and such */
#define MAX_INFO_FILES 10

#define KEYWORDLEN 32
#define N_PARENTS 4

typedef struct {
    char p[N_PARENTS][KEYWORDLEN];
    short n;
} parent_tp;

/* record of INFO */
typedef struct {
    char keyword[KEYWORDLEN];
    long start;
    short size;
    short seq;
    unsigned char narg,t[7];
    parent_tp parents;
    int directory;
} info_index_tp;

typedef struct {
    char keyword[KEYWORDLEN];
    long start;
    short size;
} learn_index_tp;

