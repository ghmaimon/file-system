#if !defined(FS)
#define FS

#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <string>
#include <sys/stat.h>
#include <chrono>
#include <ctime>  
#include <tgmath.h>

// constants ************************

#define NUM_BLOCKS 4226
#define MAX_FILE_SIZE 10240000 // BYTES
#define MAX_FILES_NUMBER 128
#define BLOCK_SIZE 8192
#define MAX_FILE_NAME_SIZE 32
#define NUM_BLOCKS_DIRECT 10
#define NUM_BLOCKS_INDIRECT BLOCK_SIZE / sizeof(int)

// **********************************

// structs and classes


typedef struct _inode
{
    int number;
    size_t size;
    time_t add_time;
    int block_list[2040];
    bool attrib_h;
    bool attrib_r;
} inode;


// commands *************************

#define PUT "put"
#define GET "get"
#define DEL "del"
#define LIST "list"
#define DF "df"
#define CREATEFS "createfs"
#define OPEN "open"
#define SAVE "savefs"
#define CLOSE "close"
#define ATTRIB "attrib"
#define PLUS_H "+h"
#define PLUS_R "+r"
#define MINUS_H "-h"
#define MINUS_R "-r"
#define QUIT "quit"

// **********************************


// function declarations ************
bool check_filename(char* filename);
bool check_file_space(FILE *file);
FILE* read_file(char* filename);
int put(char* filename);
int get(char* filename, char* new_filename);
int del(char* filename);
void list();
void df();
void open(char* fsname);
void close();
void save();
int attrib(char* filename, char* attribute);
void createfs(char* name);
void get_command();
void init_fs();


// **********************************

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports five arguments

extern int token_count;
extern char** token;
extern unsigned int free_space; // this variable will hold the free space size
extern char directory[128][32]; // this will be our directory
extern int block_map[128]; // this is the inode map
extern unsigned char data_blocks[NUM_BLOCKS][BLOCK_SIZE]; // the table of data blocks
extern bool block_oc[NUM_BLOCKS]; // block free map
extern FILE* file_read; 
extern int    status;
extern struct stat buf;
extern int number_files; // the number of files
extern int number_hidden_files; // the number of hidden files
extern bool fs_exits; // a boolean that indicates if there is an active file system
// if not one must create a file system first of all, or can open an existing one.
extern char fsname[32]; // the name of the currently active file system
#endif // FS
