
#
# TODO: Needs to fix this .jar creation and use... still not working
#

CC = gcc

OUTCLASSPATH = cplugin/
JAVAC = javac
JAR_FILE = ../bin/cplugin.jar

JAVA_OBJS = \
	CpluginEntryAPI.java    \
	CpluginModule.java	\
	CpluginType.java	\
	Cplugin.java

jar:
	$(JAVAC) -d $(OUTCLASSPATH) $(JAVA_OBJS)
	jar -cvf $(JAR_FILE) cplugin/cplugin/*.class

clean:
	rm -rf ../java/*.class $(JAR_FILE)

