
//Copyright (C) 2001-2004 HorizonLive.com, Inc.  All Rights Reserved.
//Copyright (C) 2001-2006 Constantin Kaplinsky.  All Rights Reserved.
//Copyright (C) 2000 Tridia Corporation.  All Rights Reserved.
//Copyright (C) 1999 AT&T Laboratories Cambridge.  All Rights Reserved.

//This is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.

//This software is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

//You should have received a copy of the GNU General Public License
//along with this software; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
//USA.



//RfbProto.java


import java.io.*;
import java.awt.event.*;
import java.net.Socket;
import java.util.zip.*;

import javax.swing.JCheckBox;

class RfbProto {

	int winkey = 0;
	long sysTime;
	long lastAltPressed;

	final static String
	versionMsg_3_3 = "RFB 003.003\n",
	versionMsg_3_7 = "RFB 003.007\n",
	versionMsg_3_8 = "RFB 003.008\n";

	// Vendor signatures: standard VNC/RealVNC, TridiaVNC, and TightVNC
	final static String
	StandardVendor  = "STDV",
	TridiaVncVendor = "TRDV",
	TightVncVendor  = "TGHT";

	// Security types
	final static int
	SecTypeInvalid = 0,
	SecTypeNone    = 1,
	SecTypeVncAuth = 2,
	SecTypeTight   = 16;

	// Supported tunneling types
	final static int
	NoTunneling = 0;
	final static String
	SigNoTunneling = "NOTUNNEL";

	// Supported authentication types
	final static int
	AuthNone      = 1,
	AuthVNC       = 2,
	AuthUnixLogin = 129;
	final static String
	SigAuthNone      = "NOAUTH__",
	SigAuthVNC       = "VNCAUTH_",
	SigAuthUnixLogin = "ULGNAUTH";

	// VNC authentication results
	final static int
	VncAuthOK      = 0,
	VncAuthFailed  = 1,
	VncAuthTooMany = 2;

	// Server-to-client messages
	final static int
	FramebufferUpdate   = 0,
	SetColourMapEntries = 1,
	Bell                = 2,
	ServerCutText       = 3;

	// Client-to-server messages
	final static int
	SetPixelFormat           = 0,
	FixColourMapEntries      = 1,
	SetEncodings             = 2,
	FramebufferUpdateRequest = 3,
	KeyboardEvent            = 4,
	PointerEvent             = 5,
	ClientCutText            = 6;

	// Supported encodings and pseudo-encodings
	final static int
	EncodingRaw            = 0,
	EncodingCopyRect       = 1,
	EncodingRRE            = 2,
	EncodingCoRRE          = 4,
	EncodingHextile        = 5,
	EncodingZlib           = 6,
	EncodingTight          = 7,
	EncodingZRLE           = 16,
	EncodingCompressLevel0 = 0xFFFFFF00,
	EncodingQualityLevel0  = 0xFFFFFFE0,
	EncodingXCursor        = 0xFFFFFF10,
	EncodingRichCursor     = 0xFFFFFF11,
	EncodingPointerPos     = 0xFFFFFF18,
	EncodingLastRect       = 0xFFFFFF20,
	EncodingNewFBSize      = 0xFFFFFF21;
	final static String
	SigEncodingRaw            = "RAW_____",
	SigEncodingCopyRect       = "COPYRECT",
	SigEncodingRRE            = "RRE_____",
	SigEncodingCoRRE          = "CORRE___",
	SigEncodingHextile        = "HEXTILE_",
	SigEncodingZlib           = "ZLIB____",
	SigEncodingTight          = "TIGHT___",
	SigEncodingZRLE           = "ZRLE____",
	SigEncodingCompressLevel0 = "COMPRLVL",
	SigEncodingQualityLevel0  = "JPEGQLVL",
	SigEncodingXCursor        = "X11CURSR",
	SigEncodingRichCursor     = "RCHCURSR",
	SigEncodingPointerPos     = "POINTPOS",
	SigEncodingLastRect       = "LASTRECT",
	SigEncodingNewFBSize      = "NEWFBSIZ";

	final static int MaxNormalEncoding = 255;

	// Contstants used in the Hextile decoder
	final static int
	HextileRaw                 = 1,
	HextileBackgroundSpecified = 2,
	HextileForegroundSpecified = 4,
	HextileAnySubrects         = 8,
	HextileSubrectsColoured    = 16;

	// Contstants used in the Tight decoder
	final static int TightMinToCompress = 12;
	final static int
	TightExplicitFilter = 0x04,
	TightFill           = 0x08,
	TightJpeg           = 0x09,
	TightMaxSubencoding = 0x09,
	TightFilterCopy     = 0x00,
	TightFilterPalette  = 0x01,
	TightFilterGradient = 0x02;


	String host;
	int port;
	Socket sock;
	DataInputStream is;
	OutputStream os;
	SessionRecorder rec;
	boolean inNormalProtocol = false;
	VncViewer viewer;

	// Java on UNIX does not call keyPressed() on some keys, for example
	// swedish keys To prevent our workaround to produce duplicate
	// keypresses on JVMs that actually works, keep track of if
	// keyPressed() for a "broken" key was called or not. 
	boolean brokenKeyPressed = false;

	// This will be set to true on the first framebuffer update
	// containing Zlib-, ZRLE- or Tight-encoded data.
	boolean wereZlibUpdates = false;

	// This will be set to false if the startSession() was called after
	// we have received at least one Zlib-, ZRLE- or Tight-encoded
	// framebuffer update.
	boolean recordFromBeginning = true;

	// This fields are needed to show warnings about inefficiently saved
	// sessions only once per each saved session file.
	boolean zlibWarningShown;
	boolean tightWarningShown;

	// Before starting to record each saved session, we set this field
	// to 0, and increment on each framebuffer update. We don't flush
	// the SessionRecorder data into the file before the second update. 
	// This allows us to write initial framebuffer update with zero
	// timestamp, to let the player show initial desktop before
	// playback.
	int numUpdatesInSession;

	// Measuring network throughput.
	boolean timing;
	long timeWaitedIn100us;
	long timedKbits;

	// Protocol version and TightVNC-specific protocol options.
	int serverMajor, serverMinor;
	int clientMajor, clientMinor;
	boolean protocolTightVNC;
	CapsContainer tunnelCaps, authCaps;
	CapsContainer serverMsgCaps, clientMsgCaps;
	CapsContainer encodingCaps;

	// If true, informs that the RFB socket was closed.
	private boolean closed;

	boolean useMenuModifiers;
	boolean debug_rfbProto = false;
	boolean debug_keyhandler = (debug_rfbProto | false);
//	boolean debug_keyhandler = true;
	boolean debug_mousehandler = (debug_rfbProto | false);
//	boolean debug_mousehandler = true;

	//
	// Constructor. Make TCP connection to RFB server.
	//

	RfbProto(String h, int p, VncViewer v) throws IOException {
		viewer = v;
		host = h;
		port = p;

		if (viewer.socketFactory == null) {
			sock = new Socket(host, port);
		} else {
			try {
				Class factoryClass = Class.forName(viewer.socketFactory);
				SocketFactory factory = (SocketFactory)factoryClass.newInstance();
				if (viewer.inAnApplet)
					sock = factory.createSocket(host, port, viewer);
				else
					sock = factory.createSocket(host, port, viewer.mainArgs);
			} catch(Exception e) {
				e.printStackTrace();
				throw new IOException(e.getMessage());
			}
		}
		is = new DataInputStream(new BufferedInputStream(sock.getInputStream(),
				16384));
		os = sock.getOutputStream();

		timing = false;
		timeWaitedIn100us = 5;
		timedKbits = 0;
		useMenuModifiers = false;
	}


