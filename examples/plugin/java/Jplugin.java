
import cplugin.CpluginEntryAPI;
import cplugin.CpluginModule;

/**
 * We need to follow the Java coding style and keep the main class with the
 * same name as the source file. This is required by the libcollections.
 *
 * Another requirement is the class constructor, it must receive no arguments.
 */
public class Jplugin implements CpluginEntryAPI, CpluginModule {
    private final String name = "jplugin";
    private final String version = "0.1";
    private final String creator = "Rodrigo Freitas";
    private final String description = "Java plugin example";
    private final String API =
    "{" +
        "\"API\": [" +
            "{ \"name\": \"foo_int\", \"return_type\": \"int\" }," +
            "{ \"name\": \"foo_uint\", \"return_type\": \"uint\" }," +
            "{ \"name\": \"foo_char\", \"return_type\": \"char\" }," +
            "{ \"name\": \"foo_uchar\", \"return_type\": \"uchar\" }," +
            "{ \"name\": \"foo_sint\", \"return_type\": \"sint\" }," +
            "{ \"name\": \"foo_usint\", \"return_type\": \"usint\" }," +
            "{ \"name\": \"foo_float\", \"return_type\": \"float\" }," +
            "{ \"name\": \"foo_double\", \"return_type\": \"double\" }," +
            "{ \"name\": \"foo_long\", \"return_type\": \"long\" }," +
            "{ \"name\": \"foo_ulong\", \"return_type\": \"ulong\" }," +
            "{ \"name\": \"foo_llong\", \"return_type\": \"llong\" }," +
            "{ \"name\": \"foo_ullong\", \"return_type\": \"ullong\" }," +
            "{ \"name\": \"foo_boolean\", \"return_type\": \"boolean\" }," +
            "{ \"name\": \"foo_args\", \"return_type\": \"void\", \"arguments\": [" +
                "{ \"name\": \"arg1\", \"type\": \"int\" }," +
                "{ \"name\": \"arg2\", \"type\": \"uint\" }," +
                "{ \"name\": \"arg3\", \"type\": \"sint\" }," +
                "{ \"name\": \"arg4\", \"type\": \"usint\" }," +
                "{ \"name\": \"arg5\", \"type\": \"char\" }," +
                "{ \"name\": \"arg6\", \"type\": \"uchar\" }," +
                "{ \"name\": \"arg7\", \"type\": \"float\" }," +
                "{ \"name\": \"arg8\", \"type\": \"double\" }," +
                "{ \"name\": \"arg9\", \"type\": \"long\" }," +
                "{ \"name\": \"arg10\", \"type\": \"ulong\" }," +
                "{ \"name\": \"arg11\", \"type\": \"llong\" }," +
                "{ \"name\": \"arg12\", \"type\": \"ullong\" }," +
                "{ \"name\": \"arg13\", \"type\": \"boolean\" }," +
                "{ \"name\": \"arg14\", \"type\": \"string\" }" +
            "] }" +
        "]"+
    "}";

    /** Module info functions */
    @Override
    public String getName() {
        System.out.println("getName");
        return name;
    }

    @Override
    public String getVersion() {
        System.out.println("getVersion");
        return version;
    }

    @Override
    public String getAuthor() {
        System.out.println("getAuthor");
        return creator;
    }

    @Override
    public String getDescription() {
        System.out.println("getDescription");
        return description;
    }

    @Override
    public String getAPI() {
        System.out.println("getAPI");
        return API;
    }

    /** Module init/uninit functions */
    @Override
    public int module_init() {
        System.out.println("module init");
        return 0;
    }

    @Override
    public void module_uninit() {
        System.out.println("module uninit");
    }

    Jplugin() {
        System.out.println("Constructor");
    }

    /** Module API Examples */
    public int foo_int() {
        System.out.println("foo_int");
        return 42;
    }

    public int foo_uint() {
        System.out.println("foo_uint");
        return 420;
    }

    public short foo_sint() {
        System.out.println("foo_sint");
        return 421;
    }

    public short foo_usint() {
        System.out.println("foo_usint");
        return 4201;
    }

    public byte foo_char() {
        System.out.println("foo_char");
        return 'a';
    }

    public byte foo_uchar() {
        System.out.println("foo_uchar");
        return (byte)230;
    }

    public float foo_float() {
        System.out.println("foo_float");
        return 42.5f;
    }

    public double foo_double() {
        System.out.println("foo_double");
        return 4.2;
    }

    public boolean foo_boolean() {
        System.out.println("foo_boolean");
        return true;
    }

    public int foo_long() {
        System.out.println("foo_long");
        return 42000;
    }

    public int foo_ulong() {
        System.out.println("foo_ulong");
        return 420001;
    }

    public long foo_llong() {
        System.out.println("foo_llong");
        return 420009;
    }

    public long foo_ullong() {
        System.out.println("foo_ullong");
        return 4200019;
    }

    /* With arguments, with return */
    public void foo_args(String args) {
        /* XXX: The arguments are in a JSON format. */
        System.out.println("Arguments: " + args);
    }
}

