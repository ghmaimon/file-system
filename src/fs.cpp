#include "../headers/fs.hpp"
 

int token_count;
char** token;
unsigned int free_space;
char directory[128][32];
int block_map[128];
unsigned char data_blocks[NUM_BLOCKS][BLOCK_SIZE];
bool block_oc[NUM_BLOCKS];
FILE* file_read;
int    status;
struct stat buf;
int number_files;
int number_hidden_files;
char fsname[32];
bool fs_exits;

// funcitions implementation ***********

void init_fs(){
    
    free_space = BLOCK_SIZE * (NUM_BLOCKS - 131);
    number_files = 0;
    number_hidden_files = 0;
    fs_exits = false;
    for (int i = 0; i < 128; i++)
    {
        block_map[i] = -1;
    }
    for (int i = 0; i < 132; i++)
    {
        block_oc[i] = true;
    }
    
    for (int i = 132; i < NUM_BLOCKS; i++)
    {
        block_oc[i] = false;
    }
    
}

bool check_filename(char* filename) {
    // before we start reading the file we need to make sure that the filename is valide.
    if (strlen(filename) > MAX_FILE_NAME_SIZE)
    {
        return false;
    }
    return true;
}

void get_command()
{
    char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
    // Print out the mfs prompt
    printf ("mfs> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );

    /* Parse input */
    token = (char**)malloc(MAX_NUM_ARGUMENTS * sizeof(char*));

    token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *arg_ptr;                                         
                                                           
    char *working_str  = strdup( cmd_str );                

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input stringswith whitespace used as the delimiter
    while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    // Now print the tokenized input as a debug check
    // \TODO Remove this code and replace with your shell functionality
    
    free( working_root );
}

// this function is used to find the first empty place 
// between the 128 free inode blocks.
int map_files(){
    
    for (int i = 0; i < 128; i++)
    {
        if (directory[i] == NULL || strcmp(directory[i], "") == 0) // found a free place
        {
            return i;
        }
    }
    return -1;
}

// this checks if a file with a given name exits in our file system
bool file_exits(char* filename){

    for (int i = 0; i < 128; i++)
    {
        if (directory[block_map[i]] != NULL 
        && strcmp(directory[block_map[i]], filename) == 0) // found it
        {
            return true;
        }
    }
    return false;
}

// this function allows the user to put a new file into the file system. 
int put(char* filename){
    // we need first to check if there is place to add another file.
    if (number_files >= MAX_FILES_NUMBER)
    {
        printf("put error: Max number of files reaced\n");
        return -1;
    }
    // next we check if a file already exits with the given name.
    if (file_exits(filename))
    {
        printf("error put : File name already exits\n");
        return -1;
    }
    // next check if the file name is valide.
    if (check_filename(filename))
    {
        // next we start the creation of the file in the file system
        status =  stat( filename, &buf );
        if( status != -1 ) {       
            srand((unsigned) time(0)); // this to use the rand() function
            FILE *ifp = fopen ( filename, "r" ); // open the file in read mode
            int copy_size   = buf . st_size; // get the size of the file
            int offset      = 0;
            int block_num = 0; // this si the counter that will calculate the
            // number of blocks consumed.
            inode ind; // create a new inode for the file
            ind.attrib_r = ind.attrib_h = false; // all attribute don't exits 
            // for this new file
            ind.size = copy_size; // save the size in the inode.
            if (copy_size <= free_space){ // check if there is free space for the file
                while( copy_size > 0 ){ // we will keep writing until the copy size reaches 0
                    fseek( ifp, offset, SEEK_SET);
                    int result;
                    do
                    {
                        result = (rand() % NUM_BLOCKS);
                    } while (block_oc[result] == true); // we will keep searching until we find a empty block
                    int to_read;
                    if (BLOCK_SIZE < copy_size) to_read = BLOCK_SIZE;
                    else to_read = copy_size; // the last read_size is less than block size
                    int bytes = fread(data_blocks[result], BLOCK_SIZE, 1, ifp ); // read the data block
                    if( bytes == 0 && !feof( ifp ) ) // in this cas we have not reached the end of file
                    // but we can't read anything so there is clearly something wrong.
                    {
                        printf("An error occured reading from the input file.\n");
                        return -1;
                    }
                    ind.block_list[block_num] = result; // we add the block to the block list in the inode of the file
                    block_oc[result] = true; // indecate that the block is occupied in the block free map.
                    clearerr( ifp );
                    copy_size -= BLOCK_SIZE; // reduce copy size by the already copied size
                    offset    += BLOCK_SIZE;
                    block_num ++;

                }
                free_space -= block_num * BLOCK_SIZE; // we need to reduce the free space by the size of the file.
                ind.number = map_files(); // here we search for a empty slot in the inode map table.
                block_map[number_files] = ind.number; // save the number of the inode in the map
                strcpy(directory[block_map[number_files]], filename); // save the file in the directory
                ind.add_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                // here we saved the current date as the adding date
                number_files ++; // increase number of files
                memcpy((void*)data_blocks[ind.number],(void*) &ind, BLOCK_SIZE); // in the end we need 
                // to save the inode of the file in a block in the data block
            }
            
            fclose( ifp );
        }
        else
        {
            printf("put error: File not found.\n");
        }
        
    }
    else {
            printf("put error : File name too long.\n");
    }
    return 0;
}

//this function displays the amount of free space in the
//file system in bytes. 
void df(){
    printf("%d bytes free.\n", free_space);
}

//this function displays all the files in the file system,
//their size in bytes and the time they were added to the file system 
void list(){
    // first we check if there is any files to display
    if (number_files == 0 || number_files == number_hidden_files) 
    {
        printf("list: No files found.\n");
        return;
    }
    for (int i = 0; i < 128; i++) // iterate in all the files
    {
        if (block_map[i] != -1){
            inode ind;
            memcpy((void*) &ind, (void*)data_blocks[block_map[i]], BLOCK_SIZE); // get the file inode
            if (ind.attrib_h == true){ // check if it is hidden and ignore it
                continue;
            }
            struct tm * timeinfo;
            char buffer [80];
            timeinfo = localtime (&ind.add_time);
            strftime (buffer, 80,"%b %d %H:%M",timeinfo); // here we change the date format
            std::cout << ind.size << " " << buffer << " " << directory[block_map[i]] << std::endl;
            memcpy((void*)data_blocks[block_map[i]], (void*) &ind, BLOCK_SIZE); // get the file inode
        }
    }
}

// this function allows the user to retrieve a file from the file system and
// place it in the current working directory.
int get(char* filename, char* new_filename){
    for (int i = 0; i < 128; i++)
    {
        if (directory[block_map[i]] != NULL && strcmp(directory[block_map[i]], filename) == 0) // this is to find the file from the directory.
        {
            inode ind;
            memcpy((void*) &ind, (void*)data_blocks[block_map[i]], BLOCK_SIZE); // we retrieve the inode from the data block where it is stored.
            FILE *ifp;
            if (new_filename == NULL) // here we check if there is a new file name in which we copy our file or keep the name as it is.
            {
                ifp = fopen ( filename, "w" );
            }
            else{
                ifp = fopen ( new_filename, "w" );
            }
            
            int copy_size = ind.size;
            int num_blocks = ceil((double)copy_size / (double) BLOCK_SIZE);
            int to_write = BLOCK_SIZE;
            for (int j = 0; j < num_blocks; j++)
            {
                if (j == num_blocks - 1)
                {
                    // we need to get the size of the last chunk of the file (which is generally less than the block size)
                    to_write = copy_size - (num_blocks - 1); 
                }
                int bytes = fwrite(data_blocks[ind.block_list[j]], to_write, 1, ifp);
            }
            fclose(ifp);
            return 0;
        }
    }
    return 0;
}

//this funciton allow the user to delete a file from thefile system 
int del(char* filename){
    for (int i = 0; i < 128; i++)
    {
        if (strcmp(directory[block_map[i]], filename) == 0) // this is to find the file from the directory.
        {
            inode ind;
            memcpy((void*) &ind, (void*)data_blocks[block_map[i]], BLOCK_SIZE); // we retrieve the inode from the data block where it is stored.
            if (ind.attrib_r) { // if the file is read only we can not delete it.
                printf("del: this file is for read only.\n");
                return 0;
            }
            int num_blocks = ceil((double)ind.size / (double) BLOCK_SIZE); // get the number of blocks occupied by the file.
            free_space += num_blocks * BLOCK_SIZE;
            for (int j = 0; j < num_blocks; j++)// we start to free the blocks one after one.
            {
                block_oc[ind.block_list[j]] = false; // here we indecate in the free block map that this block is free for use.   
            }
            number_files -= 1; // reduce the number of files by one.
            block_map[i] = -1;
            return 0;
        }
    }
    printf("del: File not found.\n");
    return 0;
}

// this function sets or removes an attribute from a file.
int attrib(char* filename, char* attribute){
    for (int i = 0; i < 128; i++)
    {
        if (directory[block_map[i]] != NULL && strcmp(directory[block_map[i]], filename) == 0) // first we need to find our file in the directory
        {
            inode ind;
            memcpy((void*) &ind, (void*)data_blocks[block_map[i]], BLOCK_SIZE); // retrieve the inode of the file.
            // after that we check for the given attribute and assigne it to the file.
            if (strcmp(attribute, PLUS_H) == 0) 
            {
                if (!ind.attrib_h) number_hidden_files++;
                ind.attrib_h = true;
            }
            else if (strcmp(attribute, MINUS_H) == 0)
            {
                if (ind.attrib_h) number_hidden_files--;
                ind.attrib_h = false;
            }
            if (strcmp(attribute, PLUS_R) == 0)
            {
                ind.attrib_r = true;
            }
            else if (strcmp(attribute, MINUS_R) == 0)
            {
                ind.attrib_r = false;
            }
            memcpy((void*)data_blocks[ind.number],(void*) &ind, BLOCK_SIZE); // we need to save the new inode.
            return 0;
        }
    }
    printf("attrib: File not found.\n");
    return 0;
}

// this function creates a new file system
void createfs(char* name){
    if (fs_exits) // in case a file system is already in use we can't create another one.
    {
        printf("createfs error: must close current file system first.\n");
        return;
    }
    fs_exits = true;
    strcpy(fsname, name);
}


// this function closes the currently opened file system
void close(){
    for (int i = 0; i < number_files; i++)
    {
        if (block_map[i] != -1) // we search in the directory for all files
        {
            inode ind;
            memcpy((void*) &ind, (void*)data_blocks[block_map[i]], BLOCK_SIZE); // we retrieve the inode of the file
            int num_blocks = ceil((double)ind.size / (double) BLOCK_SIZE); // get the number of blocks occupied by the file.
            free_space += num_blocks * BLOCK_SIZE; // we need to add the freed space to our total free space
            for (int j = 0; j < num_blocks; j++) // here we free all the data blocks
            {
                block_oc[ind.block_list[j]] = false;  // we need to change the data block's status in the free block map.         
            }
            block_map[i] = -1; // change the number that the maps to the file in the block map to -1 (does not exit)
        }
    }
    init_fs(); // we need to reinitialize the application to make sure all is setup for forware use.
}

// this function writes the file system in the disk
void save(){
    // first we will create the path to the directory where all file systems are stored.
    char filepath[50];
    strcpy(filepath, "file_systems/");
    strcat(filepath, fsname);
    strcat(filepath, ".fs"); // all file system images will end with .fs.
    FILE* fs = fopen(filepath, "w"); // open the file with write option.
    fwrite(&number_files, sizeof(number_files), 1, fs); // first write the number of files in the file system.
    fwrite(&number_hidden_files, sizeof(number_hidden_files), 1, fs); // write the number of hidden files in the file system.
    fwrite(&free_space, sizeof(free_space), 1, fs);
    memcpy((void*)data_blocks[0],(void*) directory, BLOCK_SIZE); // write the directory in the block 0.
    memcpy((void*)data_blocks[1],(void*) block_map, sizeof(block_map)); // write the inode map in the block 1.
    memcpy((void*)data_blocks[2],(void*) block_oc, NUM_BLOCKS); // write the free block map in the block 2.
    for (int i = 0; i < NUM_BLOCKS; i++)
    {
        fwrite(data_blocks[i], BLOCK_SIZE, 1, fs); 
    }   // write all blocks of the file system to the image.*/
    fclose(fs); // close the file in the end.*/
    memcpy((void*) directory, (void*)data_blocks[0], BLOCK_SIZE); // read the directory from the block 0.
    memcpy((void*) block_map, (void*)data_blocks[1], sizeof(block_map)); // read the inode map from the block 1.
    memcpy((void*) block_oc, (void*)data_blocks[2], NUM_BLOCKS); // read the free block map from the block 2.*/
}

// this function opens an already exiting file system image from the disk
void open(char* filename){
    // first we will create the path to the directory where all file systems are stored.
    char filepath[50];
    int offset = 0;
    strcpy(filepath, "file_systems/");
    strcat(filepath, filename);
    strcat(filepath, ".fs"); // all file system images will end with .fs.
    strcpy(fsname, filename);
    FILE* fs = fopen(filepath, "r"); // open the file with read option
    status =  stat( filepath, &buf );
    if( status == -1 ) {
        printf("open error: file system does not exit.\n");
        return;
    }
    fread(&number_files, sizeof(number_files), 1, fs); // first read the number of files in the file system.
    offset += sizeof(number_files);
    fseek(fs, offset, SEEK_SET);
    fread(&number_hidden_files, sizeof(number_hidden_files), 1, fs); // read the number of hidden files in the file system.
    offset += sizeof(number_hidden_files);
    fseek(fs, offset, SEEK_SET);
    fread(&free_space, sizeof(free_space), 1, fs);
    offset += sizeof(free_space);
    fseek(fs, offset, SEEK_SET);
    for (int i = 0; i < NUM_BLOCKS; i++)
    {
        fread(data_blocks[i], BLOCK_SIZE, 1, fs); 
        offset += BLOCK_SIZE;
        fseek(fs, offset, SEEK_SET);
    }  // read all blocks of the file system to the image.
    memcpy((void*) directory, (void*)data_blocks[0], BLOCK_SIZE); // read the directory from the block 0.
    memcpy((void*) block_map, (void*)data_blocks[1], 128 * sizeof(int)); // read the inode map from the block 1.
    memcpy((void*) block_oc, (void*)data_blocks[2], NUM_BLOCKS); // read the free block map from the block 2.*/
    //fclose(fs); // close the file in the end.*/
    fs_exits = true;
}