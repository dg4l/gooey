# gooey
experimental ui framework for [raylib](https://github.com/raysan5/raylib) :)

## use cases

basically should only be used for clobbering together a quick and ugly debug ui

## creating elements and binding them to a pane 

```c
bool val = 0;
Pane* pane = create_pane(0, 0, 640, 900, 5, BLUE);
UiElement* textbutton = create_text_button("click me", 0, 20, 150, 50, 25, DARKGRAY, RED, &val);
bind_element_to_pane(textbutton, pane);
```

then, simply run `draw_pane(pane);` in your main loop :)

the bool `val` will be toggled when the button is pressed, allowing for dynamic behavior

