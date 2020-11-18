#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "cp_functions.h"

#define True  1
#define False 0
#define WindowTitle  "Breakout 61"
#define WindowWidth  800
#define WindowHeight 700

Sound hit_paddle_sound, hit_brick_sound;
Sound hit_top_sound, end_sound;
Texture paddle_texture, ball_texture[3], Item_texture[6], Home_texture[3],Rocket_texture;
Texture brick_texture[5], brick_Main, background_texture[4];
Font big_font, small_font;

// Structure for storing info for objects, i.e. Paddle, Brick, Ball.
typedef struct
{
    float x, y;
    float width, height;
    float vel_x, vel_y;
    int destroyed;
} Object;

// Collision Detection between two objects a and b
int collide(Object a, Object b)
{
    if (a.x + a.width  < b.x || b.x + b.width  < a.x ||
        a.y + a.height < b.y || b.y + b.height < a.y)
        return False;
    else
        return True;
}

// Initial routine to load sounds, textures, and fonts.
int game_init()
{
    hit_paddle_sound = cpLoadSound("hitDown.wav");
    hit_brick_sound = cpLoadSound("hitUp.wav");
    hit_top_sound = cpLoadSound("hitTop.wav");
    end_sound = cpLoadSound("theEnd.wav");

    paddle_texture = cpLoadTexture("paddleMain.png");

    ball_texture[0] = cpLoadTexture("ballMain.png");
    ball_texture[1] = cpLoadTexture("ballMain2.png");
    ball_texture[2] = cpLoadTexture("ballMain3.png");

    Home_texture[0] = cpLoadTexture("Logo1.png");
    Home_texture[1] = cpLoadTexture("Logo2.png");
    Home_texture[2] = cpLoadTexture("Logo3.png");


    brick_texture[0] = cpLoadTexture("brickMain1.png"); //อิฐชนิดที่ 1
    brick_texture[1] = cpLoadTexture("brickMain2.png"); //อิฐชนิดที่ 2
    brick_texture[2] = cpLoadTexture("brickMain3.png"); //อิฐชนิดที่ 3
    brick_texture[3] = cpLoadTexture("brickMain4.png"); //อิฐชนิดที่ 4
    brick_texture[4] = cpLoadTexture("brickMain5.png"); //อิฐชนิดที่ 5


    Item_texture[0] = cpLoadTexture("ItemMain1.png");//Item ชนิดที่ 1
    Item_texture[1] = cpLoadTexture("ItemMain2.png");//Item ชนิดที่ 2
    Item_texture[2] = cpLoadTexture("ItemMain3.png");//Item ชนิดที่ 3
    Item_texture[3] = cpLoadTexture("ItemMain4.png");//Item ชนิดที่ 4
    Item_texture[4] = cpLoadTexture("ItemMain5.png");//Item ชนิดที่ 5
    Item_texture[5] = cpLoadTexture("ItemMain6.png");//Item ชนิดที่ 6


    Rocket_texture = cpLoadTexture("Rocket.png");


    background_texture[0] = cpLoadTexture("wallpaperMain1.png");//ฉากหลังด่านแรก
    background_texture[1] = cpLoadTexture("wallpaperMain2.png");//ฉากหลังด่านที่2
    background_texture[2] = cpLoadTexture("wallpaperMain3.png");//ฉากหลังด่านที่3
    background_texture[3] = cpLoadTexture("background.png");//ฉากจบเกม


    big_font = cpLoadFont("THSarabun.ttf", 60);
    small_font = cpLoadFont("THSarabun.ttf", 30);

    if (hit_paddle_sound == NULL || hit_brick_sound == NULL ||
        hit_top_sound == NULL || end_sound == NULL ||
        paddle_texture == NULL || ball_texture == NULL ||
        brick_texture == NULL || background_texture == NULL ||
        big_font == NULL || small_font == NULL)
        return False;
    return True;
}


int ChackPoint(Object ball , Object paddle){//ฟังก์ชั่นเช็คจุดที่ลูกบอลกระทบกับไม้ตี
    float ballx , OneOfPaddle;
    ballx = ball.x + (ball.width/2);
    OneOfPaddle = paddle.width/5; //แบ่งไม้ตีเป็น 5 ส่วน |-----|
    if(ballx <= paddle.x + OneOfPaddle) //ถ้าลูกบอลตกที่ส่วนแรก |*----|
        return -4; //ถ้าบอลตกทางซ้ายของไม้ตี บอลจะเด้งไปทางซ้าย ด้วยความเร็ว -4
    else if(ballx > paddle.x + OneOfPaddle && ballx <= paddle.x + OneOfPaddle*2)//ถ้าลูกบอลตกที่ส่วนท้าย |-*---|
        return -2; //ถ้าบอลตกทางขวาของไม้ตี บอลจะเด้งไปทางซ้าย ด้วยความเร็ว -2
    else if(ballx >= paddle.x + OneOfPaddle*3 && ballx < paddle.x + OneOfPaddle*4)//ถ้าลูกบอลตกที่ส่วนท้าย |---*-|
        return 2; //ถ้าบอลตกทางขวาของไม้ตี บอลจะเด้งไปทางขวา ด้วยความเร็ว 2
    else if(ballx >= paddle.x + OneOfPaddle*4)//ถ้าลูกบอลตกที่ส่วนท้าย |----*|
        return 4; //ถ้าบอลตกทางขวาของไม้ตี บอลจะเด้งไปทางขวา ด้วยความเร็ว 4
    else//ถ้าลูกบอลตกตรงกลาง |--*--|
        return 0; //ลูกบอลจะพุ่งขึ้นด้านบน
}


