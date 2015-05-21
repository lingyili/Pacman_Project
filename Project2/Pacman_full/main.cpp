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
//PwmOut speaker(p18);

// Example of the decleration of your implementation
void playSound(char * wav);
bool isCollide(int x, int y, int gx, int gy);
void fruit_draw(int grid_fruit_x, int grid_fruit_y);
void fruit_clear(int grid_fruit_x, int grid_fruit_y);
//void play_sound_update_map();
//void play_sound_collide();
//void play_sound_eat_super_cookie();
/** Main() is where you start your implementation
    @brief The hints of implementation are in the comments. <br>
    @brief You are expected to implement your code in main.cpp and pacman.cpp. But you could modify any code if you want to make the game work better.
*/
int main()
{   
    // Initialize the timer
    /// [Example of time control implementation]
        /// Here is a rough example to implement the timer control <br><br>
    int tick, pre_tick, tick_super,tick_fruit;
    srand (time(NULL));
    Timer timer;
    timer.start();
    tick = timer.read_ms();
    tick_super = -5000;
    pre_tick = tick;
    GRID grid;
    int x_pos = 8; //pacman position
    int y_pos = 9; //pacman position
    int score = 0; 
    int life = 3;
    int level = 0;
    int blink_time = 0;
    bool flag = true;
    bool hypertransport = true;
    bool fruit_flag = true;
    int grid_fruit_x;
    int grid_fruit_y;
    // Initialize the buttons        
    left_pb.mode(PullUp);  // The variable left_pb will be zero when the pushbutton for moving the player left is pressed    
    right_pb.mode(PullUp); // The variable rightt_pb will be zero when the pushbutton for moving the player right is pressed        
    up_pb.mode(PullUp);    //the variable fire_pb will be zero when the pushbutton for firing a missile is pressed
    down_pb.mode(PullUp);  //the variable fire_pb will be zero when the pushbutton for firing a missile is pressed
    
    //initial game menu
    uLCD.locate(3,5);
    uLCD.printf("=> Normal <=");
    uLCD.locate(5,10);
    uLCD.printf("Advanced");
    while (flag) {
        if (down_pb == 0) {
            uLCD.locate(0,5);
            uLCD.printf("                                     ");
            uLCD.locate(5,5);
            uLCD.printf("Normal");
            uLCD.locate(0,10);
            uLCD.printf("                                     ");
            uLCD.locate(2,10);
            uLCD.printf("=> Advanced <=");
            level = 1;
        }
        if (up_pb == 0) {
            uLCD.locate(0,5);
            uLCD.printf("                                     ");
            uLCD.locate(3,5);
            uLCD.printf("=> Normal <=");
            uLCD.locate(0,10);
            uLCD.printf("                                     ");
            uLCD.locate(5,10);
            uLCD.printf("Advanced");
            level = 0;
        }
        if (right_pb == 0 && level == 0) {
            flag = false;
            break;    
        }
        if (right_pb == 0 && level == 1) {
            flag = false;
            break;    
        }
    }
    
    /// [Example of the game control implementation]
        /// Here is the example to initialize the game <br><br>
    uLCD.cls();
    map_init(level);
    pacman_init(8,9); // Center of the map
    // Add a new ghost with red color
    ghost_t ghost_red;
    //fruit_f fruit_apple;
    ghost_init(&ghost_red,8,7,RED);
    ghost_show(&ghost_red);
    
    // [Demo of play sound file]
    playSound("/sd/wavfiles/BUZZER.wav");
    
    /// 1. Begin the game loop
    while(1){
        tick = timer.read_ms(); // Read current time
        uLCD.locate(0,1);
        //uLCD.printf("level:%d ",level);
        uLCD.printf("cookie:%d    ", map_remaining_cookie());
        // make game harder, always go left without pushing button
        if (level >= 1) {
            PwmOut speaker(p21);
        }
        //set blinking time
        switch (level)
        {
            case 0:
            blink_time = 5000;
            break;
            case 1:
            blink_time = 4000;
            break;
            case 3:
            blink_time = 3000;
            break;
            default:
            break;
        }
        /// 2. Implement the code to get user input and update the Pacman
            /// -[Hint] Implement the code to move Pacman. You could use either push-button or accelerometer. <br>
            
        //eat cookie and add score
        grid = map_get_grid_status(x_pos, y_pos);
        if (grid.status == GRID_COOKIE && map_eat_cookie(x_pos, y_pos)) {
            score = score + 1;    
        }
        if (grid.status == GRID_SUPER_COOKIE && map_eat_cookie(x_pos, y_pos)) {
            //play_sound_eat_super_cookie();
            score = score + 5;
            tick_super = tick;
        }
        if (map_eat_cookie(x_pos, y_pos)&& grid.status == GRID_FRUIT){
            score = score + 20;
        }
        
        //display score
        uLCD.locate(0,0); 
        uLCD.printf("SCORE:%d", score);
        
        //display lives
        uLCD.locate(10,0);
        uLCD.printf("   ");
        int i;
        uLCD.locate(10,0);
        for (i=0;i<life;i++) {
            uLCD.printf("*");    
        }
        if (life == 0) {
            uLCD.locate(6,7);
            uLCD.set_font_size(6, 8);
            uLCD.printf("GAME OVER");
            break;
        }
        
        //check remaining cookie, if 0 go to next level
        if ((map_remaining_cookie())== 0) {
            playSound("/sd/wavfiles/BUZZER.wav");
            life = life + 1;
            level = level + 1;
            uLCD.cls();
            map_init(level);
            pacman_clear(x_pos,y_pos);
            x_pos = 8;
            y_pos = 9;
            pacman_init(x_pos,y_pos);
        }
        
        //add fruit
        int fruitChance = rand() % 100;
        if (fruitChance < 8 && fruit_flag) {
            grid_fruit_x = rand() % 10 + 6;
            grid_fruit_y = rand() % 10 + 5;
            grid = map_get_grid_status(grid_fruit_x,grid_fruit_y);
            if ((grid.status != GRID_WALL)&&(grid_fruit_x != x_pos)&&(grid_fruit_y!=y_pos)&&(grid_fruit_y!=ghost_red.ghost_blk_y)&&(grid_fruit_x !=ghost_red.ghost_blk_y)) {
                map_eat_cookie(grid_fruit_x,grid_fruit_y);
                map_set_status_to_Fruit(grid_fruit_x,grid_fruit_y);
                fruit_draw(grid_fruit_x,grid_fruit_y);
                fruit_flag = false;
                tick_fruit = tick;
            }
        }
        if (tick - tick_fruit > 5000 && !fruit_flag){
            map_eat_cookie(grid_fruit_x, grid_fruit_y);
            fruit_clear(grid_fruit_x, grid_fruit_y);
            fruit_flag = true;
        }
        
        //make pacman hypertransport, but only one chance
        if (left_pb == 0 && right_pb ==0 && hypertransport) {
            pacman_clear(x_pos,y_pos);
            x_pos = 8;
            y_pos = 6;
            pacman_draw(x_pos,y_pos,1);
            grid = map_get_grid_status(x_pos, y_pos);
            if (grid.status == GRID_COOKIE && map_eat_cookie(x_pos, y_pos)) {
                 score = score + 1;    
            } 
            hypertransport = false;
        }
            //
        // make pacman move
         if (up_pb == 0) {
                grid = map_get_grid_status(x_pos, y_pos-1);
                //if (map_eat_cookie(x_pos, y_pos)) {
//                    score = score + 1;    
//                }
                //while (grid.status != GRID_WALL) {
//                    pacman_clear(x_pos,y_pos);
//                    y_pos = y_pos-1;   
//                    if (y_pos == 0) {
//                        if (map_eat_cookie(x_pos, y_pos)) {
//                            score = score + 1;    
//                        }
//                    pacman_clear(x_pos,y_pos);
//                    y_pos = 15;    
//                    }
//                    if (tick-tick_super <= blink_time){
//                        pacman_invisible_draw(x_pos, y_pos,3);
//                    } else {
//                        pacman_draw(x_pos, y_pos,3);
//                    }
//                    if (down_pb ==0 ||right_pb==0|| left_pb==0) {
//                        break;    
//                    } 
//                }
                
                if (grid.status != GRID_WALL){
                    pacman_clear(x_pos,y_pos);
                    y_pos = y_pos-1;   
                }
                if (y_pos == 0) {
                    if (map_eat_cookie(x_pos, y_pos)) {
                        score = score + 1;    
                    }
                    pacman_clear(x_pos,y_pos);
                    y_pos = 15;    
                }
                if (tick-tick_super <= blink_time){
                    pacman_invisible_draw(x_pos, y_pos,3);
                } else {
                    pacman_draw(x_pos, y_pos,3);
                }
            }
            else if (down_pb == 0) {
                grid = map_get_grid_status(x_pos, y_pos+1);
                //if (map_eat_cookie(x_pos, y_pos)) {
//                    score = score + 1;    
//                }
                if (grid.status != GRID_WALL){
                    pacman_clear(x_pos,y_pos);
                    y_pos = y_pos+1;  
                }
                if (y_pos == 15) {
                    if (map_eat_cookie(x_pos, y_pos)) {
                        score = score + 1;    
                    }
                    pacman_clear(x_pos, y_pos);
                    y_pos = 0;
                }
                if (tick-tick_super <= blink_time) {
                    pacman_invisible_draw(x_pos, y_pos,4);
                } else {
                    pacman_draw(x_pos, y_pos,4);
                }
            }
            else if (right_pb == 0) {
                grid = map_get_grid_status(x_pos+1, y_pos);
                //if (map_eat_cookie(x_pos, y_pos)) {
//                    score = score + 1;    
//                }
                if (grid.status != GRID_WALL){
                    pacman_clear(x_pos,y_pos);
                    x_pos = x_pos+1;    
                }
                if (x_pos == 16) {
                    if (map_eat_cookie(x_pos, y_pos)) {
                        score = score + 1;    
                    }
                    pacman_clear(x_pos, y_pos);
                    x_pos = 0;
                }
                if (tick-tick_super <= blink_time) {
                    pacman_invisible_draw(x_pos,y_pos,1);
                } else {
                    pacman_draw(x_pos, y_pos,1);
                }
            }
            else if (left_pb == 0) {
                grid = map_get_grid_status(x_pos-1, y_pos);
                uLCD.locate(8,1);
                uLCD.printf("520");
                //if (map_eat_cookie(x_pos, y_pos)) {
//                    score = score + 1;    
//                }
                if (grid.status != GRID_WALL){
                    pacman_clear(x_pos,y_pos);
                    x_pos = x_pos-1;
                }
                if (x_pos == 0) {
                    if (map_eat_cookie(x_pos, y_pos)) {
                        score = score + 1;    
                    }
                    pacman_clear(x_pos, y_pos);
                    x_pos = 16;
                }
                if (tick-tick_super <= blink_time) {
                    pacman_invisible_draw(x_pos,y_pos,2);
                } else {
                    pacman_draw(x_pos, y_pos,2);
                }
            }
            
        // ghost move 
        if((tick-pre_tick)>500){ // Time step control
            pre_tick = tick; // update the previous tick
        /// 3. Update the Pacman on the screen
            /// -[Hint] You could update the position of Pacman (draw it on the screen) here based on the user-input at step 2. <br>
            // [demo of the ghost movement]
            ghost_random_walk(&ghost_red);
            
            // [demo of blinking Pacman]
            //pacman_clear(8,9);
//            wait(0.1);
//            pacman_draw(8,9);
        }

        /// 4. Implement the code to check the end of game.
            /// -[Hint] Check whether the ghost catch the Pacman. Make sure you could always detect that the ghost and Pacman meet on the screen.
            /// One tricky scenario is that: Pacman is at grid (3,3) and is moving to (3,4), while the ghost is at grid (3,4) and is moving to (3,3).
            /// Either at time t or t+1, you will not see the Pacman and the ghost on the same grid.
            /// No mater what, the Pacman should be caught by ghost in above scenario.
            /// Make sure your code could deal with above scenario correctly.
            /// -[Hint] Check whether Pacman win the game <br>
            if (isCollide(x_pos,y_pos,ghost_red.ghost_blk_x,ghost_red.ghost_blk_y)) {
                    if ((tick-tick_super) <= blink_time ){
                        ghost_clear(&ghost_red, ghost_red.ghost_blk_x,ghost_red.ghost_blk_y);
                        ghost_init(&ghost_red,8,7,RED);
                        ghost_show(&ghost_red);
                        ghost_random_walk(&ghost_red);
                        score = score + 10;
                    } else {
                        //play_sound_collide();
                        playSound("/sd/wavfiles/BUZZER.wav");
                        life = life -1;
                        //pacman_clear(x_pos,y_pos);
                        pacman_die_effect(x_pos,y_pos);
                        x_pos = 8;
                        y_pos = 9;
                        ghost_clear(&ghost_red, ghost_red.ghost_blk_x,ghost_red.ghost_blk_y);
                        ghost_init(&ghost_red,8,7,RED);
                        ghost_show(&ghost_red);
                        ghost_random_walk(&ghost_red);
                        pacman_draw(x_pos,y_pos,1);
                        
                    }
            }
            if (up_pb == 0 && down_pb == 0) {
                playSound("/sd/wavfiles/BUZZER.wav");
                pacman_clear(x_pos,y_pos);
                level = level + 1;
                uLCD.cls();
                map_init(level);
                x_pos = 8;
                y_pos = 9;
                pacman_init(x_pos,y_pos);
            }
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
bool isCollide(int x, int y, int gx, int gy) {
        int dis_y;
        int dis_x;
        double dir_dis;
        double dis;
        if (y<gy) {
            dis_y = (gy)-(y);
        } else {
            dis_y = (y)-(gy);   
        }
        if (x<gx) {
            dis_x = gx-x;
        } else {
            dis_x = (x)-(gx);
        }
        dis = dis_x*dis_x+dis_y*dis_y;
        dir_dis = sqrt(dis);
        if (dir_dis < 2){
            return true;
        }
        return false;
}

void fruit_draw(int grid_fruit_x, int grid_fruit_y) {
     GRID grid_info = map_get_grid_status(grid_fruit_x,grid_fruit_y);
     uLCD.filled_circle(grid_info.x+GRID_RADIUS, grid_info.y+GRID_RADIUS, 2, 0xff0000);
}

void fruit_clear(int grid_fruit_x, int grid_fruit_y){
    GRID grid_info = map_get_grid_status(grid_fruit_x,grid_fruit_y);
    uLCD.filled_rectangle(grid_info.x, grid_info.y, grid_info.x+GRID_SIZE-1, grid_info.y+GRID_SIZE-1, BACKGROUND_COLOR);
}