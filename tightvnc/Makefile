#
# Making the VNC applet.
#

CP = cp
JC = javac
JCFLAGS = -target 1.5 -source 1.5
JAR = jar
JARSIGNER = jarsigner
#ARCHIVE = VncViewer.jar
ARCHIVE = vncviewer_libvirt.jar
MANIFEST = MANIFEST.MF
PAGES = index.vnc
INSTALL_DIR = /usr/local/vnc/classes

CLASSES = *.class

SOURCES = VncViewer.java RfbProto.java AuthPanel.java VncCanvas.java \
	  VncCanvas2.java CustomPointer.java \
	  OptionsFrame.java ClipboardFrame.java ButtonPanel.java \
	  DesCipher.java CapabilityInfo.java CapsContainer.java \
	  RecordingFrame.java SessionRecorder.java \
	  SocketFactory.java HTTPConnectSocketFactory.java \
	  HTTPConnectSocket.java ReloginPanel.java \
	  InStream.java MemInStream.java ZlibInStream.java \
	  UnknownRFBEntityException.java \
	  FBRectUpdateTooLargeException.java \
	  CmdPopMenu.java

all: $(CLASSES) $(ARCHIVE)

$(CLASSES): $(SOURCES)
	$(JC) $(JCFLAGS) -O $(SOURCES)

$(ARCHIVE): $(CLASSES) $(MANIFEST)
	cp -f $(MANIFEST) $(MANIFEST).TMP
	echo -n "revision-number: " >> $(MANIFEST).TMP
	svn info | grep 'Last Changed Rev' | awk '{print $$4}' >> $(MANIFEST).TMP
	$(JAR) cfm $(ARCHIVE) $(MANIFEST).TMP $(CLASSES)
	rm -f $(MANIFEST).TMP
	$(JARSIGNER) -keystore ./keystore -storepass coriolis $(ARCHIVE) colama

install: $(CLASSES) $(ARCHIVE)
	$(CP) $(CLASSES) $(ARCHIVE) $(PAGES) $(INSTALL_DIR)

export:: $(CLASSES) $(ARCHIVE) $(PAGES)
	@$(ExportJavaClasses)

clean::
	$(RM) *.class *.jar
