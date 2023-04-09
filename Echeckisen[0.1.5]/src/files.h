#ifndef FILES_H
#define FILES_H




// ---------------- DEFINITIONS ----------------

//error codes
#define FILES__DATA_IS_NULL -1
#define FILES__PATH_IS_NULL -2
#define FILES__NO_FILE_FOUND -3
#define FILES__WRITING_ERROR -4
#define FILES__SUCCESS 0




// ---------------- READ / WRITE ----------------
int file_read(char* path, char** data, size_t* length); // reads from file #path#
                                                        // output data into #data#
                                                        // and data length into #length#
                                                        // WARNING ! Value inside data should not refer to anything.
                                                        //                                      (can refer to NULL)
int file_write(char* path, char** data, size_t* length); // writes into file #path#
                                                         // data from #data#
                                                         // from index 0 to #length#
                                                         // WARNING ! Value inside data will not be free
int file_append(char* path, char** data, size_t* length); // same warnings as file_append()




#endif
