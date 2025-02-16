#include "gooey.h"
#include <string.h>

// hierarchy: ui context/state -> panes -> elements (can't be a pane, maybe could be in the future) 
// implement button hierarchy, two should not be selected at once, one rendered most recently should take priority
void draw_button(Button* button){
    Color color = button->normalColor;
    if (button_hovered(button)){
       color = button->hoverColor; 
    }
    if(button_clicked(button) && button->toggle){
        *button->toggle = !*button->toggle;
    }
    DrawRectangle(button->bounds.x, button->bounds.y, button->bounds.width, button->bounds.height, color);
}

// TODO: center text based on length of text and width/height of button and font size 
// create visual distinction between TextButtons and TextBoxes
void draw_text_button(TextButton* tb){
    draw_button(tb->button);
    DrawText(tb->label, tb->button->bounds.x + 2, tb->button->bounds.y + tb->button->bounds.height/2, tb->fontSize, WHITE);
}

void draw_text_box(TextBox* tb){
    DrawText(tb->label, tb->posx, tb->posy, tb->fontsize, WHITE);
}

//travel through pane and kill all subelements
void kill_pane(Pane** pane){
    if (*pane){
        for (size_t i = 0; i < (*pane)->elementCount; ++i){
            free((*pane)->elements[i]);
            (*pane)->elements[i] = NULL;
        }
        free(*pane);
        *pane = NULL;
    }
}

// if not bound to a pane, normalBounds and bounds will be the same 
Button* create_button(int posx, int posy, int width, int height, Color normalColor, Color hoverColor, bool* toggle){
    Button* button = malloc(sizeof(Button));
    button->bounds.x = posx;
    button->bounds.y = posy;
    button->normalBounds.x = button->bounds.x;
    button->normalBounds.y = button->bounds.y;
    button->bounds.width = width;
    button->bounds.height = height;
    button->normalColor = normalColor;
    button->hoverColor = hoverColor;
    button->toggle = toggle;
    return button;
}


UiCtx* init_gooey_ctx(int w, int h, int posx, int posy, uint16_t max_panes){
    UiCtx* ctx = malloc(sizeof(UiCtx));
    ctx->width = w;
    ctx->height = h;
    ctx->posX = posx;
    ctx->posY = posy;
    ctx->max_panes = max_panes;
    ctx->panes = malloc(sizeof(Pane*) * max_panes);
}

// remember to make all elements relative to the pane 
void draw_pane(Pane* pane){
    if (pane){
        DrawRectangle(pane->posx, pane->posy, pane->width, pane->height, pane->background_color);
        for (int i = 0; i < pane->elementCount; ++i){
            switch(pane->elements[i]->type){
                case GOOEY_TEXT_BUTTON:
                    TextButton* tb = pane->elements[i]->pElement;
                    tb->button->bounds.x = pane->posx + tb->button->normalBounds.x;
                    tb->button->bounds.y = pane->posy + tb->button->normalBounds.y;
                    draw_text_button(pane->elements[i]->pElement);
                    break;
                case GOOEY_BUTTON:
                    Button* b = pane->elements[i]->pElement;
                    b->bounds.x = pane->posx + b->normalBounds.x;
                    b->bounds.y = pane->posy + b->normalBounds.y;
                case GOOEY_TEXT_BOX:
                    printf("not implemented\n");
                    break;
            }
        }
    }
}

Pane* create_pane(int posx, int posy, int w, int h, uint16_t max_elements, Color background_color){
    Pane* pane = malloc(sizeof(Pane));
    pane->posx = posx;
    pane->posy = posy;
    pane->width = w;
    pane->height = h;
    pane->max_elements = max_elements;
    pane->elements = malloc(sizeof(UiElement*) * max_elements);
    pane->elementCount = 0;
    pane->background_color = background_color;
    return pane;
}

bool bind_element_to_pane(UiElement* el, Pane* pane){
    if (pane->elementCount < pane->max_elements){
        pane->elements[pane->elementCount] = el;
        if (&pane->elements[pane->elementCount]){
            ++pane->elementCount;
            return 1;
        }
        else{
            return 0;
        }
    }
    else{
        return 0;
    }
}

UiElement* text_box_to_element(TextBox* textbox){
    UiElement* el = malloc(sizeof(UiElement));
    el->type = GOOEY_TEXT_BOX;
    el->pElement = textbox;
    return el;
}

UiElement* text_button_to_element(TextButton* tb){
    UiElement* el = malloc(sizeof(UiElement));
    el->type = GOOEY_TEXT_BUTTON;
    el->pElement = tb;
    return el;
}

UiElement* create_text_button(char* text, int posx, int posy, int width, int height, int fontSize, Color normalColor, Color hoverColor, bool* toggle){
    TextButton* tb = create_text_button_intrinsic(text, posx, posy, width, height, fontSize, normalColor, hoverColor, toggle);
    UiElement* el = text_button_to_element(tb);
    return el;
}

TextButton* create_text_button_intrinsic(char* text, int posx, int posy, int width, int height, int fontSize, Color normalColor, Color hoverColor, bool* toggle){
    TextButton* tb = malloc(sizeof(TextButton));
    tb->button = create_button(posx, posy, width, height, normalColor, hoverColor, toggle);
    tb->fontSize = fontSize;
    tb->label = text;
    return tb;
}

UiElement* create_text_box(char* label, int posx, int posy, int fontsize){
    TextBox* textbox = create_text_box_intrinsic(label, posx, posy, fontsize);
    UiElement* el = text_box_to_element(textbox);
    return el;
}

TextBox* create_text_box_intrinsic(char* label, int posx, int posy, int fontsize){
    TextBox* tb = malloc(sizeof(TextBox));
    tb->label = label;
    tb->posx = posx;
    tb->posy = posy;
    tb->fontsize = fontsize;
}

bool button_hovered(Button* button){
    Rectangle mpR = {0, 0, 5, 5};
    mpR.x = GetMouseX();
    mpR.y = GetMouseY();
    return (CheckCollisionRecs(button->bounds, mpR));
}

bool button_clicked(Button* button){
    if (button_hovered(button) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        return 1;
    }
    return 0;
}
