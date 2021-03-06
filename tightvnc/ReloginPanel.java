//
//  Copyright (C) 2002 Cendio Systems.  All Rights Reserved.
//  Copyright (C) 2002 Constantin Kaplinsky.  All Rights Reserved.
//
//  This is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this software; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
//  USA.
//

//
// ReloginPanel class implements panel with a button for logging in again,
// after fatal errors or disconnect
//



import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JPanel;

//
// The panel which implements the Relogin button
//

class ReloginPanel extends JPanel implements ActionListener {
  /**
	 * 
	 */
	private static final long serialVersionUID = -6703738686086740899L;
JButton reloginButton;
  JButton closeButton;
  VncViewer viewer;

  //
  // Constructor.
  //
  public ReloginPanel(VncViewer v) {
    viewer = v;
    setLayout(new FlowLayout(FlowLayout.CENTER));
    reloginButton = new JButton("Login again");
    add(reloginButton);
    reloginButton.addActionListener(this);
    if (viewer.inSeparateFrame) {
      closeButton = new JButton("Close window");
      add(closeButton);
      closeButton.addActionListener(this);
    }
  }

  //
  // This method is called when a button is pressed.
  //
  public synchronized void actionPerformed(ActionEvent evt) {
    if (viewer.inSeparateFrame)
      viewer.vncFrame.dispose();
    if (evt.getSource() == reloginButton)
      viewer.getAppletContext().showDocument(viewer.getDocumentBase());
  }
}
