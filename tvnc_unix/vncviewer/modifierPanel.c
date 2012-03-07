/* 
 * Implement modifier panel to send combination of key strokes with different
 * modifier keys
 *
 */
#include <vncviewer.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Toggle.h>
#include <X11/Xmu/Converters.h>

enum MODIFIER_BUTTON_INDEX {
    MOD_ALT_L = 0,
    MOD_ALT_R,
    MOD_CTRL_L,
    MOD_CTRL_R,
    MOD_SHIFT_L,
    MOD_SHIFT_R,
    MOD_WIN_KEY,
    MOD_TRANSPOSE,
    MOD_MAX_IDX
};

GC gc;
GC srcGC, dstGC; /* used for debugging copyrect */
Window desktopWin;
Cursor dotCursor;
Widget form, viewport, desktop;
Widget modPanel;

static Widget modifierPanelButtons[MOD_MAX_IDX] = { 0 };
static CARD32 modifierButtonsKeycodes[MOD_MAX_IDX] = { 0 };
static short enableTranspose=0;

/* create new panel */
void CreateModifierPanel(Widget parent)
{
    long w = 0;
    Widget panel, pf, al, cl, sl, ar, cr, sr, wk, tp, clr;

    if(parent == NULL)
        parent = toplevel;

    modPanel = XtVaCreatePopupShell("modPanel", transientShellWidgetClass, 
                    parent, NULL);

    pf = XtVaCreateManagedWidget("modPanelForm", formWidgetClass,
                   modPanel, NULL);
    al = XtCreateManagedWidget("alt-l", toggleWidgetClass,
                   pf, NULL, 0);

    cl = XtCreateManagedWidget("ctrl-l", toggleWidgetClass,
                   pf, NULL, 0);
    sl = XtCreateManagedWidget("shift-l", toggleWidgetClass,
                   pf, NULL, 0);
    ar = XtCreateManagedWidget("alt-r", toggleWidgetClass,
                   pf, NULL, 0);
    cr = XtCreateManagedWidget("ctrl-r", toggleWidgetClass,
                   pf, NULL, 0);
    sr = XtCreateManagedWidget("shift-r", toggleWidgetClass,
                   pf, NULL, 0);
    wk = XtCreateManagedWidget("win-l", toggleWidgetClass,
                   pf, NULL, 0);
    clr = XtCreateManagedWidget("reset", commandWidgetClass,
                   pf, NULL, 0);
    tp = XtCreateManagedWidget("transpose", toggleWidgetClass,
                   pf, NULL, 0);

    /* init buttons */
    modifierPanelButtons[MOD_ALT_L] = al;
    modifierPanelButtons[MOD_CTRL_L] = cl;
    modifierPanelButtons[MOD_SHIFT_L] = sl;
    modifierPanelButtons[MOD_ALT_R] = ar;
    modifierPanelButtons[MOD_CTRL_R] = cr;
    modifierPanelButtons[MOD_SHIFT_R] = sr;
    modifierPanelButtons[MOD_WIN_KEY] = wk;
    modifierPanelButtons[MOD_TRANSPOSE] = tp;

    /* set key codes */
    modifierButtonsKeycodes[MOD_ALT_L] = XStringToKeysym("Alt_L");
    modifierButtonsKeycodes[MOD_ALT_R] = XStringToKeysym("Alt_R");
    modifierButtonsKeycodes[MOD_CTRL_L] = XStringToKeysym("Control_L");
    modifierButtonsKeycodes[MOD_CTRL_R] = XStringToKeysym("Control_R");
    modifierButtonsKeycodes[MOD_SHIFT_L] = XStringToKeysym("Shift_L");
    modifierButtonsKeycodes[MOD_SHIFT_R] = XStringToKeysym("Shift_R");
    modifierButtonsKeycodes[MOD_WIN_KEY] = XStringToKeysym("Super_L");

    return;
}

/* set toggle buttons */
void ResetModifierPanelButtons()
{
    int i=0;

    for(i=0; i<MOD_MAX_IDX-1; i++)
    {
        if(modifierPanelButtons[i])
            XtVaSetValues(modifierPanelButtons[i], XtNstate, 0, NULL);
    }
}

