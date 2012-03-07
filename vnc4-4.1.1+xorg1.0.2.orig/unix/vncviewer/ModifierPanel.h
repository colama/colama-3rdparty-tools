/* Copyright (C) 2002-2005 RealVNC Ltd.  All Rights Reserved.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 */
//
// ModifierPanel.h
//

#ifndef __MODIFIERPANEL_H__
#define __MODIFIERPANEL_H__

#include "TXDialog.h"
#include "TXButton.h"
#include "TXCheckbox.h"
#include "parameters.h"

class ModifierPanelCallback {
public:
  virtual void resetAllModifiers() = 0;
  virtual void toggleFullScreen() = 0;
};

class ModifierPanel : public TXDialog, public TXButtonCallback,
                      public TXCheckboxCallback
{
public:
  ModifierPanel(Display* dpy, ModifierPanelCallback* cb_)
    : TXDialog(dpy, 400, 400, "VNC Viewer: Modifier Panel"), cb(cb_),
      al(dpy, "Alt-L", this, false, this),
      ar(dpy, "Alt-R", this, false, this),
      cl(dpy, "Ctrl-l", this, false, this),
      cr(dpy, "Ctrl-R", this, false, this),
      sl(dpy, "Shift-l", this, false, this),
      sr(dpy, "Shift-R", this, false, this),
      wk(dpy, "Win", this, false, this),
      trp(dpy, "Transpose R->L", this, false, this),
      fscr(dpy, "FullScreen On", this, this, 60),
      rst(dpy, "Reset All", this, this, 60)
  {
    int x = xPad;
    al.move(x, yPad);
    x += al.width();
    cl.move(x, yPad);
    x += cl.width();
    sl.move(x, yPad);
    x += sl.width();
    wk.move(x, yPad);
    x += wk.width();
    ar.move(x, yPad);
    x += ar.width();
    cr.move(x, yPad);
    x += cr.width();
    sr.move(x, yPad);
    x += sr.width();
    trp.move(x, yPad);
    x += trp.width()+5;
    fscr.move(x, yPad);
    x += fscr.width();
    rst.move(x, yPad);
    x += rst.width();

    resize(x, ar.height() + 20);
    mwidth = x;
    mheight = ar.height() + 20;
    setBorderWidth(2);
  }
    void layout1(int startx, int starty)
    {
        int x = startx+xPad+10, y = starty+5, pad=5;
        //first column items
        al.move(x, y);
        y += al.height()+ pad;
        cl.move(x, y);
        y += cl.height()+ pad;
        sl.move(x, y);
        y += sl.height()+ pad;
        wk.move(x, y);
        y += wk.height()+ pad+pad;
        rst.move(x, y);

        //2nd column items
        y=starty+5;
        x += rst.width() + 40;
        ar.move(x, y);
        y += ar.height()+ pad;
        cr.move(x, y);
        y += cr.height()+ pad;
        sr.move(x, y);
        y += sr.height()+ pad;
        trp.move(x, y);
        y += trp.height()+pad+pad;
        fscr.move(x, y);
        y += fscr.height()+ pad+pad;
        resize(x+trp.width() + 20, y);


    }
    void showPanel(int x, int y)
    {
        layout1(xPad, yPad);
        move(x, y);
        show();
    }

    void resetAll()
    {
        al.checked(0);
        ar.checked(0);
        cl.checked(0);
        cr.checked(0);
        sl.checked(0);
        sr.checked(0);
        wk.checked(0);
        //modPanel.trp.checked(0);
    }


  virtual void initDialog() {
  }

  virtual void takeFocus(Time time) {
  }
   virtual void checkboxSelect(TXCheckbox* checkbox) {
    }

  virtual void buttonActivate(TXButton* b) {
    if (b == &fscr) {
      if (cb) cb->toggleFullScreen();
      unmap();
    }
    else if (b == &rst) {
      if (cb) cb->resetAllModifiers();
      unmap();
    }
  }

  ModifierPanelCallback* cb;
  TXCheckbox al, ar, cl, cr, sl, sr, wk, trp;
  TXButton fscr, rst;
  int mwidth, mheight;
};

#endif
