#include "./../include/processA_utilities.h"
#include <bmpfile.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <semaphore.h>

#define SEM_PATH "/sem_CIRCLE"
#define width 1600	// width of the image (in pixels)
#define height 600 	// Height of the image (in pixels)
#define depth 4		// Depth of the image (1 for greyscale images, 4 for colored images)


// Function to draw the bmp circle
void draw_bmp(bmpfile_t *bmp, int x, int y)
{

    // Data type for defining pixel colors (BGRA)
    rgb_pixel_t pixel = {255, 0, 0, 0}; // Blue

    // Radius of the circle
    int radius = 50;

    // Draw the circle
    for (int i = -radius; i <= radius; i++)
    {
        for (int j = -radius; j <= radius; j++)
        {
            // If distance is smaller, point is within the circle
            if (sqrt(i * i + j * j) < radius)
            {
                // Color the pixel at the specified (x,y) position moltiply times 20  with the given pixel values
                bmp_set_pixel(bmp, x * 20 + i, y * 20 + j, pixel);
            }
        }
    }
}

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

// Function to convert the bitmap to a matrix of 0 and 1
void bmp_to_static(bmpfile_t *bmp, rgb_pixel_t *matrix)
{
    // Convert the bitmap to a matrix
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            // Get the pixel from the bitmap
            rgb_pixel_t *pixel = bmp_get_pixel(bmp, i, j);
            
            // Set the pixel in the matrix
            matrix[i + width * j].alpha = pixel->alpha;
            matrix[i + width * j].red = pixel->red;
            matrix[i + width * j].green = pixel->green;
            matrix[i + width * j].blue = pixel->blue;
        }
    }
}

int main(int argc, char *argv[])
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

    // Configure the size of the shared memory object
    ftruncate(shm_fd, SHM_SIZE);

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

    // Paint the bitmap's background
    paint_background(bmp);

    // Draw the circle in the middle of the image
    draw_bmp(bmp, width / 2, height / 2);

   
   
   
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

   

    // Share the initial image
    // Protect the shared memory with the semaphore
    sem_wait(sem_sh);
    
    // Convert the bitmap to a matrix
    bmp_to_static(bmp, ptr);

    // Release the semaphore
    sem_post(sem_sh);

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

        // Else, if user presses print button...
        else if (cmd == KEY_MOUSE)
        {
            if (getmouse(&event) == OK)
            {
                if (check_button_pressed(print_btn, &event))
                {
                    // Save the image as .bmp file
                    bmp_save(bmp, "out/circle.bmp");

                    // Print that the image was saved
                    mvprintw(LINES - 1, 1, "Image saved succesfully!");
                    refresh();
                    sleep(1);
                    for (int j = 0; j < COLS - BTN_SIZE_X - 2; j++)
                    {
                        mvaddch(LINES - 1, j, ' ');
                    }
                }
            }
        }

        // If input is an arrow key, move circle accordingly...
        else if (cmd == KEY_LEFT || cmd == KEY_RIGHT || cmd == KEY_UP || cmd == KEY_DOWN)
        {
            move_circle(cmd);
            draw_circle();

            // Protect the shared memory with the semaphore
            sem_wait(sem_sh);

            // Paint the bitmap's background
            paint_background(bmp);

            // Draw the circle on the bitmap in the new position
            draw_bmp(bmp, circle.x, circle.y);

            // Convert the bitmap to a matrix
            bmp_to_static(bmp, ptr);

            // Release the semaphore
            sem_post(sem_sh);
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
