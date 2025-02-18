#include "gooey.h"
#include <string.h>

// hierarchy: ui context/state -> panes -> elements 
// TODO: implement button precedence, two should not be selected at once, one rendered most recently should take priority
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

bool isBullshitKey(int key){ 
    if (key == KEY_ENTER || key == KEY_TAB || key == KEY_LEFT_CONTROL || key == KEY_LEFT_ALT || key == KEY_RIGHT_CONTROL || key == KEY_RIGHT_ALT || key == KEY_LEFT_SHIFT || key == KEY_RIGHT_SHIFT || key == KEY_UP || key == KEY_DOWN){
        return 1;
    }
    else{
        return 0;
    }
}

bool handleBackspace(TextInput* ti){
        if (ti->curPos <= 0){
            return 0;
        }
        if (ti->curPos == ti->txtLength){
            ti->buf[ti->curPos-1] = '\0';
        }
        else{
            ti->buf[ti->curPos-1] = ' ';
        }
        --ti->curPos;
        --ti->txtLength;
}

void handleTextInputKeypress(TextInput* ti){
    int key = GetKeyPressed();
    if (key){
        while (key > 0){
            if (isBullshitKey(key)){
                break;
            }
            else if (ti->curPos < ti->bufsize){
                if (key == 259){
                    if (!handleBackspace(ti)){
                        return;
                    }
                }
                else if (key == KEY_LEFT){
                    if (ti->curPos > 0){
                        --ti->curPos;
                    }
                }
                else if (key == KEY_RIGHT){
                    if (ti->curPos < ti->txtLength){
                        ++ti->curPos;
                    }
                }
                else{
                    ti->buf[ti->curPos] = key;
                    if (ti->curPos == ti->txtLength){
                        ti->buf[ti->curPos+1] = '\0';
                        ++ti->txtLength;
                    }
                    ++ti->curPos;
                }
                key = GetKeyPressed();
            }
        }
    }
}


// FIXME: don't allow multiple of these shits to be selected, maybe handle in a context struct
// TODO: handle shift combos, for now we ignore shift 
// TODO: clean up this horrible function 
void draw_text_input(TextInput* ti){
    draw_text_button(ti->tb);
    // in case of overflow somehow
    if (ti->curPos > ti->bufsize){
        ti->curPos = ti->bufsize-1;
    }
    if (*(ti->tb->button->toggle)){
        handleTextInputKeypress(ti);
    }
}

void draw_text_box(TextBox* tb){
    DrawText(tb->label, tb->posx, tb->posy, tb->fontsize, WHITE);
}


// TODO: this might leak!
void kill_pane(Pane** pane){
    if (*pane){
        for (size_t i = 0; i < (*pane)->elementCount; ++i){
            switch((*pane)->elements[i]->type){
                case GOOEY_TEXT_INPUT:
                    TextInput* ti = (*pane)->elements[i]->pElement;
                    free(ti->tb->button);
                    free(ti->tb);
                    free(ti);
                    break;
                case GOOEY_TEXT_BUTTON:
                    TextButton* tb = (*pane)->elements[i]->pElement;
                    free(tb->button);
                    free(tb);
                    break;
                case GOOEY_BUTTON:
                    Button* button = (*pane)->elements[i]->pElement;
                    free(button);
                    break;
            }
            (*pane)->elements[i] = NULL;
        }
        free(*pane);
        *pane = NULL;
    }
}

UiElement* create_text_input(char* buf, unsigned int bufsize, int posx, int posy, int width, int height, unsigned int fontSize, Color color){
    UiElement* el = malloc(sizeof(UiElement));
    el->pElement = create_text_input_intrinsic(buf, bufsize, posx, posy, width, height, fontSize, color);
    el->type = GOOEY_TEXT_INPUT;
    return el;
}

TextInput* create_text_input_intrinsic(char* buf, unsigned int bufsize, int posx, int posy, int width, int height, unsigned int fontSize, Color color){
    TextInput* ti = malloc(sizeof(TextInput));
    ti->tb = create_text_button_intrinsic(buf, posx, posy, width, height, fontSize, color, color, &ti->allowInput);
    ti->buf = buf;
    ti->bufsize = bufsize;
    // TODO: implement this later
    //ti->keyPressed = 0;
    ti->allowInput = 0;
    return ti;
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
                case GOOEY_TEXT_INPUT:
                    TextInput* ti = pane->elements[i]->pElement;
                    ti->tb->button->bounds.x = pane->posx + ti->tb->button->normalBounds.x;
                    ti->tb->button->bounds.y = pane->posy + ti->tb->button->normalBounds.y;
                    draw_text_input(ti);
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
    }
    return 0;
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

UiElement* create_text_button(char* text, int posx, int posy, int width, int height, unsigned int fontSize, Color normalColor, Color hoverColor, bool* toggle){
    TextButton* tb = create_text_button_intrinsic(text, posx, posy, width, height, fontSize, normalColor, hoverColor, toggle);
    UiElement* el = text_button_to_element(tb);
    return el;
}

TextButton* create_text_button_intrinsic(char* text, int posx, int posy, int width, int height, unsigned int fontSize, Color normalColor, Color hoverColor, bool* toggle){
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
