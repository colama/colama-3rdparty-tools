import java.awt.Cursor;
import java.awt.Image;
import java.awt.Toolkit;

import javax.swing.ImageIcon;
import javax.swing.JPanel;


public class CustomPointer {

	public void setMyPointer(JPanel c)
	{
		 ImageIcon icon = new ImageIcon("");        //If it dont find the image it'll set it to transparent
		 Image a = icon.getImage();
		 Cursor myCursor = Toolkit.getDefaultToolkit().createCustomCursor(a,new java.awt.Point(10,10),"test");
			
		try{
			c.setCursor(myCursor);
			System.out.println("Canvas cursor re-setted.");
		}catch(Exception e)
		{
		    e.printStackTrace();
		}
	}
	
}
