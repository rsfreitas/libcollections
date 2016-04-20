
/**
 * A Singleton class to hold a reference to the C version of libcollections.
 */
public class CpluginData {
    private static CpluginData d;

    private CpluginData() {
        System.load("/usr/local/lib/libcollections_jni.so");
    }

    public static CpluginData getInstance() {
        if (d == null)
            d = new CpluginData();

        return d;
    }
}

