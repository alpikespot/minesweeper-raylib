#include "raylib.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <cstdlib>
#include <time.h>
#include <bits/stdc++.h>
#include "raygui.h"
using namespace std;
int gameDifficulty = 0; //0=beginner, 1=intermediate, 2=expert
bool verysecreteasteregg = 0;
const int maxTileNumX = 50;
const int maxTileNumY = 50;
const int diffData[3][3] = {{9,9,10}, {16,16,40}, {16,30,99}};
const int tileSize = 32;
const int screenSizesX[3] = {diffData[0][1]*tileSize, diffData[1][1]*tileSize, diffData[2][1]*tileSize};
const int maxMineNum = 200;
const int HUDSize = tileSize*4;
const int bezel = tileSize;
int tileMap[maxTileNumY][maxTileNumX] = { 0 };
int flags[maxTileNumX*maxTileNumY][2];
int minePos[maxMineNum][2] = {0};
int flagNum;
int tick = 0;
int randval = 0;
int mNum = 9;
int tNumX = 10;
int tNumY = 10;
double gameStart = 0;
double currGameTime;
bool GUIDropDownEdit = false;

int clicks = 0;
void checkSurrounding(int (&plMap)[maxTileNumY][maxTileNumX], int tileMap[maxTileNumY][maxTileNumX], int chkX, int chkY){
    int mineCounter = 0;
    
    
    for(int x=-1; x<2; x++){
        
        for(int y=-1; y<2; y++){
            
            int ty = chkY+y;
            int tx = chkX+x;
            
            if ( !(ty < 0 || ty > tNumY-1 || tx < 0 || tx > tNumX-1)) {
                int scannedTileValue = tileMap[ty][tx];
                if (scannedTileValue == 1){
                    mineCounter++;
                }
            }


        }
    }
    if (mineCounter == 0){
        plMap[chkY][chkX] = -3; // -3 = verified empty
        for(int x=-1; x<2; x++) { for(int y=-1; y<2; y++){
            int ty = chkY+y;
            int tx = chkX+x;
            
            if ( !(ty < 0 || ty > tNumY-1 || tx < 0 || tx > tNumX-1) ) {
            if (plMap[ty][tx]==0 ){
                checkSurrounding(plMap,tileMap, tx, ty);
                }
            }
            } }
    }
    else {
        plMap[chkY][chkX] = mineCounter;
    }
    
}

float clip(float n, float lower, float upper) {
  return max(lower, min(n, upper));
}

void checkSurroundingFirstTime(int (&plMap)[maxTileNumY][maxTileNumX], int (&tileMap)[maxTileNumY][maxTileNumX], int chkX, int chkY, int (&minePos)[maxMineNum][2]){
    int mineCounter = 0;
    int randX;
    int randY;
    int mineIdx;
    
    for(int x=-1; x<2; x++){
        
        for(int y=-1; y<2; y++){
            
            int ty = chkY+y;
            int tx = chkX+x;
            
            if ( !(ty < 0 || ty > tNumY-1 || tx < 0 || tx > tNumX-1)) {
                int scannedTileValue = tileMap[ty][tx];
                if (scannedTileValue != 0){
                    
                    //find mine idx

                    for (int i=0; i<mNum; i++){
                        if (minePos[i][0] == ty && minePos[i][1] == tx) {
                            mineIdx = i;
                            break;
                        }
                    }


                    tileMap[ty][tx] = 0;
                    do{
                        randX = rand()%tNumX;
                        randY = rand()%tNumY;
                    } while (tileMap[randY][randX] != 0 || (randY >= chkY-1 && randX >= chkX-1 && randY <= chkY+1 && randX <= chkX+1));
                    tileMap[randY][randX] = 1;
                    minePos[mineIdx][0] = randY;
                    minePos[mineIdx][1] = randX;
                    
                }
            }


        }
    }
    
    if (mineCounter == 0){
        plMap[chkY][chkX] = -3; // -3 = verified empty
        for(int x=-1; x<2; x++) { for(int y=-1; y<2; y++){
            int ty = chkY+y;
            int tx = chkX+x;
            
            if ( !(ty < 0 || ty > tNumY-1 || tx < 0 || tx > tNumX-1) ) {
            if (plMap[ty][tx]==0 ){
                checkSurrounding(plMap,tileMap, tx, ty);
                }
            }
            } }
    }
    
}

