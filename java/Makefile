
#
# TODO: Needs to fix this .jar creation and use... still not working
#

CC = gcc

OUTCLASSPATH = cplugin/
JNI = ../bin/libcollections_jni.so
JAVAC = javac
JAR_FILE = ../bin/cplugin.jar

JAVA_OBJS = \
	CpluginData.java        \
	CpluginEntryAPI.java    \
	CpluginModule.java

jar:
	$(JAVAC) -d $(OUTCLASSPATH) $(JAVA_OBJS)
	jar -cvf $(JAR_FILE) cplugin/*.class

clean:
	rm -rf ../java/*.class $(JAR_FILE)

