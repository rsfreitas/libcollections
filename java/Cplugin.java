
package cplugin;

import cplugin.CpluginType;

/**
 * A class to call libcollections API and hold a reference to the C version of
 * the library.
 */
public class Cplugin {
    private static Cplugin instance = null;

    private Cplugin() {
        System.load("/usr/local/lib/libcollections_jni.so");
    }

    public static Cplugin getInstance() {
        if (null == instance)
            instance = new Cplugin();

        return instance;
    }

    public void setReturnValue(String name, CpluginType type, Object value) {
    }

    public Object getArgument(String name) {
        return null;
    }
}

