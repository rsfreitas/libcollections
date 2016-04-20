
//import cplugin.CpluginEntryAPI;
//import cplugin.CpluginModule;

/**
 * We need to follow the Java coding style and keep the main class with the
 * same name as the source file. This is required by the libcollections.
 *
 * Another requirement is the class constructor, it must receive no arguments.
 */
public class Jplugin {// implements CpluginEntryAPI, CpluginModule {
    private final String name = "jplugin";
    private final String version = "0.1";
    private final String creator = "Rodrigo Freitas";
    private final String description = "Java plugin example";
    private final String API = "{"+
    "\"API\": [" +
        "{ \"name\": \"foo_int\", \"return_type\": \"int\" }"+
    "]"+
"}";
/*    private final String API = "{"+
    "\"API\": [" +
        "{ \"name\": \"foo_int\", \"return_type\": \"int\" },"+
        "{ \"name\": \"foo_args\", \"return_type\": \"void\","+
            "\"arguments\": ["+
                "{ \"name\": \"arg1\", \"type\": \"int\" },"+
                "{ \"name\": \"arg2\", \"type\": \"uint\" },"+
                "{ \"name\": \"arg3\", \"type\": \"sint\" },"+
                "{ \"name\": \"arg4\", \"type\": \"usint\" },"+
                "{ \"name\": \"arg5\", \"type\": \"char\" },"+
                "{ \"name\": \"arg6\", \"type\": \"uchar\" },"+
                "{ \"name\": \"arg7\", \"type\": \"float\" },"+
                "{ \"name\": \"arg8\", \"type\": \"double\" },"+
                "{ \"name\": \"arg9\", \"type\": \"long\" },"+
                "{ \"name\": \"arg10\", \"type\": \"ulong\" },"+
                "{ \"name\": \"arg11\", \"type\": \"llong\" },"+
                "{ \"name\": \"arg12\", \"type\": \"ullong\" },"+
                "{ \"name\": \"arg13\", \"type\": \"boolean\" },"+
                "{ \"name\": \"arg14\", \"type\": \"string\" }"+
            "]"+
        "}"+
    "]"+
"}";*/

    /** Module info functions */
    //@Override
    public String getName() {
        System.out.println("getName");
        return name;
    }

    //@Override
    public String getVersion() {
        System.out.println("getVersion");
        return version;
    }

    //@Override
    public String getCreator() {
        System.out.println("getCreator");
        return creator;
    }

    //@Override
    public String getDescription() {
        System.out.println("getDescription");
        return description;
    }

    //@Override
    public String getAPI() {
        System.out.println("getAPI");
        return API;
    }

    /** Module init/uninit functions */
    //@Override
    public int module_init() {
        System.out.println("module init");
        return 0;
    }

    //@Override
    public void module_uninit() {
        System.out.println("module uninit");
    }

    Jplugin() {
        System.out.println("Construtor");
    }

    /** Module API */
    public int foo_int() {
        System.out.println("foo_int");
        return 42;
    }

/*    public void foo_args(int arg1, int arg2) {
        System.out.println(arg1);
        System.out.println(arg2);
    }*/
}

