#pragma once
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

enum GOOEY_ELEMENTS{
    GOOEY_BUTTON,
    GOOEY_TEXT_BUTTON,
    GOOEY_TEXT_BOX,
    GOOEY_TEXT_INPUT
};

typedef struct GooVec2{
    int x;
    int y;
}GooVec2;

typedef struct UiElement{
    uint16_t type;
    void* pElement;
}UiElement;

// consider letting each pane set their own font for every element
typedef struct Pane{
    int posx;
    int posy;
    //GooVec2 pos;
    int width;
    int height;
    // TODO: if title is null, draw pane with no title bar, maybe create a separate function to add a title to a pane?
    char* title;
    UiElement** elements;
    uint16_t elementCount;
    uint16_t max_elements;
    Color background_color;
}Pane;

typedef struct UiCtx{
    Pane* panes;
    int width;
    int height;
    int posX;
    int posY;
    uint16_t max_panes;
}UiCtx;

typedef struct Button{
    Rectangle normalBounds;
    Rectangle bounds;
    Color normalColor;
    Color hoverColor;
    bool* toggle;
    bool clickable;
}Button;

typedef struct TextButton{
    Button* button;
    int fontSize;
    char* label;
}TextButton;

typedef struct TextInput{
    TextButton* tb;
    char* buf;
    // TODO: implement later
    //int keyPressed;
    int bufsize;
    int txtpos;
    bool allowInput;
}TextInput;

typedef struct TextBox{
    char* label;
    int fontsize;
    int posx;
    int posy;
}TextBox; 


void draw_ui_ctx(UiCtx* ctx);
bool bind_element_to_pane(UiElement* el, Pane* pane);
UiCtx* init_gooey_ctx(int w, int h, int posx, int posy, uint16_t max_panes);
TextBox* create_text_box_intrinsic(char* label, int posx, int posy, int fontsize);
Pane* create_pane(int posx, int posy, int w, int h, uint16_t max_elements, Color background_color);
UiElement* text_box_to_element(TextBox* textbox);
TextButton* create_text_button_intrinsic(char* text, int posx, int posy, int width, int height, int fontSize, Color normalColor, Color hoverColor, bool* toggle);
UiElement* create_text_button(char* text, int posx, int posy, int width, int height, int fontSize, Color normalColor, Color hoverColor, bool* toggle);
Button* create_button(int posx, int posy, int width, int height, Color normalColor, Color hoverColor, bool* toggle);
UiElement* create_text_input(char* buf, int bufsize,int posx, int posy, int width, int height, int fontSize, Color color);
TextInput* create_text_input_intrinsic(char* buf, int bufsize,int posx, int posy, int width, int height, int fontSize, Color color);
void kill_pane(Pane** pane);
UiElement* text_button_to_element(TextButton* tb);
void draw_text_box(TextBox* tb);
void draw_button(Button* button);
void draw_text_button(TextButton* tb);
void draw_pane(Pane* pane);
bool button_hovered(Button* button);
bool button_clicked(Button* button);