	synchronized void close() {
		try {
			sock.close();
			closed = true;
			if (debug_rfbProto) System.out.println("RFB socket closed");
			if (rec != null) {
				rec.close();
				rec = null;
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	synchronized boolean closed() {
		return closed;
	}

	//
	// Read server's protocol version message
	//

	void readVersionMsg() throws Exception {

		byte[] b = new byte[12];

		readFully(b);

		if ((b[0] != 'R') || (b[1] != 'F') || (b[2] != 'B') || (b[3] != ' ')
				|| (b[4] < '0') || (b[4] > '9') || (b[5] < '0') || (b[5] > '9')
				|| (b[6] < '0') || (b[6] > '9') || (b[7] != '.')
				|| (b[8] < '0') || (b[8] > '9') || (b[9] < '0') || (b[9] > '9')
				|| (b[10] < '0') || (b[10] > '9') || (b[11] != '\n'))
		{
			throw new Exception("Host " + host + " port " + port +
			" is not an RFB server");
		}

		serverMajor = (b[4] - '0') * 100 + (b[5] - '0') * 10 + (b[6] - '0');
		serverMinor = (b[8] - '0') * 100 + (b[9] - '0') * 10 + (b[10] - '0');

		if (serverMajor < 3) {
			throw new Exception("RFB server does not support protocol version 3");
		}
	}


	//
	// Write our protocol version message
	//

	void writeVersionMsg() throws IOException {
		clientMajor = 3;
		if (serverMajor > 3 || serverMinor >= 8) {
			clientMinor = 8;
			os.write(versionMsg_3_8.getBytes());
		} else if (serverMinor >= 7) {
			clientMinor = 7;
			os.write(versionMsg_3_7.getBytes());
		} else {
			clientMinor = 3;
			os.write(versionMsg_3_3.getBytes());
		}
		protocolTightVNC = false;
	}


	//
	// Negotiate the authentication scheme.
	//

	int negotiateSecurity() throws Exception {
		return (clientMinor >= 7) ?
				selectSecurityType() : readSecurityType();
	}

	//
	// Read security type from the server (protocol version 3.3).
	//

	int readSecurityType() throws Exception {
		int secType = is.readInt();

		switch (secType) {
		case SecTypeInvalid:
			readConnFailedReason();
			return SecTypeInvalid;    // should never be executed
		case SecTypeNone:
		case SecTypeVncAuth:
			return secType;
		default:
			throw new Exception("Unknown security type from RFB server: " + secType);
		}
	}

	//
	// Select security type from the server's list (protocol versions 3.7/3.8).
	//

	int selectSecurityType() throws Exception {
		int secType = SecTypeInvalid;

		// Read the list of secutiry types.
		int nSecTypes = is.readUnsignedByte();
		if (nSecTypes == 0) {
			readConnFailedReason();
			return SecTypeInvalid;    // should never be executed
		}
		byte[] secTypes = new byte[nSecTypes];
		readFully(secTypes);

		// Find out if the server supports TightVNC protocol extensions
		for (int i = 0; i < nSecTypes; i++) {
			if (secTypes[i] == SecTypeTight) {
				protocolTightVNC = true;
				os.write(SecTypeTight);
				return SecTypeTight;
			}
		}

		// Find first supported security type.
		for (int i = 0; i < nSecTypes; i++) {
			if (secTypes[i] == SecTypeNone || secTypes[i] == SecTypeVncAuth) {
				secType = secTypes[i];
				break;
			}
		}

		if (secType == SecTypeInvalid) {
			throw new Exception("Server did not offer supported security type");
		} else {
			os.write(secType);
		}

		return secType;
	}

	//
	// Perform "no authentication".
	//

	void authenticateNone() throws Exception {
		if (clientMinor >= 8)
			readSecurityResult("No authentication");
	}

	//
	// Perform standard VNC Authentication.
	//

	void authenticateVNC(String pw) throws Exception {
		byte[] challenge = new byte[16];
		readFully(challenge);

		if (pw.length() > 8)
			pw = pw.substring(0, 8);  // Truncate to 8 chars

		// Truncate password on the first zero byte.
		int firstZero = pw.indexOf(0);
		if (firstZero != -1)
			pw = pw.substring(0, firstZero);

		byte[] key = {0, 0, 0, 0, 0, 0, 0, 0};
		System.arraycopy(pw.getBytes(), 0, key, 0, pw.length());

		DesCipher des = new DesCipher(key);

		des.encrypt(challenge, 0, challenge, 0);
		des.encrypt(challenge, 8, challenge, 8);

		os.write(challenge);

		readSecurityResult("VNC authentication");
	}

	//
	// Read security result.
	// Throws an exception on authentication failure.
	//

	void readSecurityResult(String authType) throws Exception {
		int securityResult = is.readInt();

		switch (securityResult) {
		case VncAuthOK:
			if (debug_rfbProto) System.out.println(authType + ": success");
			break;
		case VncAuthFailed:
			if (clientMinor >= 8)
				readConnFailedReason();
			throw new Exception(authType + ": failed");
		case VncAuthTooMany:
			throw new Exception(authType + ": failed, too many tries");
		default:
			throw new Exception(authType + ": unknown result " + securityResult);
		}
	}

	//
	// Read the string describing the reason for a connection failure,
	// and throw an exception.
	//

	void readConnFailedReason() throws Exception {
		int reasonLen = is.readInt();
		byte[] reason = new byte[reasonLen];
		readFully(reason);
		throw new Exception(new String(reason));
	}

	//
	// Initialize capability lists (TightVNC protocol extensions).
	//

	void initCapabilities() {
		tunnelCaps    = new CapsContainer();
		authCaps      = new CapsContainer();
		serverMsgCaps = new CapsContainer();
		clientMsgCaps = new CapsContainer();
		encodingCaps  = new CapsContainer();

		// Supported authentication methods
		authCaps.add(AuthNone, StandardVendor, SigAuthNone,
		"No authentication");
		authCaps.add(AuthVNC, StandardVendor, SigAuthVNC,
		"Standard VNC password authentication");

		// Supported encoding types
		encodingCaps.add(EncodingCopyRect, StandardVendor,
				SigEncodingCopyRect, "Standard CopyRect encoding");
		// encodingCaps.add(EncodingRRE, StandardVendor,
		//         SigEncodingRRE, "Standard RRE encoding");
		//encodingCaps.add(EncodingCoRRE, StandardVendor,
		//         SigEncodingCoRRE, "Standard CoRRE encoding");
		encodingCaps.add(EncodingHextile, StandardVendor,
				SigEncodingHextile, "Standard Hextile encoding");
		//encodingCaps.add(EncodingZRLE, StandardVendor,
		//         SigEncodingZRLE, "Standard ZRLE encoding");
		//encodingCaps.add(EncodingZlib, TridiaVncVendor,
		//         SigEncodingZlib, "Zlib encoding");
		//encodingCaps.add(EncodingTight, TightVncVendor,
		//         SigEncodingTight, "Tight encoding");

		// Supported pseudo-encoding types
		encodingCaps.add(EncodingCompressLevel0, TightVncVendor,
				SigEncodingCompressLevel0, "Compression level");
		encodingCaps.add(EncodingQualityLevel0, TightVncVendor,
				SigEncodingQualityLevel0, "JPEG quality level");
		encodingCaps.add(EncodingXCursor, TightVncVendor,
				SigEncodingXCursor, "X-style cursor shape update");
		encodingCaps.add(EncodingRichCursor, TightVncVendor,
				SigEncodingRichCursor, "Rich-color cursor shape update");
		encodingCaps.add(EncodingPointerPos, TightVncVendor,
				SigEncodingPointerPos, "Pointer position update");
		encodingCaps.add(EncodingLastRect, TightVncVendor,
				SigEncodingLastRect, "LastRect protocol extension");
		encodingCaps.add(EncodingNewFBSize, TightVncVendor,
				SigEncodingNewFBSize, "Framebuffer size change");
	}

	//
	// Setup tunneling (TightVNC protocol extensions)
	//

	void setupTunneling() throws IOException {
		int nTunnelTypes = is.readInt();
		if (nTunnelTypes != 0) {
			readCapabilityList(tunnelCaps, nTunnelTypes);

			// We don't support tunneling yet.
			writeInt(NoTunneling);
		}
	}

	//
	// Negotiate authentication scheme (TightVNC protocol extensions)
	//

	int negotiateAuthenticationTight() throws Exception {
		int nAuthTypes = is.readInt();
		if (nAuthTypes == 0)
			return AuthNone;

		readCapabilityList(authCaps, nAuthTypes);
		for (int i = 0; i < authCaps.numEnabled(); i++) {
			int authType = authCaps.getByOrder(i);
			if (authType == AuthNone || authType == AuthVNC) {
				writeInt(authType);
				return authType;
			}
		}
		throw new Exception("No suitable authentication scheme found");
	}

	//
	// Read a capability list (TightVNC protocol extensions)
	//

	void readCapabilityList(CapsContainer caps, int count) throws IOException {
		int code;
		byte[] vendor = new byte[4];
		byte[] name = new byte[8];
		for (int i = 0; i < count; i++) {
			code = is.readInt();
			readFully(vendor);
			readFully(name);
			caps.enable(new CapabilityInfo(code, vendor, name));
		}
	}

	//
	// Write a 32-bit integer into the output stream.
	//

	void writeInt(int value) throws IOException {
		byte[] b = new byte[4];
		b[0] = (byte) ((value >> 24) & 0xff);
		b[1] = (byte) ((value >> 16) & 0xff);
		b[2] = (byte) ((value >> 8) & 0xff);
		b[3] = (byte) (value & 0xff);
		os.write(b);
	}

	//
	// Write the client initialisation message
	//

	void writeClientInit() throws IOException {
		byte tmp;

		if (debug_rfbProto) System.out.println("processing writeClientInit");
		if (viewer.options.shareDesktop) {
			if (debug_rfbProto) System.out.println("writeClientInit : sending 1 (share)");
			tmp = (byte) 0x1;
		} else {
			if (debug_rfbProto) System.out.println("writeClientInit : sending 0 (don't share)");
			tmp = (byte) 0x0;
		}
		os.write(tmp);

		viewer.options.disableShareDesktop();
	}


	//
	// Read the server initialisation message
	//

	String desktopName;
	int framebufferWidth, framebufferHeight;
	int bitsPerPixel, depth;
	boolean bigEndian, trueColour;
	int redMax, greenMax, blueMax, redShift, greenShift, blueShift;

	void readServerInit() throws IOException {
		if (debug_rfbProto) System.out.println("processing readServerInit");
		framebufferWidth = is.readUnsignedShort();
		framebufferHeight = is.readUnsignedShort();
		bitsPerPixel = is.readUnsignedByte();
		depth = is.readUnsignedByte();
		bigEndian = (is.readUnsignedByte() != 0);
		trueColour = (is.readUnsignedByte() != 0);
		redMax = is.readUnsignedShort();
		greenMax = is.readUnsignedShort();
		blueMax = is.readUnsignedShort();
		redShift = is.readUnsignedByte();
		greenShift = is.readUnsignedByte();
		blueShift = is.readUnsignedByte();
		byte[] pad = new byte[3];
		readFully(pad);
		int nameLength = is.readInt();
		byte[] name = new byte[nameLength];
		readFully(name);
		desktopName = new String(name);

		if (debug_rfbProto) System.out.println("PixelFormat offerred by server");
		if (debug_rfbProto) System.out.println("PixelFormat bitPerPixel = " + bitsPerPixel + ", colour depth = " + depth);
		if (debug_rfbProto) System.out.println("PixelFormat bigEndian = " + bigEndian + ", trueColour = " + trueColour);
		if (debug_rfbProto) System.out.println("PixelFormat Max R = " + redMax + ", G = " + greenMax + ", B = " + blueMax);
		if (debug_rfbProto) System.out.println("PixelFormat Shift R = " + redShift + ", G = " + greenShift + ", B = " + blueShift);


		// Read interaction capabilities (TightVNC protocol extensions)
		if (protocolTightVNC) {
			if (debug_rfbProto) System.out.println("Exploring TightVNC options in server init message");
			int nServerMessageTypes = is.readUnsignedShort();
			int nClientMessageTypes = is.readUnsignedShort();
			int nEncodingTypes = is.readUnsignedShort();
			is.readUnsignedShort();
			readCapabilityList(serverMsgCaps, nServerMessageTypes);
			readCapabilityList(clientMsgCaps, nClientMessageTypes);
			readCapabilityList(encodingCaps, nEncodingTypes);
		}

		inNormalProtocol = true;
	}


	//
	// Create session file and write initial protocol messages into it.
	//

	void startSession(String fname) throws IOException {
		rec = new SessionRecorder(fname);
		rec.writeHeader();
		rec.write(versionMsg_3_3.getBytes());
		rec.writeIntBE(SecTypeNone);
		rec.writeShortBE(framebufferWidth);
		rec.writeShortBE(framebufferHeight);
		byte[] fbsServerInitMsg =   {
				32, 24, 0, 1, 0,
				(byte)0xFF, 0, (byte)0xFF, 0, (byte)0xFF,
				16, 8, 0, 0, 0, 0
		};
		rec.write(fbsServerInitMsg);
		rec.writeIntBE(desktopName.length());
		rec.write(desktopName.getBytes());
		numUpdatesInSession = 0;

		// FIXME: If there were e.g. ZRLE updates only, that should not
		//        affect recording of Zlib and Tight updates. So, actually
		//        we should maintain separate flags for Zlib, ZRLE and
		//        Tight, instead of one ``wereZlibUpdates'' variable.
		//
		if (wereZlibUpdates)
			recordFromBeginning = false;

		zlibWarningShown = false;
		tightWarningShown = false;
	}

	//
	// Close session file.
	//

	void closeSession() throws IOException {
		if (rec != null) {
			rec.close();
			rec = null;
		}
	}


	//
	// Set new framebuffer size
	//

	void setFramebufferSize(int width, int height) {
		if (debug_rfbProto) System.out.println("processing setFramebufferSize");
		framebufferWidth = width;
		framebufferHeight = height;
	}


	//
	// Read the server message type
	//

	int readServerMessageType() throws IOException {
		int msgType = is.readUnsignedByte();
		if (debug_rfbProto) System.out.println("processing readServerMessageType");
		// If the session is being recorded:
		if (rec != null) {
			if (msgType == Bell) {    // Save Bell messages in session files.
				rec.writeByte(msgType);
				if (numUpdatesInSession > 0)
					rec.flush();
			}
		}

		return msgType;
	}


	//
	// Read a FramebufferUpdate message
	//

	int updateNRects;

	void readFramebufferUpdate() throws IOException {
		if (debug_rfbProto) System.out.println("processing readFramebufferUpdate");
		is.readByte();		// discard padding after mesg type
		updateNRects = is.readUnsignedShort();
		if (debug_rfbProto) System.out.println("	updateNRects = " + updateNRects);
		// If the session is being recorded:
		if (rec != null) {
			rec.writeByte(FramebufferUpdate);
			rec.writeByte(0);
			rec.writeShortBE(updateNRects);
		}

		numUpdatesInSession++;
	}

	// Read a FramebufferUpdate rectangle header

	int updateRectX, updateRectY, updateRectW, updateRectH, updateRectEncoding;

	void readFramebufferUpdateRectHdr() throws Exception {
		if (debug_rfbProto) System.out.println("processing readFramebufferUpdateRectHdr");
		updateRectX = is.readUnsignedShort();
		updateRectY = is.readUnsignedShort();
		updateRectW = is.readUnsignedShort();
		updateRectH = is.readUnsignedShort();
		updateRectEncoding = is.readInt();
		if (debug_rfbProto) System.out.println("\tenc = " + updateRectEncoding + ", x = " + updateRectX + ",y = " + updateRectY + ", w = " + updateRectW + ", h = " + updateRectH);

		if (updateRectEncoding == EncodingZlib ||
				updateRectEncoding == EncodingZRLE ||
				updateRectEncoding == EncodingTight)
			wereZlibUpdates = true;

		// If the session is being recorded:
		if (rec != null) {
			if (numUpdatesInSession > 1)
				rec.flush();        // Flush the output on each rectangle.
			rec.writeShortBE(updateRectX);
			rec.writeShortBE(updateRectY);
			rec.writeShortBE(updateRectW);
			rec.writeShortBE(updateRectH);
			if (updateRectEncoding == EncodingZlib && !recordFromBeginning) {
				// Here we cannot write Zlib-encoded rectangles because the
				// decoder won't be able to reproduce zlib stream state.
				if (!zlibWarningShown) {
					if (debug_rfbProto) System.out.println("Warning: Raw encoding will be used " +
					"instead of Zlib in recorded session.");
					zlibWarningShown = true;
				}
				rec.writeIntBE(EncodingRaw);
			} else {
				rec.writeIntBE(updateRectEncoding);
				if (updateRectEncoding == EncodingTight && !recordFromBeginning &&
						!tightWarningShown) {
					if (debug_rfbProto) System.out.println("Warning: Re-compressing Tight-encoded " +
					"updates for session recording.");
					tightWarningShown = true;
				}
			}
		}

		if (updateRectEncoding < 0 || updateRectEncoding > MaxNormalEncoding)
			return;

		if (updateRectX + updateRectW > framebufferWidth ||
				updateRectY + updateRectH > framebufferHeight) {
			throw new FBRectUpdateTooLargeException(
					"Framebuffer update rectangle too large: "
					+ updateRectW + "x" + updateRectH + " at (" +
					updateRectX + "," + updateRectY + ")");
		}
	}

	// Read CopyRect source X and Y.

	int copyRectSrcX, copyRectSrcY;

	void readCopyRect() throws IOException {
		if (debug_rfbProto) System.out.println("processing readCopyRect");
		copyRectSrcX = is.readUnsignedShort();
		copyRectSrcY = is.readUnsignedShort();

		// If the session is being recorded:
		if (rec != null) {
			rec.writeShortBE(copyRectSrcX);
			rec.writeShortBE(copyRectSrcY);
		}
	}


	//
	// Read a ServerCutText message
	//

	String readServerCutText() throws IOException {
		if (debug_rfbProto) System.out.println("processing readServerCutText");
		byte[] pad = new byte[3];
		readFully(pad);
		int len = is.readInt();
		byte[] text = new byte[len];
		readFully(text);
		return new String(text);
	}


	//
	// Read an integer in compact representation (1..3 bytes).
	// Such format is used as a part of the Tight encoding.
	// Also, this method records data if session recording is active and
	// the viewer's recordFromBeginning variable is set to true.
	//

	int readCompactLen() throws IOException {
		int[] portion = new int[3];
		portion[0] = is.readUnsignedByte();
		int byteCount = 1;
		int len = portion[0] & 0x7F;
		if ((portion[0] & 0x80) != 0) {
			portion[1] = is.readUnsignedByte();
			byteCount++;
			len |= (portion[1] & 0x7F) << 7;
			if ((portion[1] & 0x80) != 0) {
				portion[2] = is.readUnsignedByte();
				byteCount++;
				len |= (portion[2] & 0xFF) << 14;
			}
		}

		if (rec != null && recordFromBeginning)
			for (int i = 0; i < byteCount; i++)
				rec.writeByte(portion[i]);

		return len;
	}


	//
	// Write a FramebufferUpdateRequest message
	//

	void writeFramebufferUpdateRequest(int x, int y, int w, int h,
			boolean incremental)
	throws IOException
	{
		byte[] b = new byte[10];
		if (debug_rfbProto) System.out.println("processing writeFramebufferUpdateRequest");
		if (debug_rfbProto) System.out.println("\tincremental =  " + incremental + ", x = " + x + " , y = " + y + ", w = " + w + ", h = " + h);
		b[0] = (byte) FramebufferUpdateRequest;
		b[1] = (byte) (incremental ? 1 : 0);
		b[2] = (byte) ((x >> 8) & 0xff);
		b[3] = (byte) (x & 0xff);
		b[4] = (byte) ((y >> 8) & 0xff);
		b[5] = (byte) (y & 0xff);
		b[6] = (byte) ((w >> 8) & 0xff);
		b[7] = (byte) (w & 0xff);
		b[8] = (byte) ((h >> 8) & 0xff);
		b[9] = (byte) (h & 0xff);

		os.write(b);

		if (false && !incremental) {
			try {
				Thread.sleep(500);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			if (debug_rfbProto) System.out.println("FrameBufferUpdateRequest: incremental =  " + incremental + ", x = " + x + " , y = " + y + ", w = " + w + ", h = " + h);
			os.write(b);
		}
	}


	//
	// Write a SetPixelFormat message
	//

	void writeSetPixelFormat(int bitsPerPixel, int depth, boolean bigEndian,
			boolean trueColour,
			int redMax, int greenMax, int blueMax,
			int redShift, int greenShift, int blueShift)
	throws IOException
	{
		byte[] b = new byte[20];
		if (debug_rfbProto) System.out.println("processing writeSetPixelFormat");
		b[0]  = (byte) SetPixelFormat;
		b[4]  = (byte) bitsPerPixel;
		b[5]  = (byte) depth;
		b[6]  = (byte) (bigEndian ? 1 : 0);
		b[7]  = (byte) (trueColour ? 1 : 0);
		b[8]  = (byte) ((redMax >> 8) & 0xff);
		b[9]  = (byte) (redMax & 0xff);
		b[10] = (byte) ((greenMax >> 8) & 0xff);
		b[11] = (byte) (greenMax & 0xff);
		b[12] = (byte) ((blueMax >> 8) & 0xff);
		b[13] = (byte) (blueMax & 0xff);
		b[14] = (byte) redShift;
		b[15] = (byte) greenShift;
		b[16] = (byte) blueShift;

		os.write(b);
	}


	//
	// Write a FixColourMapEntries message.  The values in the red, green and
	// blue arrays are from 0 to 65535.
	//

	void writeFixColourMapEntries(int firstColour, int nColours,
			int[] red, int[] green, int[] blue)
	throws IOException
	{
		byte[] b = new byte[6 + nColours * 6];
		if (debug_rfbProto) System.out.println("processing writeFixColourMapEntries");

		b[0] = (byte) FixColourMapEntries;
		b[2] = (byte) ((firstColour >> 8) & 0xff);
		b[3] = (byte) (firstColour & 0xff);
		b[4] = (byte) ((nColours >> 8) & 0xff);
		b[5] = (byte) (nColours & 0xff);

		for (int i = 0; i < nColours; i++) {
			b[6 + i * 6]     = (byte) ((red[i] >> 8) & 0xff);
			b[6 + i * 6 + 1] = (byte) (red[i] & 0xff);
			b[6 + i * 6 + 2] = (byte) ((green[i] >> 8) & 0xff);
			b[6 + i * 6 + 3] = (byte) (green[i] & 0xff);
			b[6 + i * 6 + 4] = (byte) ((blue[i] >> 8) & 0xff);
			b[6 + i * 6 + 5] = (byte) (blue[i] & 0xff);
		}

		os.write(b);
	}


	//
	// Write a SetEncodings message
	//

	void writeSetEncodings(int[] encs, int len) throws IOException {
		byte[] b = new byte[4 + 4 * len];
		if (debug_rfbProto) System.out.println("processing writeSetEncodings");


		b[0] = (byte) SetEncodings;
		b[2] = (byte) ((len >> 8) & 0xff);
		b[3] = (byte) (len & 0xff);

		for (int i = 0; i < len; i++) {
			b[4 + 4 * i] = (byte) ((encs[i] >> 24) & 0xff);
			b[5 + 4 * i] = (byte) ((encs[i] >> 16) & 0xff);
			b[6 + 4 * i] = (byte) ((encs[i] >> 8) & 0xff);
			b[7 + 4 * i] = (byte) (encs[i] & 0xff);
		}

		os.write(b);
	}


	//
	// Write a ClientCutText message
	//

	void writeClientCutText(String text) throws IOException {
		byte[] b = new byte[8 + text.length()];
		if (debug_rfbProto) System.out.println("processing writeClientCutText");

		b[0] = (byte) ClientCutText;
		b[4] = (byte) ((text.length() >> 24) & 0xff);
		b[5] = (byte) ((text.length() >> 16) & 0xff);
		b[6] = (byte) ((text.length() >> 8) & 0xff);
		b[7] = (byte) (text.length() & 0xff);

		System.arraycopy(text.getBytes(), 0, b, 8, text.length());

		os.write(b);
	}


	//
	// A buffer for putting pointer and keyboard events before being sent.  This
	// is to ensure that multiple RFB events generated from a single Java Event 
	// will all be sent in a single network packet.  The maximum possible
	// length is 5 modifier down events, a single key event followed by 5
	// modifier up events i.e. 10 key events or 80 bytes.
	//

	byte[] eventBuf = new byte[80];
	int eventBufLen;


	// Useful shortcuts for modifier masks.

	final static int CTRL_MASK  = InputEvent.CTRL_MASK;
	final static int SHIFT_MASK = InputEvent.SHIFT_MASK;
	final static int META_MASK  = InputEvent.META_MASK;
	final static int ALT_MASK   = InputEvent.ALT_MASK;
	final static int WINDOWS_MASK = 16384;
	final static int R_CTRL_MASK  = 32768;
	final static int R_SHIFT_MASK = 65536;
	final static int R_ALT_MASK   = 131072;
	final static int R_META_MASK  = 262144;
	final static int BUTTON4_MASK = 8;
	final static int BUTTON5_MASK = 16;


	//
	// Write a pointer event message.  We may need to send modifier key events
	// around it to set the correct modifier state.
	//

	int pointerMask = 0;

	void writePointerEvent(MouseEvent evt) throws IOException {
		int modifiers = evt.getModifiers();
		MouseWheelEvent mouseWheelEvt = null;

		int mask2 = 2;
		int mask3 = 4;

		boolean old_debug_keyhandler = debug_keyhandler;

		if (!debug_mousehandler) debug_keyhandler = false;

		if (viewer.options.reverseMouseButtons2And3) {
			mask2 = 4;
			mask3 = 2;
		}

		// Note: For some reason, AWT does not set BUTTON1_MASK on left
		// button presses. Here we think that it was the left button if
		// modifiers do not include BUTTON2_MASK or BUTTON3_MASK.

		if (evt.getID() == MouseEvent.MOUSE_PRESSED) {
			if ((modifiers & InputEvent.BUTTON2_MASK) != 0) {
				pointerMask = mask2;
				modifiers &= ~ALT_MASK;
			} else if ((modifiers & InputEvent.BUTTON3_MASK) != 0) {
				pointerMask = mask3;
				modifiers &= ~META_MASK;
			} else {
				pointerMask = 1;
			}
		} else if (evt.getID() == MouseEvent.MOUSE_RELEASED) {
			pointerMask = 0;
			if ((modifiers & InputEvent.BUTTON2_MASK) != 0) {
				modifiers &= ~ALT_MASK;
			} else if ((modifiers & InputEvent.BUTTON3_MASK) != 0) {
				modifiers &= ~META_MASK;
			}
		} else if (evt.getID() == MouseEvent.MOUSE_WHEEL) {
			mouseWheelEvt = (MouseWheelEvent) evt;

			if (mouseWheelEvt.getWheelRotation() < 0) {
				pointerMask = BUTTON4_MASK;
			} else {
				pointerMask = BUTTON5_MASK;
			}
		}

		eventBufLen = 0;
		writeModifierKeyEvents(modifiers);

		int x = evt.getX();
		int y = evt.getY();

		if (x < 0) x = 0;
		if (y < 0) y = 0;

		eventBuf[eventBufLen++] = (byte) PointerEvent;
		eventBuf[eventBufLen++] = (byte) pointerMask;
		eventBuf[eventBufLen++] = (byte) ((x >> 8) & 0xff);
		eventBuf[eventBufLen++] = (byte) (x & 0xff);
		eventBuf[eventBufLen++] = (byte) ((y >> 8) & 0xff);
		eventBuf[eventBufLen++] = (byte) (y & 0xff);

		//
		// Always release all modifiers after an "up" event
		//

		if (pointerMask == 0) {
			writeModifierKeyEvents(0);
		}

		os.write(eventBuf, 0, eventBufLen);

		if (pointerMask == BUTTON4_MASK || pointerMask == BUTTON5_MASK) {
			pointerMask = 0;
			evt.consume();
		}
		debug_keyhandler = old_debug_keyhandler;
	}
	
	void initilizeModifierMenu() throws IOException {
		useMenuModifiers = true;

		// forget about modifier states once inside the panel
		oldModifiers = 0;
		winkey = 0;

		// fake an ALT key release, because the app has already seen one
		eventBufLen = 0;
		writeModifierKeyEvents(ALT_MASK);
		writeKeyEvent(0xffe9, false);		// keySym for ALT_L
		os.write(eventBuf, 0, eventBufLen);
		if (debug_keyhandler) System.out.println("Modifier key released, sent KeySym = " + 0xffe9);
	}
	
	void resetModifierMenu() {
		
		oldModifiers = 0;
		useMenuModifiers = false;
		// reset modifier states once outside the panel
		winkey = 0;
	}


	//
	// Write a key event message.  We may need to send modifier key events
	// around it to set the correct modifier state.  Also we need to translate
	// from the Java key values to the X keysym values used by the RFB protocol.
	//

	void writeKeyEvent(KeyEvent evt) throws IOException {

		int keyCode = evt.getKeyCode();
		// VK_UNDEFINED for KEY_TYPED events
		int keyChar = evt.getKeyChar();
		// CHAR_UNDEFINED if only modifiers present
		int keyLoc = evt.getKeyLocation();
		// KEY_LOCATION_UNKNOWN, STANDARD, LEFT, RIGHT, NUMPAD
		int keyMod = evt.getModifiers();
		int keySym = 0;    

		// setup event id flags
		boolean keyPressed = (evt.getID() == KeyEvent.KEY_PRESSED);
		boolean keyReleased = (evt.getID() == KeyEvent.KEY_RELEASED);
		boolean keyTyped = (evt.getID() == KeyEvent.KEY_TYPED);


		if (keyChar == KeyEvent.CHAR_UNDEFINED) {
			JCheckBox cbk = null;

			assert !keyTyped : "keyTyped shouldn't be true\n" ;

			if (keyPressed  && !evt.isActionKey()) {
				// we ignore all modifier key press events except for the WIN key ...
				// the WIN key is treated separately under actionkeys
				if (debug_keyhandler) System.out.println("Ignoring modifier key press. KeyCode = " + keyCode);
				return;
			}

			// handle key releases
			assert keyReleased || evt.isActionKey() : "keyReleased || actionkey should have been true\n" ;

			// distinguish left and right modifier keys ...
			if (keyLoc == KeyEvent.KEY_LOCATION_LEFT) {
				switch (keyCode) {
				case KeyEvent.VK_META : 
					keySym = 0xffe7;
					keyMod |= META_MASK;
					//cbk = viewer.modifierMenu.metaL;
					break;
				case KeyEvent.VK_CONTROL :
					keySym = 0xffe3;
					keyMod |= CTRL_MASK;
					cbk = viewer.modifierMenu.ctrlL;
					break;
				case KeyEvent.VK_SHIFT :
					keySym = 0xffe1; 
					keyMod |= SHIFT_MASK;
					cbk = viewer.modifierMenu.shiftL;
					break;
				case KeyEvent.VK_ALT : 
					keySym = 0xffe9; 
					keyMod |= ALT_MASK;
					cbk = viewer.modifierMenu.altL; 
					break;
				default : 
					break; // don't quite care about these ...
				}
			}

			if (keyLoc == KeyEvent.KEY_LOCATION_RIGHT) { 
				switch (keyCode) {
				case KeyEvent.VK_META : 
					keySym = 0xffe8;
					keyMod |= R_META_MASK;
					// cbk = viewer.modifierMenu.metaR;
					break;
				case KeyEvent.VK_CONTROL :
					if (viewer.transposeRightMods) {
						keySym = 0xffe3;
						keyMod |= CTRL_MASK;
					} else {
						keySym = 0xffe4; 
						keyMod |= R_CTRL_MASK;	
					}
					
					cbk = viewer.modifierMenu.ctrlR;
					break;
				case KeyEvent.VK_SHIFT :
					if (viewer.transposeRightMods) {
						keySym = 0xffe1; 
						keyMod |= SHIFT_MASK;
					} else {
						keySym = 0xffe2;
						keyMod |= R_SHIFT_MASK;	
					}
					
					cbk = viewer.modifierMenu.shiftR;
					break;
				case KeyEvent.VK_ALT :
					if (viewer.transposeRightMods) {
						keySym = 0xffe9; 
						keyMod |= ALT_MASK;
					} else {
						keySym = 0xffea; 
						keyMod |= R_ALT_MASK;	
					}
					
					cbk = viewer.modifierMenu.altR;
					break;
				default : 
					break; // don't quite care about these ...
				}				
			}

			if (useMenuModifiers) { // the spl panel is active ...
				if (keyCode == KeyEvent.VK_WINDOWS) {
					if (keyReleased) {
						cbk = viewer.modifierMenu.windows;
						if (debug_keyhandler) System.out.println("Handling WinKey in panel.");
					} else {
						return; // ignore WIN key presses within the panel
					}
				}

				if (cbk != null) {
					cbk.setSelected(!cbk.isSelected());
					if (debug_keyhandler) System.out.println("Setting checkbox state through keyboard");
					return;
				}

			}

			assert !useMenuModifiers : "shouldn't be true" + useMenuModifiers;

			// 
			// display the special keys menu on 2 quick presses of the ALT key
			//
			if (keyCode == KeyEvent.VK_ALT) {
				if (keyReleased) {
					if ((System.currentTimeMillis()-lastAltPressed) < 500) {
						viewer.vc.showSplKeysMenu();
						initilizeModifierMenu();
						return;
					}
					lastAltPressed = System.currentTimeMillis();
				}
			}


			//
			// need to deal with modifier key releases ...
			// the WIN key modifier is dealt with separately
			// skipped VK_META because i can't find the equivalent keySym
			// 
			if (keyCode == KeyEvent.VK_CONTROL || keyCode == KeyEvent.VK_SHIFT ||
					keyCode == KeyEvent.VK_META || keyCode == KeyEvent.VK_ALT) {

				if (winkey == 1){
					keyMod |= WINDOWS_MASK;	// bitmask for WIN key
				}

				eventBufLen = 0;
				writeModifierKeyEvents(keyMod);
				writeKeyEvent(keySym, false);
				os.write(eventBuf, 0, eventBufLen);
				if (debug_keyhandler) System.out.println("Modifier key released, sent KeySym = " + keySym);

				//
				// ensure we can continue to respond to modifier releases
				//
				oldModifiers &= ~keyMod;
			}  // keyCode == CTRL|SHIFT|ALT ...

			if (!evt.isActionKey()) return;
		}



		if (evt.isActionKey()) {

			//
			// An action key should be one of the following.
			// If not then just ignore the event else set up
			// keySym and continue on
			//

			if (debug_keyhandler) System.out.println("Handling action keys. KeyCode = " + keyCode);
			switch(keyCode) {
			case KeyEvent.VK_HOME:            keySym = 0xff50; break;
			case KeyEvent.VK_LEFT:            keySym = 0xff51; break;
			case KeyEvent.VK_UP:              keySym = 0xff52; break;
			case KeyEvent.VK_RIGHT:           keySym = 0xff53; break;
			case KeyEvent.VK_DOWN:            keySym = 0xff54; break;
			case KeyEvent.VK_PAGE_UP:         keySym = 0xff55; break;
			case KeyEvent.VK_PAGE_DOWN:       keySym = 0xff56; break;
			case KeyEvent.VK_END:             keySym = 0xff57; break;
			case KeyEvent.VK_INSERT:          keySym = 0xff63; break;
			case KeyEvent.VK_F1:              keySym = 0xffbe; break;
			case KeyEvent.VK_F2:              keySym = 0xffbf; break;
			case KeyEvent.VK_F3:              keySym = 0xffc0; break;
			case KeyEvent.VK_F4:              keySym = 0xffc1; break;
			case KeyEvent.VK_F5:              keySym = 0xffc2; break;
			case KeyEvent.VK_F6:              keySym = 0xffc3; break;
			case KeyEvent.VK_F7:              keySym = 0xffc4; break;
			case KeyEvent.VK_F8:              keySym = 0xffc5; break;
			case KeyEvent.VK_F9:              keySym = 0xffc6; break;
			case KeyEvent.VK_F10:             keySym = 0xffc7; break;
			case KeyEvent.VK_F11:             keySym = 0xffc8; break;
			case KeyEvent.VK_F12:             keySym = 0xffc9; break;
			case KeyEvent.VK_SCROLL_LOCK:     keySym = 0xff14; break;
			case KeyEvent.VK_PRINTSCREEN:     keySym = 0xff61; break;
			case KeyEvent.VK_PAUSE:           keySym = 0xff13; break;
			case KeyEvent.VK_CAPS_LOCK:       keySym = 0xffe5; break;
			case KeyEvent.VK_NUM_LOCK:        keySym = 0xff7f; break;
			case KeyEvent.VK_WINDOWS:
				// 
				// treat the WIN key specially because it is not really an 
				// action key like the others.
				//

				keySym = 0;
				if (keyPressed) { 
					// WinKey was pressed ...
					if (debug_keyhandler) System.out.println("Handling WIN key press. KeyCode = " + keyCode);
					winkey = 1;
					return; // treat this like we would alt|cntl|shift
				}
				if (keyReleased) { 
					// WinKey was released ...
					if (debug_keyhandler) System.out.println("Handling WIN key release. KeyCode = " + keyCode);
					winkey = (winkey == 1) ? 2 : 0;
				}
				break;
			default:
				return;
			}
		} else {

			//
			// A "normal" key press.  Ordinary ASCII characters go straight through.
			// For CTRL-<letter>, CTRL is sent separately so just send <letter>.
			// Backspace, tab, return, escape and delete have special keysyms.
			// Anything else we ignore.
			//

			keySym = keyChar; 	// default assignment, modified below
			if (debug_keyhandler) System.out.println("processing normal key. keySym = " + keySym);

			if (keyChar < 0x20) {
				if (evt.isControlDown()) {
					// need to understand why this is being done
					keySym = keyChar + 0x60;
				} else {
					switch(keyChar) {
					case KeyEvent.VK_BACK_SPACE: keySym = 0xff08; break;
					case KeyEvent.VK_TAB:        keySym = 0xff09; break;
					case KeyEvent.VK_ENTER:      keySym = 0xff0d; break;
					case KeyEvent.VK_ESCAPE:     keySym = 0xff1b; break;
					}
				}
			} else if (keyChar == 0x7f) {
				keySym = 0xffff;	// Delete
			} else if (keyChar > 0xff) {
				// JDK1.1 on X incorrectly passes some keysyms straight through,
				// so we do too.  JDK1.1.4 seems to have fixed this.
				// The keysyms passed are 0xff00 .. XK_BackSpace .. XK_Delete
				// Also, we pass through foreign currency keysyms (0x20a0..0x20af).
				// Anything else we ignore
				if ((keyChar < 0xff00 || keyChar > 0xffff) &&
						!(keyChar >= 0x20a0 && keyChar <= 0x20af))
					return;
			}
		}

		// Fake keyPresses for keys that only generates keyRelease events
		if ((keySym == 0xe5) || (keySym == 0xc5) || // XK_aring / XK_Aring
				(keySym == 0xe4) || (keySym == 0xc4) || // XK_adiaeresis / XK_Adiaeresis
				(keySym == 0xf6) || (keySym == 0xd6) || // XK_odiaeresis / XK_Odiaeresis
				(keySym == 0xa7) || (keySym == 0xbd) || // XK_section / XK_onehalf
				(keySym == 0xa3)) {                  // XK_sterling
			// Make sure we do not send keypress events twice on platforms
			// with correct JVMs (those that actually report KeyPress for all
			// keys)  
			if (keyPressed)
				brokenKeyPressed = true;

			if (keyReleased && !brokenKeyPressed) {
				// We've got a release event for this key, but haven't 
				// received a press. Fake it. 
				int msk = keyMod;
				if (winkey == 1){
					msk |= WINDOWS_MASK;	// bitmask for WIN key
				}

				if (debug_keyhandler) System.out.println("faking a key press for keySym = " + keySym);
				eventBufLen = 0;
				writeModifierKeyEvents(msk);
				writeKeyEvent(keySym, true);
				os.write(eventBuf, 0, eventBufLen);
			}

			if (keyReleased)
				brokenKeyPressed = false;  
		} // finished handling keys that only generate key releases ...

		if (useMenuModifiers == true) {
			//
			// we should get here when a non modifier key is being typed
			// after the spl keys menu is on display
			//
			assert keyChar != KeyEvent.CHAR_UNDEFINED : "keyChar undefined\n" ; 


			// we ignore the key press event
			//

			if (keyReleased) {
				keyMod = viewer.modifierMenu.getModifier();
				oldModifiers = 0;
				if(keyCode==27 && keyMod==0)
				{
					viewer.vc.hideSplKeysMenu();

					useMenuModifiers = false;
					// reset modifier states once outside the panel
					winkey = 0;
					return;
				}
				if (debug_keyhandler) System.out.println("key release in panel.");
				if (debug_keyhandler) System.out.println("keyMod = " + keyMod);
				if (debug_keyhandler) System.out.println("keySym = " + keySym);
//				sendFakeModifiersRelease();	// ensure base state
				eventBufLen = 0;
				writeModifierKeyEvents(keyMod);
				writeKeyEvent(keySym, true);	// send key press event
				os.write(eventBuf, 0, eventBufLen);

				eventBufLen = 0;
				writeModifierKeyEvents(keyMod);
				writeKeyEvent(keySym, false);	// send key release event as well
				writeModifierKeyEvents(0);
				os.write(eventBuf, 0, eventBufLen);

				viewer.vc.hideSplKeysMenu();
				
				useMenuModifiers = false;
				// reset modifier states once outside the panel
				winkey = 0;
			}
			return;
		} // if (useMenuModifiers == true) ...

		// 
		// process the WinKey as a modifier.
		//

		switch (winkey) {
		case 1 : 
			// the winkey pressed, add it to the modifier
			if (debug_keyhandler) System.out.println("Winkey pressed, setting up keyMod");
			keyMod |= WINDOWS_MASK;
			break;
		case 2 :  
			// the winkey was released, before the press was acted upon
			// need to treat it as a key in itself
			if (debug_keyhandler) System.out.println("Winkey released, setting up keySym to 0xffeb.");
			keySym = 0xffeb;
			keyMod |= WINDOWS_MASK;
			winkey = 0;
			break;
		default : 	// do nothing at all
			break;
		}

		eventBufLen = 0;
		writeModifierKeyEvents(keyMod);
		writeKeyEvent(keySym, keyPressed);
		os.write(eventBuf, 0, eventBufLen);

		return;
	}


	//
	// Add a raw key event with the given X keysym to eventBuf.
	//

	void writeKeyEvent(int keysym, boolean down) {
		eventBuf[eventBufLen++] = (byte) KeyboardEvent;
		eventBuf[eventBufLen++] = (byte) (down ? 1 : 0);
		eventBuf[eventBufLen++] = (byte) 0;
		eventBuf[eventBufLen++] = (byte) 0;
		eventBuf[eventBufLen++] = (byte) ((keysym >> 24) & 0xff);
		eventBuf[eventBufLen++] = (byte) ((keysym >> 16) & 0xff);
		eventBuf[eventBufLen++] = (byte) ((keysym >> 8) & 0xff);
		eventBuf[eventBufLen++] = (byte) (keysym & 0xff);
		if (debug_keyhandler) System.out.println("writing out key. keySym = " + keysym + " keydown? = " + down);
	}


	//
	// Write key events to set the correct modifier state.
	//

	int oldModifiers = 0;

	void writeModifierKeyEvents(int newModifiers) {
		if (debug_keyhandler) 
		{
			System.out.println("writing out modifiers");
			System.out.println("modifiers (win) 	:  " + ((newModifiers&WINDOWS_MASK) != (oldModifiers&WINDOWS_MASK)));
			System.out.println("modifiers (ctrl_l) 	:  " + ((newModifiers&CTRL_MASK) != (oldModifiers&CTRL_MASK)));
			System.out.println("modifiers (shift_l) 	:  " + ((newModifiers&SHIFT_MASK) != (oldModifiers&SHIFT_MASK)));
			System.out.println("modifiers (alt_l) 	:  " + ((newModifiers&ALT_MASK) != (oldModifiers&ALT_MASK)));
			System.out.println("modifiers (meta_l) 	:  " + ((newModifiers&META_MASK) != (oldModifiers&META_MASK)));
			System.out.println("modifiers (ctrl_r) 	:  " + ((newModifiers&R_CTRL_MASK) != (oldModifiers&R_CTRL_MASK)));
			System.out.println("modifiers (shift_r) 	:  " + ((newModifiers&R_SHIFT_MASK) != (oldModifiers&R_SHIFT_MASK)));
			System.out.println("modifiers (alt_r) 	:  " + ((newModifiers&R_ALT_MASK) != (oldModifiers&R_ALT_MASK)));
			System.out.println("modifiers (meta_r) 	:  " + ((newModifiers&R_META_MASK) != (oldModifiers&R_META_MASK)));
		}
		if ((newModifiers & WINDOWS_MASK) != (oldModifiers & WINDOWS_MASK))
			writeKeyEvent(0xffeb, (newModifiers & WINDOWS_MASK) != 0);

		if ((newModifiers & CTRL_MASK) != (oldModifiers & CTRL_MASK))
			writeKeyEvent(0xffe3, (newModifiers & CTRL_MASK) != 0);

		if ((newModifiers & SHIFT_MASK) != (oldModifiers & SHIFT_MASK))
			writeKeyEvent(0xffe1, (newModifiers & SHIFT_MASK) != 0);

		if ((newModifiers & META_MASK) != (oldModifiers & META_MASK))
			writeKeyEvent(0xffe7, (newModifiers & META_MASK) != 0);

		if ((newModifiers & ALT_MASK) != (oldModifiers & ALT_MASK))
			writeKeyEvent(0xffe9, (newModifiers & ALT_MASK) != 0);

		if ((newModifiers & R_CTRL_MASK) != (oldModifiers & R_CTRL_MASK))
			writeKeyEvent(0xffe4, (newModifiers & R_CTRL_MASK) != 0);

		if ((newModifiers & R_SHIFT_MASK) != (oldModifiers & R_SHIFT_MASK))
			writeKeyEvent(0xffe2, (newModifiers & R_SHIFT_MASK) != 0);

		if ((newModifiers & R_ALT_MASK) != (oldModifiers & R_ALT_MASK))
			writeKeyEvent(0xffea, (newModifiers & R_ALT_MASK) != 0);

		if ((newModifiers & R_META_MASK) != (oldModifiers & R_META_MASK))
			writeKeyEvent(0xffe8, (newModifiers & R_META_MASK) != 0);

		oldModifiers = newModifiers;
	}


	//
	// Compress and write the data into the recorded session file. This
	// method assumes the recording is on (rec != null).
	//

	void recordCompressedData(byte[] data, int off, int len) throws IOException {
		Deflater deflater = new Deflater();
		deflater.setInput(data, off, len);
		int bufSize = len + len / 100 + 12;
		byte[] buf = new byte[bufSize];
		deflater.finish();
		int compressedSize = deflater.deflate(buf);
		recordCompactLen(compressedSize);
		rec.write(buf, 0, compressedSize);
	}

	void recordCompressedData(byte[] data) throws IOException {
		recordCompressedData(data, 0, data.length);
	}

	//
	// Write an integer in compact representation (1..3 bytes) into the
	// recorded session file. This method assumes the recording is on
	// (rec != null).
	//

	void recordCompactLen(int len) throws IOException {
		byte[] buf = new byte[3];
		int bytes = 0;
		buf[bytes++] = (byte)(len & 0x7F);
		if (len > 0x7F) {
			buf[bytes-1] |= 0x80;
			buf[bytes++] = (byte)(len >> 7 & 0x7F);
			if (len > 0x3FFF) {
				buf[bytes-1] |= 0x80;
				buf[bytes++] = (byte)(len >> 14 & 0xFF);
			}
		}
		rec.write(buf, 0, bytes);
	}

	public void startTiming() {
		timing = true;

		// Carry over up to 1s worth of previous rate for smoothing.

		if (timeWaitedIn100us > 10000) {
			timedKbits = timedKbits * 10000 / timeWaitedIn100us;
			timeWaitedIn100us = 10000;
		}
	}

	public void stopTiming() {
		timing = false; 
		if (timeWaitedIn100us < timedKbits/2)
			timeWaitedIn100us = timedKbits/2; // upper limit 20Mbit/s
	}

	public long kbitsPerSecond() {
		return timedKbits * 10000 / timeWaitedIn100us;
	}

	public long timeWaited() {
		return timeWaitedIn100us;
	}

	public void readFully(byte b[]) throws IOException {
		readFully(b, 0, b.length);
	}

	public void readFully(byte b[], int off, int len) throws IOException {
		long before = 0;
		if (timing)
			before = System.currentTimeMillis();

		is.readFully(b, off, len);

		if (timing) {
			long after = System.currentTimeMillis();
			long newTimeWaited = (after - before) * 10;
			int newKbits = len * 8 / 1000;

			// limit rate to between 10kbit/s and 40Mbit/s

			if (newTimeWaited > newKbits*1000) newTimeWaited = newKbits*1000;
			if (newTimeWaited < newKbits/4)    newTimeWaited = newKbits/4;

			timeWaitedIn100us += newTimeWaited;
			timedKbits += newKbits;
		}
	}

	void sendFakeModifiersRelease () {

		try {
			eventBufLen = 0;
			writeKeyEvent(0xffe3, false);		// control_l
			writeKeyEvent(0xffe1, false);		// shift_l
			writeKeyEvent(0xffe9, false);		// alt_l
			writeKeyEvent(0xffeb, false);		// WinKey
			os.write(eventBuf, 0, eventBufLen);

			eventBufLen = 0;
			writeKeyEvent(0xffe4, false);		// control_r
			writeKeyEvent(0xffe2, false);		// shift_r
			writeKeyEvent(0xffea, false);		// alt_r
			os.write(eventBuf, 0, eventBufLen);

			oldModifiers  = 0;	// clear global modifier state 
			if (debug_keyhandler) System.out.println("Sent fake modifer keys releases.");         
		} catch (IOException e) {

			if (debug_keyhandler) System.out.println("Failed to send fake modifer keys releases.");
			e.printStackTrace();
		}
	}

}
