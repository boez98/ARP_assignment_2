#include "./../include/processA_utilities.h"
#include <stdio.h>
#include <bmpfile.h>
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include<unistd.h>
#include<string.h>



#define width 1600	// width of the image (in pixels)
#define height 600 	// Height of the image (in pixels)
#define depth 4		// Depth of the image (1 for greyscale images, 4 for colored images)




// Function to draw the bmp circle
void draw_bmp(bmpfile_t *bmp, int x_pos, int y_pos)

{
	// Data type for defining pixel colors (BGRA)
  	rgb_pixel_t pixel = {255, 0, 0, 0}; // Blue

	// Code for drawing a centered circle of given radius...
  	int radius = 50;
  	for(int x = -radius; x <= radius; x++) {
    		
		for(int y = -radius; y <= radius; y++) {
      			
			// If distance is smaller, point is within the circle
      			if(sqrt(x*x + y*y) < radius) {
          		
			// Color the pixel at the specified (x,y) position moltiply times 20  with the given pixel values
         		 bmp_set_pixel(bmp,  x_pos + x,  y_pos + y, pixel);

      }
    }
  }
}


// Function for painting the background to delete the old bitmap
void paint_background(bmpfile_t *bmp)
{

    // Data type for defining pixel colors (BGRA)
    rgb_pixel_t pixel = {255, 255, 255, 0};  // White

    // painting
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            bmp_set_pixel(bmp, i, j, pixel);
        }
    }
}


int main(int argc, char *argv[])
{
	int input_x;
	int input_y;

	void *shared_memory;
	void *shared_memory2;
	char buff[100];
	char buff2[100];
	int shmid, shmid2;
	
// Shared memory name: shared_memory manage x position while shared_memory2 y position

	//creates shared memory segment with key 2345, having size 1024 bytes. IPC_CREAT is used to create the shared segment if it does not exist. 0666 are the permisions on the shared segment
	shmid=shmget((key_t)2345, 1024, 0666|IPC_CREAT); // x pos
	// the same but with key 5432  
	shmid2=shmget((key_t)5432, 1024, 0666|IPC_CREAT); // y pos



	//process attached to shared memory segment
	shared_memory=shmat(shmid,NULL,0); 
	shared_memory2=shmat(shmid2,NULL,0); 

	    
	
	    // convert int to chart
	    sprintf(buff,"%d", input_x);
	    sprintf(buff2,"%d", input_y);
	    
	    // Write into the memory segment
	    strcpy(shared_memory,buff); //x pos written to shared memory
	    
	    strcpy(shared_memory2,buff2); //y pos written to shared memory
    	
    	
    	
    		

	// Data structure for storing the bitmap file
 	bmpfile_t *bmp;

	// Instantiate bitmap, passing three parameters
	bmp = bmp_create(width, height, depth);




    // Utility variable to avoid trigger resize event on launch
    int first_resize = TRUE;

    // Initialize UI
    init_console_ui();

	// start position to be in the middle of the image
	int start_x = width/2;
	int start_y = height/2;
	
	// Draw the circle 
    	draw_bmp(bmp, start_x, start_y);



    // Infinite loop
    while (TRUE)
    {
        // Write into the memory segment
	strcpy(shared_memory,buff); //x pos written to shared memory 
        strcpy(shared_memory2,buff2); //y pos written to shared memory
	    
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

        // Else, if user presses print button...
        else if(cmd == KEY_MOUSE) {
            if(getmouse(&event) == OK) {
                if(check_button_pressed(print_btn, &event)) {

			// SNAPSHOT: Save image as .bmp file
  			bmp_save(bmp, "out/circle.bmp");


                    mvprintw(LINES - 1, 1, "Print button pressed");
                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS - BTN_SIZE_X - 2; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                }
            }
        }

        // If input is an arrow key, move circle accordingly...
        else if(cmd == KEY_LEFT || cmd == KEY_RIGHT || cmd == KEY_UP || cmd == KEY_DOWN) {
            move_circle(cmd);
            draw_circle();

	    // paint background
	    paint_background(bmp);

            // Draw the circle
            draw_bmp(bmp,  20*circle.x, 20*circle.y);
	    
	    
	    // convert int to chart
	    sprintf(buff,"%d", circle.y);
	    sprintf(buff2,"%d", circle.x);
        }
    }
    
	// Free resources before termination
 	bmp_destroy(bmp);

    endwin();
    return 0;
}
