import java.awt.BorderLayout;
import java.awt.Font;
import java.awt.GridLayout;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;

import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JCheckBox;
import javax.swing.JLabel;

public class CmdPopMenu {
    /**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	
	int menuModifier;
	JCheckBox altL, ctrlL, shiftL, windows, altR, ctrlR, shiftR;
	GridLayout leftKeysGrid;
	JPanel modifierKeyPanel;
	JPanel mainModifierPanel;
	JLabel versionLable;
	JButton close;
	JCheckBox transposeRightMods;
	JPanel cPanel;
	VncViewer vncViewer;
	
	
	public CmdPopMenu(VncViewer viewer){
		vncViewer = viewer;
		
		modifierKeyPanel = new JPanel(new GridLayout(1,8));
		mainModifierPanel = new JPanel(new BorderLayout());
		cPanel=new JPanel();
		cPanel.setLayout(new BorderLayout(3,3));
		
	    altL = new JCheckBox ("Alt(Left)");
	    ctrlL = new JCheckBox ("Ctrl(Left)");
	    shiftL = new JCheckBox ("Shift(Left)");
	    windows = new JCheckBox ("Windows");
	    altR = new JCheckBox ("Alt(Right)");
	    ctrlR = new JCheckBox ("Ctrl(Right)");
	    shiftR = new JCheckBox ("Shift(Right)");

	    transposeRightMods = new JCheckBox("Transpose R->L");
	    
	    close= new JButton("X");
	    close.setFont(new Font("Arial", Font.BOLD, 12));
	    close.setBorderPainted(true);
	    
	    if (viewer.jarRevisionStr == null || viewer.jarRevisionStr.trim().length() == 0)
	    	versionLable = new JLabel("Modifier key panel (Version info not available.)");
	    else
	    	versionLable = new JLabel("Modifier key panel (revision:"
	    			+ viewer.jarRevisionStr + ")");
	    modifierKeyPanel.add (altL);
	    modifierKeyPanel.add (ctrlL);
	    modifierKeyPanel.add (shiftL);
	    modifierKeyPanel.add (altR);
	    modifierKeyPanel.add (ctrlR);
	    modifierKeyPanel.add (shiftR);
	    modifierKeyPanel.add (windows);
	    modifierKeyPanel.add (transposeRightMods);
	    mainModifierPanel.add (versionLable, BorderLayout.NORTH);
	    mainModifierPanel.add (modifierKeyPanel, BorderLayout.CENTER);
	   
	    cPanel.add(close,BorderLayout.EAST);	  
	    cPanel.add(new JLabel(),BorderLayout.WEST); 
	    cPanel.add(versionLable,BorderLayout.CENTER);

	    mainModifierPanel.add(cPanel,BorderLayout.NORTH);
	   
	    close.addActionListener(new ActionListener() {
	    	public void actionPerformed(ActionEvent e) {
	    		if(e.getSource()==close) {
	    			vncViewer.vc.hideSplKeysMenu();
	    			vncViewer.rfb.resetModifierMenu();
	    			vncViewer.vc.requestFocusInWindow();
				}
				
	    	}
	    });
	    
	    transposeRightMods.addActionListener( new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				if (evt.getSource() == transposeRightMods) {
					vncViewer.transposeRightMods = !vncViewer.transposeRightMods;
				}
				vncViewer.vc.requestFocusInWindow();
			}
	    });
	    
	    altL.addItemListener(new ItemListener () {

			public void itemStateChanged(ItemEvent arg0) {
				vncViewer.vc.requestFocusInWindow();
			}
	    });
	    
	    ctrlL.addItemListener(new ItemListener () {

			public void itemStateChanged(ItemEvent arg0) {
				vncViewer.vc.requestFocusInWindow();
			}
	    });
	    
	    shiftL.addItemListener(new ItemListener () {

			public void itemStateChanged(ItemEvent arg0) {
				vncViewer.vc.requestFocusInWindow();
			}
	    });
	    
	    windows.addItemListener(new ItemListener () {

			public void itemStateChanged(ItemEvent arg0) {
				vncViewer.vc.requestFocusInWindow();
			}
	    });
	    
	    shiftR.addItemListener(new ItemListener () {

			public void itemStateChanged(ItemEvent arg0) {
				vncViewer.vc.requestFocusInWindow();
			}
	    });
	    
	    altR.addItemListener(new ItemListener () {

			public void itemStateChanged(ItemEvent arg0) {
				vncViewer.vc.requestFocusInWindow();
			}
	    });
	    
	    ctrlR.addItemListener(new ItemListener () {

			public void itemStateChanged(ItemEvent arg0) {
				vncViewer.vc.requestFocusInWindow();
			}
	    });
    }
	
	void resetAll () {
		altL.setSelected(false);
	    ctrlL.setSelected(false);
	    shiftL.setSelected(false);
	    windows.setSelected(false);
	    altR.setSelected(false);
	    ctrlR.setSelected(false);
	    shiftR.setSelected(false);
	    menuModifier = 0;
	}
	
	int getModifier() {
		menuModifier = 0;
		if (altR.isSelected() == true) {
			menuModifier = menuModifier | RfbProto.R_ALT_MASK;
		}
		if (altL.isSelected() == true) {
			menuModifier = menuModifier | RfbProto.ALT_MASK;
		}
		if (ctrlR.isSelected() == true) {
			menuModifier = menuModifier | RfbProto.R_CTRL_MASK;
		}
		if (ctrlL.isSelected() == true) {
			menuModifier = menuModifier | RfbProto.CTRL_MASK;
		}
		if (shiftR.isSelected() == true) {
			menuModifier = menuModifier | RfbProto.R_SHIFT_MASK;
		}
		if (shiftL.isSelected() == true) {
			menuModifier = menuModifier | RfbProto.SHIFT_MASK;
		}
		if (windows.isSelected() == true) {
			menuModifier = menuModifier | RfbProto.WINDOWS_MASK;
		}
		
		return menuModifier;
	}
}
