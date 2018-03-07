
OUTCLASSPATH = out/
JAVAC = javac
JAR_FILE = ../bin/cplugin.jar
DEST_DIR = /usr/local/lib

JAVA_OBJS = \
	CpluginEntryAPI.java    	\
	CpluginModule.java

jar:
	$(shell test -d $(OUTCLASSPATH) || mkdir $(OUTCLASSPATH))
	$(JAVAC) -d $(OUTCLASSPATH) $(JAVA_OBJS)
	jar -cvf $(JAR_FILE) -C $(OUTCLASSPATH) .

clean:
	rm -rf $(OUTCLASSPATH)/cplugin/*.class $(JAR_FILE)

purge: clean jar

install:
	cp -f $(JAR_FILE) $(DEST_DIR)

