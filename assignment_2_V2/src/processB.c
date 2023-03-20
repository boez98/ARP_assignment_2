#include "./../include/processB_utilities.h"
#include <bmpfile.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <semaphore.h>

#define SEM_PATH "/sem_CIRCLE"
#define width 1600	// width of the image (in pixels)
#define height 600 	// Height of the image (in pixels)
#define depth 4		// Depth of the image (1 for greyscale images, 4 for colored images)



// Function for painting the background to delete the old bitmap
void paint_background(bmpfile_t *bmp)
{

    // Data type for defining pixel colors (BGRA)
    rgb_pixel_t pixel = {0, 0, 0, 0}; // black

    // painting
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            bmp_set_pixel(bmp, i, j, pixel);
        }
    }
}


// Function to convert the matrix of 0 and 1 to a bitmap
void static_to_bmp(rgb_pixel_t *matrix, bmpfile_t *bmp)
{
    // Convert the matrix to a bitmap
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            // Get the pixel from the matrix
            rgb_pixel_t pixel = matrix[i + width * j];
            
            // Set the pixel in the bitmap
            bmp_set_pixel(bmp, i, j, pixel);
        }
    }
}

// Function to find center of the cirlce in the bitmap
void find_center(bmpfile_t *bmp, int *x, int *y)
{
    // Variable to store the length of the current circumference rope
    int length = 0;

    // Variable to store the length of the longest circumference rope
    int max_length = 0;

    
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            // Get the pixel at the specified (x,y) position
            // Data type for defining pixel
            rgb_pixel_t *pixel = bmp_get_pixel(bmp, i, j);

            // If the pixel is not black
            if (pixel->blue != 0 || pixel->green != 0 || pixel->red != 0)
            {
                // Increment the length of the current circumference rope
                length++;
            }
            else
            {
                // If the current circumference rope is longer than the longest one
                if (length > max_length)
                {
                    // Update the longest circumference rope
                    max_length = length;

                    // Update the center of the circle
                    *x = j / 20;
                    *y = (i - length / 2) / 20;
                }

                // Reset the length of the current circumference rope
                length = 0;
            }
        }
    }
}

int main(int argc, char const *argv[])
{

			/*    	BITMAP 		*/
		
    // Data structure for storing the bitmap file
    bmpfile_t *bmp;

    // Instantiate bitmap, passing three parameters
    bmp = bmp_create(width, height, depth);

    
    			 /*    	SHARED MEMORY 	    */ 

    // Shared memory name
    const char *shm_name = "/SHARED_CIRCLE";

    // Size in bytes of shared memory
    int SHM_SIZE = width * height * sizeof(rgb_pixel_t);

    // Shared memory file descriptor
    int shm_fd;

    // Open the shared memory object
    shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);

    // Map the shared memory object into the address space of the process
    rgb_pixel_t *ptr = (rgb_pixel_t *)mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    
    if (ptr == MAP_FAILED)
    {
        // Destroy the bitmap
        bmp_destroy(bmp);
        
        // Close the shared memory object
        shm_unlink(shm_name); 
        
        return 1;
    }

    // Utility variable to avoid trigger resize event on launch
    int first_resize = TRUE;

    // Initialize UI
    init_console_ui();

    // Variables to store the center of the circle
    int x, y;



    // Initialize the semaphore
    sem_t *sem_sh = sem_open(SEM_PATH, O_CREAT, S_IRUSR | S_IWUSR, 1);
    
    if (sem_sh == SEM_FAILED)
    {
        // Destroy the bitmap
        bmp_destroy(bmp);
        
        // Unmap the shared memory object
        munmap(ptr, SHM_SIZE);
        
        // Close the shared memory object
        shm_unlink(shm_name); 
        
        return 1;
    }

    

    // Infinite loop
    while (TRUE)
    {

        // Get input in non-blocking mode
        int cmd = getch();

        // If user resizes screen, re-draw UI...
        if (cmd == KEY_RESIZE)
        {
            if (first_resize)
            {
                first_resize = FALSE;
            }
            else
            {
                reset_console_ui();
            }
        }

        else
        {
            // Protect the shared memory with a semaphore
            sem_wait(sem_sh);

            // Paint the bitmap's background
            paint_background(bmp);

            // Convert the matrix to a bitmap
            static_to_bmp(ptr, bmp);

            // Release the semaphore
            sem_post(sem_sh);

            // Find the center of the circle
            find_center(bmp, &x, &y);

            mvaddch(x, y, 'x');
            refresh();
        }
    }
    
    // Free the bitmap
    bmp_destroy(bmp);

    // Unmap the shared memory object
    munmap(ptr, SHM_SIZE);
    
    // Close the shared memory object
    shm_unlink(shm_name);

    endwin();

    return 1;
}
