#include "./../include/processB_utilities.h"
#include <stdio.h>
#include <bmpfile.h>
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include<unistd.h>
#include<string.h>




int main(int argc, char const *argv[])
{
    
	

	void *shared_memory;   // x pos
	void *shared_memory2;  // y pos
	char buff[100];
	char buff2[100];
	int shmid, shmid2;
	int input_x;
	int input_y;

// Shared memory name
	shmid=shmget((key_t)2345, 1024, 0666);
	shmid2=shmget((key_t)5432, 1024, 0666);


	//process attached to shared memory segment
	shared_memory=shmat(shmid,NULL,0);
	shared_memory2=shmat(shmid2,NULL,0); 


	
	
	
    // Utility variable to avoid trigger resize event on launch
    int first_resize = TRUE;

    // Initialize UI
    init_console_ui();

    // Infinite loop
    while (TRUE) {
	
	
        // Get input in non-blocking mode
        int cmd = getch();
        
	
            
        // If user resizes screen, re-draw UI...
        if(cmd == KEY_RESIZE) {
            if(first_resize) {
                first_resize = FALSE;
            }
            else {
                reset_console_ui();
            }
        }

        else {
            mvaddch(LINES/2, COLS/2, '0');
            refresh();
        }
        
        
        
	// convert char to int
	input_x = atoi((char *)shared_memory );
	input_y = atoi((char *)shared_memory2 );
	
	
        // Move circle position
        mvaddch(input_x, input_y, 'x');
        
}
    
    
    
    endwin();
    return 0;
}
