/* 
 * Header file for modifier Panel functions 
 *
 */

/* create new panel */
void CreateModifierPanel(Widget parent);

/* show the paenl */
void ShowModifierPanel(Widget w, XEvent *ev, String *params,
        Cardinal *num_params);
/* hide the panel */
void HideModifierPanel(Widget w, XEvent *ev, String *params,
        Cardinal *num_params);
/* set using keybord */
void SetModifierPanelButton(Widget w, XEvent *ev, String *params,
        Cardinal *num_params);
/* send key events */
void SendKeyWithModifier(Widget w, XEvent *ev, String *params,
        Cardinal *num_params);
/* reset all modifier keys */
void SendResetAllModifiers(Widget w, XEvent *ev, String *params,
        Cardinal *num_params);

