1-Refactoring of Metadata, because i'm trying to calculate the free space value multiple times in different places

need something more like:

typedef struct{
    Metadata* next;
    Blk_md;
}Metadata;

typedef struct{
    int alloc_nums;
    size_t frsp;
    Alloc_md amd [];
}Blk_md;
