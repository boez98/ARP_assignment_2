# ARP-Assignment2 - Group AA98

Andrea Bolla 4482930
Anna Possamai 5621738

# Introduction
The task is to design, develop, test and deploy is an interactive simulator of a (simplified) typical vision system, able to track an object in a 2-D plane. This is a true example of an industrial vision system in a robotic manufacturing workshop.
This assignment requires the use of a shared memory in which two processes operate simultaneously, as happens in reality in similar applications.
In our case we don't have a camera, so we will simulate the creation of the moving image using an ncurses window. Using arrow keys, we will move a spot in a window to simulate the perception of the camera. The spot that we will see by moving will produce the creation of a realistic RGB image (a circle in this case) in the simulated, shared, video memory.
We will use an 80 x 30 ncurses window. The video memory, for realism, will be 1600 x 600 pixels RGB true color 3 (3 bytes/pixel). There will then be a factor of 20x in the location of the interactive spot and the corresponding image in the video memory.

# Structure

1. The master process responsible of spawning the other process
2. Process A, which simulates capture from a video camera and fills the video memory.
3. Process B will simulate the extraction of a feature from the acquired moving image instead. For simplicity, the image in the video memory will be scanned and the center of the image will be extracted. 

Two ncurses window will spawn:
1. Using arrow keys, we will move a spot in a window to simulate the perception of the camera. The spot that we will see by moving will produce the creation of a realistic RGB image. By pressing a key, or by operating the mouse on a button, a snapshot of the image memory will be saved on a .bmp file.
2. In a second ncurses window, also 80 x 30, the position trace of the center of the image will be shown.

- The function 'paint_background()' is a trick to avoid this behavior by painting all the image of white

![image copia](https://user-images.githubusercontent.com/107572039/211508543-4ce67da4-a9ac-41ba-a4b5-51c5147a7a7d.jpg)



- The function 'draw_bmp()' simply draw a bmp circle



![circle copia](https://user-images.githubusercontent.com/107572039/211508717-6cdec41f-ee9a-4f5d-9c30-d1081f2ccbeb.jpg)










# *libbitmap* installation and usage
To work with the bitmap library, you need to follow these steps:
1. Download the source code from [this GitHub repo](https://github.com/draekko/libbitmap.git) in your file system.
2. Navigate to the root directory of the downloaded repo and run the configuration through command ```./configure```. Configuration might take a while.  While running, it prints some messages telling which features it is checking for.
3. Type ```make``` to compile the package.
4. Run ```make install``` to install the programs and any data files and documentation.
5. Upon completing the installation, check that the files have been properly installed by navigating to ```/usr/local/lib```, where you should find the ```libbmp.so``` shared library ready for use.
6. In order to properly compile programs which use the *libbitmap* library, you first need to notify the **linker** about the location of the shared library. To do that, you can simply add the following line at the end of your ```.bashrc``` file: ```export LD_LIBRARY_PATH="/usr/local/lib:$LD_LIBRARY_PATH"```     


# Using *libbitmap* in your code

Now that you have properly installed the library in your system, it's time to use it in your programs:
1. Include the library in your programs via ```#include <bmpfile.h>```. If you want to check the content of ```bmpfile.h``` to glimpse the functionalities of the library, navigate to ```/usr/local/include```, where the header file should be located.
2. Compile programs which use the *libbitmap* library by linking the shared library with the ```-lbmp``` command.     
Example for compiling **circle.c**: ```gcc src/circle.c -lbmp -lm -o bin/circle``` 

# Compiling and running **processA** and **processB**
The two processes are implemented as UIs through *ncurses* library, therefore you need to compile their source files by linking the shared library via ```-lncurses```. As for the first assignment, exploit the resize event of the windows to get out of situations in which the graphical elements do not properly spawn.

To compile the code type:
     
     ./compile.sh
     
then to run it:

     ./run.sh


# Problems
- In the second ncurses window, the position of the center of the circle is shown in real time but remain the trace of the path made, I wasn't able to fix it