/* show modifier panel */
void ShowModifierPanel(Widget wg, XEvent *ev, String *params,
        Cardinal *num_params)
{
    int w =0, h=0, x=0, y=0;    
    char *geometry;

    ResetModifierPanelButtons();
    XtVaGetValues(toplevel, XtNwidth, &w, XtNheight, &h, XtNx, &x, XtNy, &y, NULL);
    //position is appropriately
    geometry = XtMalloc(256);
    sprintf(geometry, "%dx%d+%d+%d", w-25, 30, x, y+h-80);
    XtVaSetValues(modPanel, XtNgeometry, geometry, NULL);

    XtVaSetValues(modifierPanelButtons[MOD_TRANSPOSE], XtNstate, 
                    enableTranspose == 1, NULL);

    XtPopup(modPanel, XtGrabNone);
    XSetWMProtocols(dpy, XtWindow(modPanel), &wmDeleteWindow, 1);
}

/* hide panel */
void HideModifierPanel(Widget w, XEvent *ev, String *params,
        Cardinal *num_params)
{
    XtPopdown(modPanel);
    ResetModifierPanelButtons();
}

/* set the toggle button using keybord */
void SetModifierPanelButton(Widget wg, XEvent *ev, String *params,
        Cardinal *num_params)
{
    int idx = 0, state = 0;

    idx = atoi(params[0]);

    XtVaGetValues(modifierPanelButtons[idx], XtNstate, &state, NULL);
    XtVaSetValues(modifierPanelButtons[idx], XtNstate, state == 0, NULL);
        
}

/* Get key code sym for given symcode, depending upon 
 * transpose flag is set or not
 */
CARD32 GetModifierEffectiveKeycode(CARD32 keycode)
{
    if(enableTranspose == 0)
        return keycode;
    if(keycode== modifierButtonsKeycodes[MOD_ALT_R])
            return modifierButtonsKeycodes[MOD_ALT_L];
    else if(keycode== modifierButtonsKeycodes[MOD_CTRL_R])
            return modifierButtonsKeycodes[MOD_CTRL_L];
    else if(keycode == modifierButtonsKeycodes[MOD_SHIFT_R])
            return modifierButtonsKeycodes[MOD_SHIFT_L];
/*
    switch(keycode)
    {
    case modifierButtonsKeycodes[MOD_ALT_R]:
            return modifierButtonsKeycodes[MOD_ALT_L];
    case modifierButtonsKeycodes[MOD_CTRL_R]:
            return modifierButtonsKeycodes[MOD_CTRL_L];
    case modifierButtonsKeycodes[MOD_SHIFT_R]:
            return modifierButtonsKeycodes[MOD_SHIFT_L];
    }
*/
    return keycode;
}

/* send key events for modifiers 
 * flag == 1 : send keypress event
 * flag == 0 : send keyrelease event
 */
void SendModifierEvents(int flag)
{
    int i=0, state = 0;


    XtVaGetValues(modifierPanelButtons[MOD_TRANSPOSE], XtNstate, &state, NULL);
    if(state == 1)
        enableTranspose = 1;

    if(flag)
    {
        //send keypress events
        for(i=0; i<MOD_MAX_IDX-1; i++)
        {
            if(modifierPanelButtons[i])
            {
                XtVaGetValues(modifierPanelButtons[i], XtNstate, &state, NULL);

                if(state)
                    SendKeyEvent(GetModifierEffectiveKeycode(modifierButtonsKeycodes[i]), flag);
            }
        }
    }
    else
        //send keyrelease in reverse order
        for(i=MOD_MAX_IDX-2; i>=0; i--)
        {
            if(modifierPanelButtons[i])
            {
                XtVaGetValues(modifierPanelButtons[i], XtNstate, &state, NULL);

                if(state)
                    SendKeyEvent(GetModifierEffectiveKeycode(modifierButtonsKeycodes[i]), flag);
            }
        } 
}


void SendKeyWithModifier(Widget w, XEvent *ev, String *params,
        Cardinal *num_params)
{
    int state = 0, keypress = 0;
    char keyname[256] = { 0 };
    KeySym ks = 0;
    
    XLookupString(&ev->xkey, keyname, sizeof(keyname), &ks, NULL);
    if(IsModifierKey(ks))
        return;

    //some valid key pressed, send the event
    if(ev->type == KeyPress)
    {
        SendModifierEvents(1);
        SendKeyEvent(ks, (ev->type == KeyPress));
    }
    else
    {
        SendKeyEvent(ks, (ev->type == KeyPress));
        SendModifierEvents(0);
        HideModifierPanel(w, ev, params, 0);
    }
}
    
/* send events to release all modifier keys */
void SendResetAllModifiers(Widget w, XEvent *ev, String *params,
        Cardinal *num_params)
{
    int i=0, state = 0;

    for(i=0; i<MOD_MAX_IDX-1; i++)
    {
        if(modifierPanelButtons[i])
        {   
            SendKeyEvent(modifierButtonsKeycodes[i], 1);
            SendKeyEvent(modifierButtonsKeycodes[i], 0);
        }
    }

}