void setMines(int (&tileMap)[maxTileNumY][maxTileNumX], int (&minePos)[maxMineNum][2]){
    for (int i=0;i<mNum;i++){
        int randX = rand()%tNumX;
        int randY = rand()%tNumY;

        do{
            randX = rand()%tNumX;
            randY = rand()%tNumY;
        }
        while(tileMap[randY][randX] == 1);
        
        tileMap[randY][randX] = 1;
        minePos[i][0] = randY;
        minePos[i][1] = randX;
        
    }
}

int main(void)
{
    bool showMessageBox;
    bool gameReset = true;
    int playerMap[maxTileNumY][maxTileNumX] = { 0 };
    int gameState = 0; //0=unfinished, 1=win, -1=lose, 2= settings
    int screenWidth  = tNumX * tileSize + bezel*2;
    int screenHeight = tNumY * tileSize + HUDSize;
    bool sp1Edit = false; bool sp2Edit = false; bool sp3Edit = false;
    int sp1Val = tNumX; int sp2Val = tNumY; int sp3Val = mNum; 
    srand(time(0)); 
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "raylib [audio] example - music playing (streaming)");
    InitAudioDevice();

    Sound AAAA = LoadSound("/home/antonspc/Other/raylibMinesweeper/src/aaaa.wav");
    SetTargetFPS(60);      
    int counter = 0;
    enum textures {
        flagImg, mineImg, xImg, faceded, facenormal, faceclick, facewin, facewin1, facewin2, facewin3, facewin0
    };

    Texture2D imgs[11];
    const char* imgNames[11] = {"img/flag.png", "img/mine.png", "img/no.png", "img/facedead.png", "img/facenormal.png", "img/faceclick.png", "img/facewin.png", "img/facewin1.png", "img/facewin2.png", "img/facewin3.png", "img/facewin0.png"};
    for (int i=0; i<11;i++){
        Image tempimg = LoadImage(imgNames[i]);
        ImageResize(&tempimg, int(tempimg.width*float(tileSize)/32),tempimg.height*float(tileSize)/32);
        imgs[i] = LoadTextureFromImage(tempimg);
    }
    
    
    while (!WindowShouldClose())   
    {
        Vector2 mPos = (Vector2){int((GetMousePosition().x-bezel) / tileSize), int((GetMousePosition().y -HUDSize)/ tileSize)};
        Vector2 mScrPos = GetMousePosition();
        bool validMouse = (mPos.x > -1 && mPos.x < tNumX && mPos.y > -1 && mPos.y < tNumY);
        if (gameReset) {
            verysecreteasteregg = false;
            if (screenWidth != screenSizesX[gameDifficulty]+ bezel*2){
                if (gameDifficulty != 3){
                    tNumX = diffData[gameDifficulty][1];
                    tNumY = diffData[gameDifficulty][0];
                    mNum = diffData[gameDifficulty][2];
                }
                screenWidth  = tNumX * tileSize + bezel*2;
                screenHeight = tNumY * tileSize + HUDSize;
                SetWindowSize(screenWidth, screenHeight);
            }
            memset(tileMap, 0, sizeof(tileMap));
            memset(playerMap, 0, sizeof(playerMap));
            memset(minePos, 0, sizeof(minePos));
            memset(flags, 0, sizeof(flags));
            flagNum = 0;
            gameState = 0; 
            setMines(tileMap, minePos);
            clicks = 0;
            gameReset = false;
            currGameTime = 0;
        }

        if(gameState == 0 && validMouse){
            int x = int(mPos.x); int y = int(mPos.y);
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && playerMap[y][x] == 0){
                if (clicks){
                    if (tileMap[y][x] != 1){
                    checkSurrounding(playerMap, tileMap, x, y);
                    }
                    else {
                        //Game over
                        playerMap[y][x] = -2;
                        for (int i = 1000; i < mNum; i++){
                            int mX = minePos[i][1]; int mY = minePos[i][0];
                            playerMap[mY][mX] = -2;
                        }
                        gameState = -1;
                    }
                }
                else {
                    checkSurroundingFirstTime(playerMap, tileMap, x, y, minePos);
                    gameStart = time(0);
                }
                clicks++;
            }
            
            else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
                if (playerMap[y][x] == 0){
                    playerMap[y][x] = -1;
                    flags[flagNum][0] = y; flags[flagNum][1] = x;
                    flagNum++;
                    
                } else if (playerMap[y][x] == -1){

                    playerMap[y][x] = 0;
                    for (int flag=0;flag<flagNum;flag++){
                        if (flags[flag][1] == x && flags[flag][0] == y){
                            flags[flag][0] = 0; flags[flag][1] = 0;
                            for (int fRep = flag; fRep<flagNum; fRep++){
                                flags[fRep][1] = flags[fRep+1][1];
                                flags[fRep][0] = flags[fRep+1][0]; 
                            }
                            break;
                        }
                    }
                    flagNum--;
                }
            }
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {

                //check if game is won
                bool correctFlag;
                bool noUncheckedTiles = true;
                if (flagNum == mNum){
                        for (int flagIdx=0; flagIdx<flagNum; flagIdx++){
                            
                            correctFlag = false;
                            for (int mineIdx=0; mineIdx<mNum; mineIdx++) {
                                if (minePos[mineIdx][1] == flags[flagIdx][1] && minePos[mineIdx][0] == flags[flagIdx][0]) {
                                    correctFlag = true;
                                    break;
                                }
                            }
                            if (correctFlag){
                                continue;
                            }
                            else {
                                break;
                            }
                        }
                        for (int x=0;x<tNumX;x++) {
                            if (!noUncheckedTiles) {
                                break;
                                }
                            for (int y=0;y<tNumY;y++) { 
                                if(playerMap[y][x]==0) { 
                                    noUncheckedTiles = false;
                                    break;}
                                    }
                                }
                        if (correctFlag && noUncheckedTiles){
                            gameState = 1;
                            if (rand()%2 == 0){
                                verysecreteasteregg = true;
                                
                                PlaySound(AAAA);
                            }

                        }
                    }
            }
        }
        BeginDrawing();
            
            ClearBackground(RAYWHITE);
            if (gameState==2){ //settings
            if (screenHeight<400 || screenWidth<200) {
                SetWindowSize(400,300);
            }
                if (gameDifficulty != 3){
                    sp1Val = diffData[gameDifficulty][1];
                    sp2Val = diffData[gameDifficulty][0];
                    sp3Val = diffData[gameDifficulty][2];
                }
                else {
                    tNumX = sp1Val;
                    tNumY = sp2Val;
                    mNum = sp3Val;
                }
                
                if (true){
                    DrawText("Horizontal size:", 25,tileSize*3, tileSize/2, GRAY);
                    if (GuiSpinner((Rectangle){ MeasureText("Horizontal size:", tileSize/2)+40, tileSize*2.75, tileSize*3, tileSize }, NULL, &sp1Val, 4, 35, sp1Edit)) 
                        sp1Edit = !sp1Edit;

                    DrawText("Vertical size:", 25,tileSize*5, tileSize/2, GRAY);
                    if (GuiSpinner((Rectangle){ MeasureText("Vertical size:", tileSize/2)+40, tileSize*4.75, tileSize*3, tileSize }, NULL, &sp2Val, 4, 35, sp2Edit)) 
                        sp2Edit = !sp2Edit;
                    
                    DrawText("Custom mine count:", 25,tileSize*7, tileSize/2, GRAY);
                    if (GuiSpinner((Rectangle){ MeasureText("Custom mine count:", tileSize/2)+40, tileSize*6.75, tileSize*3, tileSize }, NULL, &sp3Val, 0, max(min(tNumX*tNumY-9, 150), 0), sp3Edit)) 
                        sp3Edit = !sp3Edit;
                }
                GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
                GuiSetStyle(DROPDOWNBOX, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
                DrawText("Game difficulty:", 25, tileSize, tileSize/2, GRAY);
                if (GuiDropdownBox((Rectangle){ MeasureText("Game difficulty:", tileSize/2)+30, tileSize*0.75, tileSize*3, tileSize }, "Beginner;Intermediate;Expert;Custom", &gameDifficulty, GUIDropDownEdit)) GUIDropDownEdit = !GUIDropDownEdit;
                if (GuiButton((Rectangle){ 25, screenHeight-tileSize*2, tileSize*3, tileSize}, "Back")) 
                {gameState=-1; gameReset = 1;}

            } else {
                    //draw settings
                    DrawRectangle(0, HUDSize, screenWidth, screenHeight-HUDSize, (Color){200,200,200,255});
                    int sx = 0; int sy=0; int sh = 16; int sw = MeasureText("settings", sh);
                    DrawRectangle(sx, sy, sw, sh, (Color){200,200,200,255});
                    DrawText("settings", sx, sy, sh, BLACK);
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mScrPos.x >sx && mScrPos.x<sx+sw && mScrPos.y >sy && mScrPos.y<sy+sh){
                        gameState = 2;
                        sp1Val = tNumX; sp2Val = tNumY; sp3Val = mNum; 
                    }
                //head drawing and code
                int HeadWidth = imgs[facenormal].width;
                Vector2 FacePos = {screenWidth/2-HeadWidth/2, HUDSize/4};
                Color colors[3] = {RED, YELLOW, GREEN};
                Color txtColors[8] = {BLUE, DARKGREEN, RED, DARKBLUE, DARKBROWN, GRAY, DARKGRAY, BLACK };
                if (gameState==1) 
                {
                    if (verysecreteasteregg){
                        if (tick%4==0){
                            randval = facewin1 + rand()%3;
                        }
                        DrawTexture(imgs[randval], FacePos.x, FacePos.y, WHITE);
                    }
                    else {
                        DrawTexture(imgs[facewin], FacePos.x, FacePos.y, WHITE);
                    }
                    
                }
                else if (gameState==-1){DrawTexture(imgs[faceded], FacePos.x, FacePos.y, WHITE);}
                else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && validMouse && playerMap[int(mPos.y)][int(mPos.x)] == 0) 
                    {DrawTexture(imgs[faceclick], FacePos.x, FacePos.y, WHITE);}
                else {DrawTexture(imgs[facenormal], FacePos.x, FacePos.y, WHITE);}
                
                if (mScrPos.x > FacePos.x && mScrPos.y > FacePos.y && mScrPos.x < FacePos.x+HeadWidth && mScrPos.y < FacePos.y+HeadWidth && IsMouseButtonPressed(0)){
                    gameReset = true;
                }
                if (clicks >0 && gameState == 0) {
                    currGameTime = time(0) - gameStart;
                }
                
                DrawRectangle(0, HUDSize/4, tileSize*2, tileSize, BLACK);
                DrawText(TextFormat("%i", int(currGameTime)), 4, HUDSize/4+2, tileSize, RED);

                DrawRectangle(screenWidth-(tileSize*2), HUDSize/4, tileSize*2, tileSize, BLACK);
                DrawText(TextFormat("%i", mNum-flagNum), screenWidth-(tileSize*2)+4, HUDSize/4+2, tileSize, RED);

                //draw tiles
                for(int y=0; y<tNumY ; y++){
                    for(int x=0; x<tNumX ; x++){
                        int tX = x*tileSize+1+bezel; int tY = y*tileSize+1+HUDSize;

                        Color tilecolor = GRAY;
                        if (playerMap[y][x] > 0){
                            tilecolor = (Color){240,240,240,255};
                        }else if (playerMap[y][x] == -2){
                            tilecolor = RED;
                        }else if (playerMap[y][x] == -3){
                            tilecolor = (Color){240,240,240,255};
                        }
                        
                        //highlight selected tile
                        if (mPos.x == x && mPos.y == y && playerMap[y][x] != -3 && playerMap[y][x] <= 0){      
                            tilecolor.r = clip(tilecolor.r - 50,0,255);
                            tilecolor.g = clip(tilecolor.g - 50,0,255);
                            tilecolor.b = clip(tilecolor.b - 50,0,255);
                            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && gameState == 0 && playerMap[y][x] != -1) {
                                tilecolor = (Color){240,240,240,255};
                                }
                            }
                        DrawRectangle(tX, tY, tileSize-2, tileSize-2, tilecolor);

                        if (playerMap[y][x] > 0) {
                                const char* txt = TextFormat("%i", playerMap[y][x]);
                                DrawText(txt, (tileSize/2 - (MeasureText(txt, tileSize))/2)+tileSize*x+bezel, tileSize*y+HUDSize, tileSize, txtColors[playerMap[y][x]-1]);

                        }
                        if (playerMap[y][x] == -1){
                            DrawTexture(imgs[flagImg], tX-1,tY-1, WHITE); }
                    }  
                }
                if (gameState == -1) {
                    for (int i=0;i<mNum;i++){
                        int x=minePos[i][1]; int y=minePos[i][0];
                        if (playerMap[y][x] != -1) {
                            int tX = x*tileSize+1+bezel; int tY = y*tileSize+1+HUDSize;
                            DrawTexture(imgs[mineImg], tX-1,tY-1, WHITE);
                        }
                    }
                    for (int i=0;i<flagNum;i++){
                        int x=flags[i][1]; int y=flags[i][0];
                        if (tileMap[y][x] == 0) {
                            int tX = x*tileSize+1+bezel; int tY = y*tileSize+1+HUDSize;
                            DrawTexture(imgs[mineImg], tX-1,tY-1, WHITE);
                            DrawTexture(imgs[xImg], tX-1,tY-1, WHITE);
                        }
                    }
                }
            }
        EndDrawing();
        tick++;
    }
    UnloadSound(AAAA); 
    CloseAudioDevice();        
    CloseWindow();             
    return 0;
}
