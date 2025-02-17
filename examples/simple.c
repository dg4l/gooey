#include "../gooey.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

void swap_pane_pos(Pane* p1, Pane* p2){
    if (!p1 || !p2){
        return;
    }
    Pane temp_pane;
    temp_pane.posx = p1->posx;
    temp_pane.posy = p1->posy;
    temp_pane.width = p1->width;
    temp_pane.height = p1->height;
    p1->posx = p2->posx;
    p1->posy = p2->posy;
    p1->width = p2->width;
    p1->height = p2->height;
    p2->posx = temp_pane.posx;
    p2->posy = temp_pane.posy;
    p2->height = temp_pane.height;
    p2->width = temp_pane.width;
}


int main(void){
    int val = 0;
    bool dec = 0;
    bool inc = 0;
    bool shouldDelete = 0;
    char cValbuf[10];
    char* inbuf = malloc(sizeof(char) * 256);
    int txtpos = 0;
    Pane* testpane = create_pane(0, 0, 640, 900, 5, GRAY);
    Pane* testpane2 = create_pane(640, 0, 640, 900, 5, LIGHTGRAY);
    UiElement* decButton = create_text_button("    -", 0, 20, 150, 50, 25, DARKGRAY, RED, &dec);
    UiElement* incButton = create_text_button("    +", 260, 20, 150, 50, 25, DARKGRAY, GREEN, &inc);
    UiElement* txtDisplay = create_text_button("Text Input:", 50, 100, 150, 50, 25, DARKGRAY, DARKGRAY, NULL);
    UiElement* valDisplay = create_text_button(cValbuf, 120, 90, 150, 50, 25, DARKGRAY, RED, NULL);
    UiElement* delButton = create_text_button("DEALLOC!", 300, 90, 150, 50, 25, RED, GREEN, &shouldDelete);
    UiElement* txtinput = create_text_input(inbuf, 50, 50, 150, 400, 125, 25, DARKGRAY);
    bind_element_to_pane(decButton, testpane);
    bind_element_to_pane(incButton, testpane);
    bind_element_to_pane(valDisplay, testpane);
    bind_element_to_pane(delButton, testpane);
    bind_element_to_pane(txtinput, testpane2);
    bind_element_to_pane(txtDisplay, testpane2);
    InitWindow(1280, 900, "test");
    while (!WindowShouldClose()){
        sprintf(cValbuf, "    %d", val);
        BeginDrawing();
        ClearBackground(BLACK);
        draw_pane(testpane);
        draw_pane(testpane2);
        if (dec){
            --val;
            dec = 0;
        }
        if (inc){
            ++val;
            inc = 0;
        }
        if (shouldDelete){
            kill_pane(&testpane);
            shouldDelete = 0;
        }
        if (IsKeyPressed(KEY_ENTER)){
            swap_pane_pos(testpane, testpane2);
        }
        EndDrawing();
    }
    kill_pane(&testpane);
    kill_pane(&testpane2);
    free(inbuf);
    inbuf = NULL;
    CloseWindow();
    return 0;
}
