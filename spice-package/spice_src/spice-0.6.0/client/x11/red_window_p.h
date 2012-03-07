/*
   Copyright (C) 2009 Red Hat, Inc.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _H_RED_WINDOW_P
#define _H_RED_WINDOW_P

#ifdef USE_OGL
#include <GL/glx.h>
#endif // USE_OGL
#include <X11/Xdefs.h>
#include <X11/Xlib.h>

typedef Window Win;
#ifdef USE_OGL
typedef GLXContext RedGlContext;
typedef GLXPbuffer RedPbuffer;
#endif // USE_OGL

class RedWindow;
class Icon;
struct PixelsSource_p;

class RedWindow_p {
public:
    RedWindow_p();

    void migrate(RedWindow& red_window, PixelsSource_p& pix_source, int dest_screen);
    void create(RedWindow& red_window, PixelsSource_p& pix_source, int x, int y,
                unsigned int width, unsigned int height, int in_screen);
    void destroy(RedWindow& red_window, PixelsSource_p& pix_source);
    void set_minmax(PixelsSource_p& pix_source, int width, int height);
    void wait_for_reparent();
    void wait_for_map();
    void wait_for_unmap();
    void sync(bool shadowed = false);
    void set_visibale(bool vis) { _visibale = vis;}
    void move_to_current_desktop();
    Window get_window() {return _win;}

    static void win_proc(XEvent& event);
    static Cursor create_invisible_cursor(Window window);

#ifdef USE_OGL
    void set_glx(int width, int height);
#endif // USE_OGL
    static void handle_key_press_event(RedWindow& red_window, XKeyEvent* event);

protected:
    int _screen;
    Window _win;
    Cursor _invisible_cursor;
    bool _visibale;
    bool _expect_parent;
    SpicePoint _show_pos;
#ifdef USE_OGL
    GLXContext _glcont_copy;
#endif // USE_OGL
    Icon* _icon;
    bool _focused;
    bool _ignore_foucs;
    bool _shadow_foucs_state;
    XEvent _shadow_focus_event;
    bool _pointer_in_window;
    bool _ignore_pointer;
    bool _shadow_pointer_state;
    XEvent _shadow_pointer_event;
    Colormap _colormap;
};

#endif

