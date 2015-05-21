/* Gatech ECE2035 2015 SPRING PAC MAN
 * Copyright (c) 2015 Gatech ECE2035
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/** @file main.cpp */
// Include header files for platform
#include "mbed.h"
#include "wave_player.h"
#include "SDFileSystem.h"

// Include header files for pacman project
#include "globals.h"
#include "map_public.h"
#include "pacman.h"
#include "ghost.h"
#include "MMA8452.h"

// Platform initialization
DigitalIn left_pb(p21);  // push bottem
DigitalIn right_pb(p22); // push bottem
DigitalIn up_pb(p23);    // push bottem
DigitalIn down_pb(p24);  // push bottem
uLCD_4DGL uLCD(p9,p10,p11); // LCD (serial tx, serial rx, reset pin;)
Serial pc(USBTX,USBRX);     // used by Accelerometer
MMA8452 acc(p28, p27, 100000); // Accelerometer
AnalogOut DACout(p18);      // speaker
wave_player waver(&DACout); // wav player
SDFileSystem sd(p5, p6, p7, p8, "sd"); // SD card and filesystem (mosi, miso, sck, cs)

// Example of the decleration of your implementation
void playSound(char * wav);


/** Main() is where you start your implementation
    @brief The hints of implementation are in the comments. <br>
    @brief You are expected to implement your code in main.cpp and pacman.cpp. But you could modify any code if you want to make the game work better.
*/
int main()
{   
    // Initialize the timer
    /// [Example of time control implementation]
        /// Here is a rough example to implement the timer control <br><br>
    int tick, pre_tick;
    double x,y,z;
    srand (time(NULL));
    Timer timer;
    timer.start();
    tick = timer.read_ms();
    pre_tick = tick;
    
    // Initialize the buttons        
    left_pb.mode(PullUp);  // The variable left_pb will be zero when the pushbutton for moving the player left is pressed    
    right_pb.mode(PullUp); // The variable rightt_pb will be zero when the pushbutton for moving the player right is pressed        
    up_pb.mode(PullUp);    //the variable fire_pb will be zero when the pushbutton for firing a missile is pressed
    down_pb.mode(PullUp);  //the variable fire_pb will be zero when the pushbutton for firing a missile is pressed
    
    /// [Example of the game control implementation]
        /// Here is the example to initialize the game <br><br>
    uLCD.cls();
    map_init();
    pacman_init(8,9); // Center of the map
    
    // Add a new ghost with red color
    ghost_t ghost_red;
    ghost_init(&ghost_red,8,7,RED);
    ghost_show(&ghost_red);
    
    // [Demo of play sound file]
    playSound("/sd/wavfiles/BUZZER.wav");
    
    /// 1. Begin the game loop
    while(1){
        tick = timer.read_ms(); // Read current time
        
        /// 2. Implement the code to get user input and update the Pacman
            /// -[Hint] Implement the code to move Pacman. You could use either push-button or accelerometer. <br>
        if (right_pb == 0) {
            
        }
        // [Some hints for user-input detection]
        acc.readXYZGravity(&x,&y,&z); //read accelerometer
        uLCD.locate(0,1);
        uLCD.printf("sensor x%4.1f y%4.1f\n",x,y); //You could remove this code if you already make the accelerometer work.
            /// -[Hint] Here is a simple way to utilize the readings of accelerometer:
            ///         If x is larger than certain value (ex:0.3), then make the Pacman move right.
            ///         If x<-0.3, then make it move left. <br>


        if((tick-pre_tick)>500){ // Time step control
            pre_tick = tick; // update the previous tick
            
        /// 3. Update the Pacman on the screen
            /// -[Hint] You could update the position of Pacman (draw it on the screen) here based on the user-input at step 2. <br>

            // [demo of the ghost movement]
            ghost_random_walk(&ghost_red);
            
            // [demo of blinking Pacman]
            // It just a demonstration, you could remove this code.
            pacman_clear(8,9);
            wait(0.1);
            pacman_draw(8,9);

        }
        
        /// 4. Implement the code to check the end of game.
            /// -[Hint] Check whether the ghost catch the Pacman. Make sure you could always detect that the ghost and Pacman meet on the screen.
            /// One tricky scenario is that: Pacman is at grid (3,3) and is moving to (3,4), while the ghost is at grid (3,4) and is moving to (3,3).
            /// Either at time t or t+1, you will not see the Pacman and the ghost on the same grid.
            /// No mater what, the Pacman should be caught by ghost in above scenario.
            /// Make sure your code could deal with above scenario correctly.
            /// -[Hint] Check whether Pacman win the game <br>

    }
}


// Example of implementation of your functions
void playSound(char * wav)
{
    // open wav file
    FILE *wave_file;
    wave_file=fopen(wav,"r");

    if(wave_file == NULL){
        uLCD.locate(9,0);
        uLCD.printf("ERROR_SD");
        return;
    }
    
    // play wav file
    waver.play(wave_file);

    // close wav file
    fclose(wave_file);
}

