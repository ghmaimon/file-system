#include "../headers/fs.hpp"

using namespace std;

extern int token_count;
extern char** token;
extern unsigned int free_space;

int main(int argc, char const *argv[])
{
    init_fs();
    while (1)
    {
        int token_index  = 0;
        get_command();
        if (!strcmp(token[token_index], CREATEFS))
        {
            if (token_count < 2)printf("put error: File system name not provided.\n");
            else createfs(token[1]);
        }
        else if(!strcmp(token[token_index], OPEN)){
            if (token_count < 2) printf("open error: File name not provided.\n");
            else open(token[1]);
        }
        else if (!strcmp(token[token_index], QUIT)) exit(0);
        else if (!fs_exits) {
            printf("error: a file system must be created first.\n");
            continue;
        }
        else if (!strcmp(token[token_index], PUT))
        {
            if (token_count < 2)printf("put error: File name not provided.\n");
            else put(token[1]);
        }
        else if(!strcmp(token[token_index], GET)){
            if (token_count < 2) printf("get error: File name not provided.\n");
            else if(token_count < 3) get(token[1], NULL);
            else get(token[1], token[2]);
        }
        else if(!strcmp(token[token_index], DEL)){
            if (token_count < 2) printf("del error: File name not provided.\n");
            else del(token[1]);
        }
        else if(!strcmp(token[token_index], ATTRIB)){
            if (token_count < 3) printf("attrib error: Not enough argiments.\n");
            else attrib(token[2], token[1]);
        }
        else if(!strcmp(token[token_index], LIST))list();
        else if(!strcmp(token[token_index], CLOSE))close();
        else if(!strcmp(token[token_index], DF)) df();
        else if (!strcmp(token[token_index], SAVE)) save();
        else {
            printf("command not reconized\n");
        }
    }
            return 0;
}