int main(int argc, char *args[])
{ 
    enum {BALL_VEL_Y = -5,PADDLE_VEL_X = 7 };
    int BALL_VEL_X, running, n_bricks = 11, n_hits = 0, score = 0 , ball_N = 1 /*ตัวแปรกสำหรับนับลูกบอล*/, type[8][n_bricks] /*ตัวแปรกำหนดชนิดของอิฐ*/, RocketN = 0, Level = 1, Life = 3, plus = True /*ตัวแปรสำหรับเช็คว่าความเร็วถูกเพิ่มหรือยัง*/,randomItem/*ตัวแปรสำหรับสุ่มItem*/;
    srand(time(NULL));
    while(True)
    {
        BALL_VEL_X = 5 - rand() % 10;///ุ่มค่า X ที่ลูกบอลจะวิ่งในแต่ละเกม
        if(!(BALL_VEL_X > -2 && BALL_VEL_X < 2))//ถ้าค่าเหมาะสมจะจบการสุ่ม
            break;
    }
    char msg[80];
    Object bricks[8][n_bricks];
    Object ball = {WindowWidth/2-12, WindowHeight-80, 24, 24, 0, BALL_VEL_Y, False};
    Object ball_Bonus[16];//ตัวแปรสำหรับสร้างบอล 16 ลูก
    Object paddle = {WindowWidth/2-62, WindowHeight-50, 124, 18, 0, 0, False};
    Object Item = {0, 0, 24, 24, 0, 0, True}; //ตัวแปรสำหรับเลือก Item // Set ค่าเริ่มต้นของ Item
    Object Rocket = {0, 0, 24, 24, 0, 0, True};
    Event event;
    

    if (cpInit(WindowTitle, WindowWidth, WindowHeight) == False) {
        fprintf(stderr, "Window initialization failed!\n");
        exit(1);
    }

    if (game_init() == False) {
        fprintf(stderr, "Game initialization failed!\n");
        exit(1);
    }
    running = True;
    while (running)//สร้างหน้า Home สำหรับเลือกด่าน
    {
        cpClearScreen();
        cpDrawTexture(255, 255, 255,0, 0, WindowWidth, WindowHeight, Home_texture[Level-1]); //แสดง Wallpaper แต่ละด่าน
        cpSwapBuffers();
        while (cbEventListener(&event)) {
                if (event.type == QUIT ||event.type == KEYUP && event.key.keysym.sym == K_ESCAPE){ //เมื่อกด ESC จะเริ่มเล่นเกม
                    event.type = KEYDOWN;
                    running = False;
                    break;
                }
                if (event.type == KEYDOWN) { //ปรับ Level ตามที่ผู้เล่นกด และป้องกัน Level ที่น้อย หรือมากเกินไป
                    if (event.key.keysym.sym == K_LEFT){ 
                        Level--;
                        if(Level < 1)
                            Level = 1;    
                    }
                    if (event.key.keysym.sym == K_RIGHT){
                        Level++;
                        if(Level > 3)
                            Level = 3;
                    }
                    cpPlaySound(hit_paddle_sound);
                    event.type = KEYUP;
                }
        }
        cpDelay(10);
    }

    if(Level == 1){
        for(int i = 0; i < 16; i++){//ลูปกำหนดค่าเริ่มต้นของบอล 15 ลูก
            BALL_VEL_X = 3 - rand() % 6;//สุ่มค่า X ที่ลูกบอลจะวิ่งในแต่ละเกม
            int x , y , Vel_y = BALL_VEL_Y; //ตัวแปรสำหรับกำหนดค่า เพื่อให้ให้ไปกระทบกับ Level ถัดไป
            if(!i)//บอลปกติจะสร้างขึ้นที่ไม้ตี
            {
                x = WindowWidth/2-12;
                y = WindowHeight-80;
                ball_Bonus[i].vel_x = 0;
                ball_Bonus[i].destroyed = False;
            }else
            {
                x = WindowWidth/2-12;
                y = 80;
                ball_Bonus[i].vel_x = BALL_VEL_X;
                ball_Bonus[i].destroyed = True;
                while(True) //สุ่ม ทิศทาง และความเร็วในแกน Y
                {
                    Vel_y = 5 - rand() % 10;///ุ่มค่า X ที่ลูกบอลจะวิ่งในแต่ละเกม
                    if(!(Vel_y > -2 && Vel_y < 2))//ถ้าค่าเหมาะสมจะจบการสุ่ม
                        break;
                }
            }
            
            ball_Bonus[i].x = x;
            ball_Bonus[i].y = y;
            ball_Bonus[i].width = 24;
            ball_Bonus[i].height = 24;
            ball_Bonus[i].vel_y = Vel_y;
        }

        for(int i = 0, y = 80; i < 8; i++){
            for (int n = 0,x = 70; n < n_bricks; n++) {
                bricks[i][n].width = 55;
                bricks[i][n].height = 18;
                bricks[i][n].x = x;
                bricks[i][n].y = y;
                bricks[i][n].destroyed = False;
                x += bricks[i][n].width+5;
                if(i == 0 && n == 5)
                    type[i][n] = 3; //สร้างอิฐโบนัส
                else
                    type[i][n] = 0; //set count สำหรับนับจำนวนครั้งที่ลูกบอลกระทบอิฐ
            }
            y += 20;
        }

        running = True;
        while (running) {
            cpClearScreen();
            cpDrawTexture(255, 255, 255,0, 0, WindowWidth, WindowHeight, background_texture[0]);
            cpDrawTexture(255, 255, 255,paddle.x, paddle.y, paddle.width, paddle.height, paddle_texture);
            for(int i = 0 ; i < 16; i++){//สร้างลูกบอกบนหน้าจอถ้าลูกบอลนั้นยังไม่ถูกทำลาย
                if(!ball_Bonus[i].destroyed)
                    cpDrawTexture(255, 255, 255,ball_Bonus[i].x, ball_Bonus[i].y, ball_Bonus[i].width, ball_Bonus[i].height, ball_texture[0]);
            }
            sprintf(msg, "Level: %d Life: %d", Level,Life);//ข้อความแสดงเลขด่าน,จำนวนชีวิต
            cpDrawText(255, 255, 255, 3, 3, msg, small_font, 0);
            sprintf(msg, "Score: %d", score);
            cpDrawText(255, 255, 255, 3, 33, msg, small_font, 0);
            for(int i = 0; i < 8; i++){
                for (int n = 0; n < n_bricks; n++) {
                    if(!bricks[i][n].destroyed) 
                        cpDrawTexture(255, 255, 255,bricks[i][n].x, bricks[i][n].y, bricks[i][n].width, bricks[i][n].height,brick_texture[type[i][n]]); //แสดงผลอิฐแต่ละชนิด
                }
            }
            for(int i = 0; i < 16; i++){
                if(ball_Bonus[i].y + ball_Bonus[i].width > WindowHeight && !ball_Bonus[i].destroyed){
                    ball_Bonus[i].destroyed = True;
                    ball_N--;
                }
            }

            if (!ball_N || n_hits == n_bricks*8) {//ถ้าไม่เหลือลูกบอลในเกมเลย
                if(n_hits == n_bricks*8){
                    cpPlaySound(end_sound);
                    cpDrawText( 255, 255, 255, 400, 350, "YOU WON ESCAPE FOR NEXT", big_font, 1);
                    Level++;//เปลี่ยนด่านเมื่อผู้เล่นชนะ
                    running = False;
                }else if(!Life){ //ถ้าจำนวนชีวิตหมดจะจบเกม
                    cpPlaySound(end_sound);
                    cpDrawText( 255, 255, 255, 400, 350, "ENDGAME", big_font, 1);
                    running = False;
                }else{
                    cpPlaySound(end_sound);
                    cpDrawText( 255, 255, 255, 400, 350, "YOU DIE!! ESCAPE FOR THE NEXT LIFE", big_font, 1); 
                    ball_Bonus[0].y = WindowHeight-80;
                    ball_Bonus[0].x = paddle.x+50;//กำหนดให้ลูกบอลใหม่เริ่มต้นตำแหน่งที่ไม้ตีอยู่
                    ball_Bonus[0].vel_y = -ball_Bonus[0].vel_y;
                    ball_Bonus[0].vel_x = 0;
                    ball_Bonus[0].destroyed = False;
                    ball_N++; //เพิ่มลูกบอกจาก 0 เป็น 1 ลูกเมื่อผู้เล่นตาย
                    Life--;
                }
                cpSwapBuffers();
                while (1) {
                    cbEventListener(&event);
                    if (event.type == QUIT || event.type == KEYUP && event.key.keysym.sym == K_ESCAPE) {
                        event.type = KEYDOWN;
                        break;
                    }
                }

            }
            cpSwapBuffers();
            if(n_hits == n_bricks && plus){ //เพิ่มความเร็วลูกบอลเมื่ออิฐถูกทำลายไป 11 ก้อน
                for(int i = 0; i < 16; i++){//เพิ่มความเร็วทั้ง 11 ลูก
                    if(!ball_Bonus[i].destroyed){
                        if(ball_Bonus[i].vel_y < 0)
                            ball_Bonus[i].vel_y -= 3;
                        else
                            ball_Bonus[i].vel_y += 3;
                    }
                }
                plus = False;
            }


            while (cbEventListener(&event)) {
                if (event.type == QUIT ||
                    event.type == KEYUP && event.key.keysym.sym == K_ESCAPE) {
                    running = False;
                    break;
                }

                if (event.type == KEYDOWN) {
                    if (event.key.keysym.sym == K_LEFT)
                        paddle.vel_x = -abs(PADDLE_VEL_X);
                    if (event.key.keysym.sym == K_RIGHT)
                        paddle.vel_x = abs(PADDLE_VEL_X);
                }else if (event.type == KEYUP) {
                    if (event.key.keysym.sym == K_LEFT)
                        paddle.vel_x = 0;
                    if (event.key.keysym.sym == K_RIGHT)
                       paddle.vel_x = 0;
                }
            }
            paddle.x += paddle.vel_x;

            if (paddle.x < 0)
                paddle.x = 0;
            if (paddle.x + paddle.width > WindowWidth)
                paddle.x = WindowWidth - paddle.width;

            for(int i = 0; i < 16; i++){
                if(!ball_Bonus[i].destroyed){//ทำให้ลูกบอลแต่ละลูกเคลื่อนที่ตามค่าที่สุ่มไว้
                    ball_Bonus[i].x += ball_Bonus[i].vel_x;
                    ball_Bonus[i].y += ball_Bonus[i].vel_y;
                }
            }

            for(int i = 0; i < 16; i++){//เปลี่ยนทิศทางบอลแต่ละลูกเมื่อมีการกระทบกับขอบข้าง
                if ((ball_Bonus[i].x < 0 || ball_Bonus[i].x + ball_Bonus[i].width > WindowWidth) && !ball_Bonus[i].destroyed){
                    cpPlaySound(hit_top_sound);
                    ball_Bonus[i].vel_x = -ball_Bonus[i].vel_x;
                }
            }
            for(int i = 0; i < 16; i++){//เปลี่ยนทิศทางบอลแต่ละลูกเมื่อมีการกระทบกับขอบบน
                if (ball_Bonus[i].y < 0 && !ball_Bonus[i].destroyed) {
                    cpPlaySound(hit_top_sound);
                    ball_Bonus[i].vel_y = -ball_Bonus[i].vel_y;
                }
            }
            for(int j = 0; j < 16; j++)//เช็คการกระทบของบอลแต่ละลูกกับอิฐแต่ละก้อน
            {
                for(int i = 0; i < 8; i++)
                {
                    for (int n = 0; n < n_bricks; n++)
                    {
                        if (!bricks[i][n].destroyed && collide(ball_Bonus[j], bricks[i][n]) == True && !ball_Bonus[j].destroyed)
                        {
                            cpPlaySound(hit_brick_sound);
                            ball_Bonus[j].vel_y = -ball_Bonus[j].vel_y;
                            if(type[i][n] == 1){ //ถ้าลูกบอลกระทบอิฐมากกว่า 1 ครั้ง อิฐจะถูกทำลาย
                                bricks[i][n].destroyed = True;
                                n_hits++;
                            }
                            else if(type[i][n] == 3)
                            {
                                bricks[i][n].destroyed = True;
                                n_hits++;
                                for(int k = 1; k < 16; k++)
                                {
                                    ball_Bonus[k].destroyed = False;
                                    ball_N++;
                                }

                            }
                            else if(!type[i][n]){
                                type[i][n] = 1;
                            }
                            score += 10;
                            break;
                        }
                    }
                }
            }
            
            for(int i = 0; i < 16; i++){//เช็คการกระทบของบอลแต่ละลูฏ
                if(collide(ball_Bonus[i], paddle) == True && !ball_Bonus[i].destroyed) {
                    cpPlaySound(hit_paddle_sound);
                    ball_Bonus[i].vel_y = -ball_Bonus[i].vel_y;
                    ball_Bonus[i].vel_x = ChackPoint(ball_Bonus[i],paddle); //เปลี่ยนทิศทางบอลตามเงื่อนไขในฟังชั่น
                }
             }
 
             cpDelay(10);
         }
    }
    //.................................................................................................................................
    //.................................................................................................................................
    //.................................................................................................................................
    //.................................................................................................................................
    //.................................................................................................................................
    //.................................................................................................................................
    if(Level == 2){//จบ Level 1 เริ่มการทำงานด่านที่ 2
         for(int i = 0, y = 80; i < 8; i++){
             for (int n = 0,x = 70; n < n_bricks; n++) {
                 bricks[i][n].width = 55;
                 bricks[i][n].height = 18;
                 bricks[i][n].x = x;
                 bricks[i][n].y = y;
                 bricks[i][n].destroyed = False;
                 x += bricks[i][n].width+5;
                 if(i == 6 && (n < 2 || n > n_bricks - 3))
                     type[i][n] = 2;//set ให้อิฐไม่สามารถถูกทำลายได้
                 else if(i == 7 && (n < 2 || n > n_bricks - 3))
                     type[i][n] = 3;//set เป็นอิฐชนิดที่ 4
                 else
                     type[i][n] = 0; //set count สำหรับนับจำนวนครั้งที่ลูกบอลกระทบอิฐ
             }
             y += 20;
         }
 
         running = True;
         while (running) {
             cpClearScreen();
             cpDrawTexture(255, 255, 255,0, 0, WindowWidth, WindowHeight, background_texture[1]);
             cpDrawTexture(255, 255, 255,paddle.x, paddle.y, paddle.width, paddle.height, paddle_texture);
             cpDrawTexture(255, 255, 255,ball.x, ball.y, ball.width, ball.height, ball_texture[1]);
             sprintf(msg, "Level: %d Life: %d", Level,Life);//ข้อความแสดงเลขด่าน,จำนวนชีวิต
             cpDrawText(255, 255, 255, 3, 3, msg, small_font, 0);
             sprintf(msg, "Score: %d", score);
             cpDrawText(255, 255, 255, 3, 33, msg, small_font, 0);
 
             if(!Item.destroyed)//ถ้า Item ยังไม่ถูกทำลาย
                 cpDrawTexture(255, 255, 255,Item.x, Item.y, Item.width, Item.height, Item_texture[randomItem]);//สร้างItem ให้ตกจากอิฐ
 
             for(int i = 0; i < 8; i++){
                 for (int n = 0; n < n_bricks; n++) {
                    if(!bricks[i][n].destroyed)
                        cpDrawTexture(255, 255, 255,bricks[i][n].x, bricks[i][n].y, bricks[i][n].width, bricks[i][n].height,brick_texture[type[i][n]]); //แสดงผลอิฐแต่ละชนิด
                 }
             }
 
             if(!Item.destroyed && (Item.y + Item.width > WindowHeight)){//เช็คว่า Item ชนขอบล่าง
                 Item.x = 0;
                 Item.y = 0;
                 Item.vel_y = 0;
                 Item.destroyed = True;
                 //Set Item ใหม่เมื่อเก็บ Item ไม่ได้
             }
 
             if (ball.y + ball.width > WindowHeight || n_hits == n_bricks*8 - 4) {
                 if(n_hits == n_bricks*8 - 4){
                     cpPlaySound(end_sound);
                     cpDrawText( 255, 255, 255, 400, 350, "YOU WON ESCAPE FOR NEXT", big_font, 1);
                     Level++;//เปลี่ยนด่านเมื่อผู้เล่นชนะ
                     running = False;
                 }else if(!Life){ //ถ้าจำนวนชีวิตหมดจะจบเกม
                     cpPlaySound(end_sound);
                     cpDrawText( 255, 255, 255, 400, 350, "ENDGAME", big_font, 1);
                     running = False;
                 }else{
                     cpPlaySound(end_sound);
                     cpDrawText( 255, 255, 255, 400, 350, "YOU DIE!! ESCAPE FOR THE NEXT LIFE", big_font, 1); 
                     ball.y = WindowHeight-80;
                     ball.x = paddle.x+50;//กำหนดให้ลูกบอลใหม่เริ่มต้นตำแหน่งที่ไม้ตีอยู่
                     ball.vel_y = -ball.vel_y;
                     ball.vel_x = 0;
                     Life--;
                 }
                 cpSwapBuffers();
                 while (1) {
                     cbEventListener(&event);
                     if (event.type == QUIT || event.type == KEYUP && event.key.keysym.sym == K_ESCAPE) {
                         event.type = KEYDOWN;
                         break;
                     }
                 }
 
             }
             cpSwapBuffers();
             if(n_hits == n_bricks && plus){ //เพิ่มความเร็วลูกบอลเมื่ออิฐถูกทำลายไป 11 ก้อน
                 if(ball.vel_y < 0)
                     ball.vel_y -= 3;
                 else
                     ball.vel_y += 3;
                 plus = False;
             }
 
 
             while (cbEventListener(&event)) {
                 if (event.type == QUIT ||
                     event.type == KEYUP && event.key.keysym.sym == K_ESCAPE) {
                     running = False;
                     break;
                 }
 
                 if (event.type == KEYDOWN) {
                     if (event.key.keysym.sym == K_LEFT)
                         paddle.vel_x = -abs(PADDLE_VEL_X);
                     if (event.key.keysym.sym == K_RIGHT)
                         paddle.vel_x = abs(PADDLE_VEL_X);
                 }else if (event.type == KEYUP) {
                     if (event.key.keysym.sym == K_LEFT)
                         paddle.vel_x = 0;
                     if (event.key.keysym.sym == K_RIGHT)
                        paddle.vel_x = 0;
                 }
             }
             paddle.x += paddle.vel_x;
 
             if (paddle.x < 0)
                 paddle.x = 0;
             if (paddle.x + paddle.width > WindowWidth)
                 paddle.x = WindowWidth - paddle.width;
 
             ball.x += ball.vel_x;
             ball.y += ball.vel_y;
 
             if(!Item.destroyed)//ถ้า Item ยังไม่ถูกทำลาย
                 Item.y += Item.vel_y;//กำหนดให้ Item ตก
 
 
             if (ball.x < 0 || ball.x + ball.width > WindowWidth){
                 cpPlaySound(hit_top_sound);
                 ball.vel_x = -ball.vel_x;
             }
 
             if (ball.y < 0) {
                 cpPlaySound(hit_top_sound);
                 ball.vel_y = -ball.vel_y;
             }
             for(int i = 0; i < 8; i++){
                 for (int n = 0; n < n_bricks; n++) {
                     if (!bricks[i][n].destroyed &&collide(ball, bricks[i][n]) == True) {cpPlaySound(hit_brick_sound);
                         ball.vel_y = -ball.vel_y;
                         if(type[i][n] == 1){ //ถ้าลูกบอลกระทบอิฐมากกว่า 1 ครั้ง อิฐจะถูกทำลาย
                             bricks[i][n].destroyed = True;
                             n_hits++;
                             score += 10;
                         }
                         else if(type[i][n] == 0){
                             type[i][n] = 1;
                             score += 10;
                         }
                         else if(type[i][n] == 3 && Item.destroyed)//ถ้า Item ยังไม่ถูกทำลาย Item ที่ 2 จะยังไม่ดรอป
                         {
                             bricks[i][n].destroyed = True;
                             n_hits++;
                             randomItem =  rand() % 5;//สุ่มชนิด Item
                             Item.x = bricks[i][n].x + bricks[i][n].width/2 - Item.width/2;//กำหนด Item ให้ Item เกิดจุดที่อิฐแตก
                             Item.y = bricks[i][n].y;
                             Item.vel_y = 4;
                             Item.destroyed = False;
                         }
                         break;
                     }
                 }
             }
             if (!Item.destroyed && collide(Item, paddle) == True){//เมื่อ Item ตกที่ไม้ตี้ Item จะถูกทำลาย(Item แสดงผล)
                 cpPlaySound(hit_paddle_sound);
                 Item.x = 0;
                 Item.y = 0;
                 Item.vel_y = 0;
                 Item.destroyed = True;
                 //Set Item ใหม่เมื่อเราเก็บItem ได้
                 if(!randomItem)
                     paddle.width += 50; //เพิ่มขนาดไม้ตี
                 else if (randomItem == 1)
                     paddle.width -= 50; //เพิ่มขนาดไม้ตีลดขนาดไม้ตี
                 else if (randomItem == 2)
                     Life++;//ชีวิตเพิ่มขึ้น 1 ชีวิต
                 else if (randomItem == 3)
                     if(ball.vel_y < 0) //เพิ่มความเร็วลูกบอล
                         ball.vel_y -= 3;
                     else
                         ball.vel_y += 3;
                 else if (randomItem == 4)
                     if(ball.vel_y < 0 && ball.vel_y + 3) //ลดความเร็วลูกบอล //ป้องกันการที่ลูกบอลความเร็วเป็น 0
                         ball.vel_y += 3;
                     else if(ball.vel_y - 3)
                         ball.vel_y -= 3;
                     else if (ball.vel_y < 0) //ป้องกันการที่ลูกบอลความเร็วเป็น 0
                         ball.vel_y = 1;
                     else
                         ball.vel_y = -1;
                     
             }
 
            if (collide(ball, paddle) == True) {
                cpPlaySound(hit_paddle_sound);
                ball.vel_y = -ball.vel_y;
                ball.vel_x = ChackPoint(ball,paddle); //เปลี่ยนทิศทางบอลตามเงื่อนไขในฟังชั่น
            }

            cpDelay(10);
        }
    }
    //.................................................................................................................................
    //.................................................................................................................................
    //.................................................................................................................................
    //.................................................................................................................................
    //.................................................................................................................................
    //.................................................................................................................................
    //.................................................................................................................................
    //.................................................................................................................................
    //.................................................................................................................................
    //.................................................................................................................................
    //.................................................................................................................................
    //.................................................................................................................................
    if(Level == 3){//จบ Level 2 เริ่มการทำงานด่านที่ 3 
    for(int i = 0, y = 80; i < 8; i++){
            for (int n = 0,x = 70; n < n_bricks; n++) {
                bricks[i][n].width = 55;
                bricks[i][n].height = 18;
                bricks[i][n].x = x;
                bricks[i][n].y = y;
                bricks[i][n].destroyed = False;
                x += bricks[i][n].width+5;
                if((i == 0 || i == 7) && (n == 0 || n == n_bricks-1)) //Set ชนิดของ อิฐ
                    type[i][n] = 3; 
                else if(i == 7)
                    type[i][n] = 4;
                else
                    type[i][n] = 0; 
            }
            y += 20;
        }

        running = True;
        while (running) {
            cpClearScreen();
            cpDrawTexture(255, 255, 255,0, 0, WindowWidth, WindowHeight, background_texture[2]);
            cpDrawTexture(255, 255, 255,paddle.x, paddle.y, paddle.width, paddle.height, paddle_texture);
            cpDrawTexture(255, 255, 255,ball.x, ball.y, ball.width, ball.height, ball_texture[2]);
            sprintf(msg, "Level: %d Life: %d", Level,Life);//ข้อความแสดงเลขด่าน,จำนวนชีวิต
            cpDrawText(255, 255, 255, 3, 3, msg, small_font, 0);
            sprintf(msg, "Score: %d", score);
            cpDrawText(255, 255, 255, 3, 33, msg, small_font, 0);

            if(!Item.destroyed)//ถ้า Item ยังไม่ถูกทำลาย
                cpDrawTexture(255, 255, 255,Item.x, Item.y, Item.width, Item.height, Item_texture[randomItem]);//สร้างItem ให้ตกจากอิฐ

            if(RocketN && Rocket.destroyed){
                Rocket.x = paddle.x + (paddle.width/2) - 12; //กำหนดให้จรวดยิงออกจากไม้ตี
                Rocket.y = paddle.y;
                Rocket.vel_y = -8;
                Rocket.destroyed = False;
                RocketN--;//จำนวนจรวดลดลง
            }

            if(!Rocket.destroyed)//ถ้าจรวดไม่ถูกทำลาย
                cpDrawTexture(255, 255, 255,Rocket.x, Rocket.y, Rocket.width, Rocket.height, Rocket_texture);//ให้สร้างจรวด

            for(int i = 0; i < 8; i++){
                for (int n = 0; n < n_bricks; n++) {
                    if(!bricks[i][n].destroyed)
                        cpDrawTexture(255, 255, 255,bricks[i][n].x, bricks[i][n].y, bricks[i][n].width, bricks[i][n].height,brick_texture[type[i][n]]);//แสดงผลอิฐแต่ละชนิด
                }
            }

            if(!Item.destroyed && (Item.y + Item.width > WindowHeight)){//เช็คว่า Item ชนขอบล่าง
                Item.x = 0;
                Item.y = 0;
                Item.vel_y = 0;
                Item.destroyed = True;
                //Set Item ใหม่เมื่อเก็บ Item ไม่ได้
            }

            if (ball.y + ball.width > WindowHeight || n_hits == n_bricks*8) {
                if(n_hits == n_bricks*8){
                    cpPlaySound(end_sound);
                    cpDrawText( 255, 255, 255, 400, 350, "MY GOD YOU IS ONE FOR ALL", big_font, 1);
                    running = False;
                }else if(!Life){ //ถ้าจำนวนชีวิตหมดจะจบเกม
                    cpPlaySound(end_sound);
                    cpDrawText( 255, 255, 255, 400, 350, "ENDGAME", big_font, 1);
                    running = False;
                }else{
                    cpPlaySound(end_sound);
                    cpDrawText( 255, 255, 255, 400, 350, "YOU DIE!! ESCAPE FOR THE NEXT LIFE", big_font, 1); 
                    ball.y = WindowHeight-80;
                    ball.x = paddle.x+50;//กำหนดให้ลูกบอลใหม่เริ่มต้นตำแหน่งที่ไม้ตีอยู่
                    ball.vel_y = -ball.vel_y;
                    ball.vel_x = 0;
                    Life--;
                }
                cpSwapBuffers();
                while (1) {
                    cbEventListener(&event);
                    if (event.type == QUIT || event.type == KEYUP && event.key.keysym.sym == K_ESCAPE) {
                        event.type = KEYDOWN;
                        break;
                    }
                }

            }
            cpSwapBuffers();
            if(n_hits == n_bricks && plus){ //เพิ่มความเร็วลูกบอลเมื่ออิฐถูกทำลายไป 11 ก้อน
                if(ball.vel_y < 0)
                    ball.vel_y -= 3;
                else
                    ball.vel_y += 3;
                plus = False;
            }


            while (cbEventListener(&event)) {
                if (event.type == QUIT ||
                    event.type == KEYUP && event.key.keysym.sym == K_ESCAPE) {
                    running = False;
                    break;
                }

                if (event.type == KEYDOWN) {
                    if (event.key.keysym.sym == K_LEFT)
                        paddle.vel_x = -abs(PADDLE_VEL_X);
                    if (event.key.keysym.sym == K_RIGHT)
                        paddle.vel_x = abs(PADDLE_VEL_X);
                }else if (event.type == KEYUP) {
                    if (event.key.keysym.sym == K_LEFT)
                        paddle.vel_x = 0;
                    if (event.key.keysym.sym == K_RIGHT)
                       paddle.vel_x = 0;
                }
            }
            paddle.x += paddle.vel_x;

            if (paddle.x < 0)
                paddle.x = 0;
            if (paddle.x + paddle.width > WindowWidth)
                paddle.x = WindowWidth - paddle.width;

            ball.x += ball.vel_x;
            ball.y += ball.vel_y;

            if(!Item.destroyed)//ถ้า Item ยังไม่ถูกทำลาย
                Item.y += Item.vel_y;//กำหนดให้ Item ตก

            if(!Rocket.destroyed)//ถ้าจรวดไม่ถูกทำลาย
                Rocket.y += Rocket.vel_y;//กำหนดให้จรวดพุ่งขึ้น

            if (Rocket.y < 0) {
                cpPlaySound(hit_top_sound);
                Rocket.destroyed = True;
            }
            if (ball.x < 0 || ball.x + ball.width > WindowWidth){
                cpPlaySound(hit_top_sound);
                ball.vel_x = -ball.vel_x;
            }

            if (ball.y < 0) {
                cpPlaySound(hit_top_sound);
                ball.vel_y = -ball.vel_y;
            }
            for(int i = 0; i < 8; i++){
                for (int n = 0; n < n_bricks; n++) {
                    if (!bricks[i][n].destroyed &&collide(ball, bricks[i][n]) == True) {
                        cpPlaySound(hit_brick_sound);
                        ball.vel_y = -ball.vel_y;
                        if(type[i][n] == 1 || (type[i][n] == 3 && !Item.destroyed)){ //ทำลายอิฐเมื่อ ลูกบอลกระทบอิฐมากกว่า 1 ครั้ง อิฐจะถูกทำลาย หรือ ทำลายอิฐโบนัสเมื่อมี Item ดรอปอยู่
                            bricks[i][n].destroyed = True;
                            n_hits++;
                            score += 10;
                        }
                        else if(type[i][n] == 0){
                            type[i][n] = 1;
                            score += 10;
                        }
                        else if(type[i][n] == 3 && Item.destroyed)//ถ้า Item ยังไม่ถูกทำลาย Item ที่ 2 จะยังไม่ดรอป
                        {
                            bricks[i][n].destroyed = True;
                            n_hits++;
                            randomItem =  rand() % 6;//สุ่มชนิด Item 6 ชนิด
                            Item.x = bricks[i][n].x + bricks[i][n].width/2 - Item.width/2;//กำหนด Item ให้ Item เกิดจุดที่อิฐแตก
                            Item.y = bricks[i][n].y;
                            Item.vel_y = 4;
                            Item.destroyed = False;
                        }else if(type[i][n] == 4)
                        {   
                            ball.vel_x = 3 - rand() % 6;//สุ่มค่า X ที่ลูกบอลจะวิ่งใหม่เมื่อ อิฐชนิดที่ 4 ถูกทำลาย
                            bricks[i][n].destroyed = True;
                            n_hits++;
                            score += 10; 
                        }
                        break;
                    }else if (!bricks[i][n].destroyed &&collide(Rocket, bricks[i][n]) == True) { //ทำลายอิฐเมื่อจรวดยิงโดนอิฐ และ ทำลายจรวด
                            cpPlaySound(hit_paddle_sound);
                            Rocket.x = 0;
                            Rocket.y = 0;
                            Rocket.vel_y = 0;
                            Rocket.destroyed = True;
                            bricks[i][n].destroyed = True;//ทำลายอิฐ
                            n_hits++;
                            score += 10;
                            break;
                    }
                }
            }

            if (!Item.destroyed && collide(Item, paddle) == True){//เมื่อ Item ตกที่ไม้ตี้ Item จะถูกทำลาย(Item แสดงผล)
                cpPlaySound(hit_paddle_sound);
                Item.x = 0;
                Item.y = 0;
                Item.vel_y = 0;
                Item.destroyed = True;
                //Set Item ใหม่เมื่อเราเก็บItem ได้
                if(!randomItem)
                    paddle.width += 50; //เพิ่มขนาดไม้ตี
                else if (randomItem == 1)
                    paddle.width -= 50; //เพิ่มขนาดไม้ตีลดขนาดไม้ตี
                else if (randomItem == 2)
                    Life++;//ชีวิตเพิ่มขึ้น 1 ชีวิต
                else if (randomItem == 3)
                    if(ball.vel_y < 0) //เพิ่มความเร็วลูกบอล
                        ball.vel_y -= 3;
                    else
                        ball.vel_y += 3;
                else if (randomItem == 4)
                    if(ball.vel_y < 0 && ball.vel_y + 3) //ลดความเร็วลูกบอล //ป้องกันการที่ลูกบอลความเร็วเป็น 0
                        ball.vel_y += 3;
                    else if(ball.vel_y - 3)
                        ball.vel_y -= 3;
                    else if (ball.vel_y < 0) //ป้องกันการที่ลูกบอลความเร็วเป็น 0
                        ball.vel_y = 1;
                    else
                        ball.vel_y = -1;
                else if(randomItem == 5)
                        RocketN += 3;
                    
            }

            if (collide(ball, paddle) == True) {
                cpPlaySound(hit_paddle_sound);
                ball.vel_y = -ball.vel_y;
                ball.vel_x = ChackPoint(ball,paddle); //เปลี่ยนทิศทางบอลตามเงื่อนไขในฟังชั่น
            }

            cpDelay(10);
        }
    }//จบ Level 3

    running = True;
    while (running)//สร้างแสดงคะแนน
    {
        cpClearScreen();
        cpDrawTexture(255, 255, 255,0, 0, WindowWidth, WindowHeight, background_texture[3]); //แสดง Wallpaper มาตรฐาน
        sprintf(msg, "Score: %d", score); // แสดงคะแนนที่ผู้เล่นได้
        cpDrawText(255, 255, 255, 300, 300, msg, big_font, 0);
        if(score < 1500) //แสดงข้อความตามเงื่อนไขแต่ละคะแนน
            cpDrawText( 255, 255, 255, 400, 400, "YOUR ARE POOR CHICKEN", big_font, 1);
        else if(score >= 1500 && score <= 3500)
            cpDrawText( 255, 255, 255, 400, 400, "YOUR ARE CHICKEN", big_font, 1);
        else
            cpDrawText( 255, 255, 255, 400, 400, "YOUR ARE CHICKEN HAVE DEVELOPMENT", big_font, 1);
        cpSwapBuffers();
        while (cbEventListener(&event)) {
                if (event.type == QUIT ||event.type == KEYUP && event.key.keysym.sym == K_ESCAPE){ //เมื่อกด ESC จะปิดเกม
                    running = False;
                    break;
                }
        }
        cpDelay(10);
    }
    cpCleanUp();
    return 0;
}
