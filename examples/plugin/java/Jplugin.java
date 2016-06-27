
import cplugin.CpluginEntryAPI;
import cplugin.CpluginModule;
import cplugin.CpluginType;
import cplugin.CpluginArguments;
import cplugin.CpluginObject;

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
    public String getCreator() {
        System.out.println("getCreator");
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
    public CpluginObject foo_int() {
        System.out.println("foo_int");

        return new CpluginObject(CpluginType.CL_INT, 42);
    }

    public CpluginObject foo_uint() {
        System.out.println("foo_uint");

        return new CpluginObject(CpluginType.CL_UINT, 420);
    }

    public CpluginObject foo_sint() {
        System.out.println("foo_sint");

        return new CpluginObject(CpluginType.CL_SINT, 421);
    }

    public CpluginObject foo_usint() {
        System.out.println("foo_usint");

        return new CpluginObject(CpluginType.CL_USINT, 4201);
    }

    public CpluginObject foo_char() {
        System.out.println("foo_char");

        return new CpluginObject(CpluginType.CL_CHAR, 'a');
    }

    public CpluginObject foo_uchar() {
        System.out.println("foo_uchar");

        return new CpluginObject(CpluginType.CL_UCHAR, 230);
    }

    public CpluginObject foo_float() {
        System.out.println("foo_float");

        return new CpluginObject(CpluginType.CL_FLOAT, 42.5f);
    }

    public CpluginObject foo_double() {
        System.out.println("foo_double");

        return new CpluginObject(CpluginType.CL_DOUBLE, 4.2);
    }

    public CpluginObject foo_boolean() {
        System.out.println("foo_boolean");

        return new CpluginObject(CpluginType.CL_BOOLEAN, true);
    }

    public CpluginObject foo_long() {
        System.out.println("foo_long");

        return new CpluginObject(CpluginType.CL_LONG, 42000);
    }

    public CpluginObject foo_ulong() {
        System.out.println("foo_ulong");

        return new CpluginObject(CpluginType.CL_ULONG, 420001);
    }

    public CpluginObject foo_llong() {
        System.out.println("foo_llong");

        return new CpluginObject(CpluginType.CL_LLONG, 420009);
    }

    public CpluginObject foo_ullong() {
        System.out.println("foo_ullong");

        return new CpluginObject(CpluginType.CL_LLONG, 4200019);
    }

    /* With arguments, with return */
    public void foo_args(CpluginArguments args) {
        System.out.println("Number of arguments: " + args.argCount());
        int arg1 = (Integer)args.getArgument("arg1");
        int arg2 = (Integer)args.getArgument("arg2");
        short arg3 = (Short)args.getArgument("arg3");
        short arg4 = (Short)args.getArgument("arg4");
        byte arg5 = (Byte)args.getArgument("arg5");
        byte arg6 = (Byte)args.getArgument("arg6");
        float arg7 = (Float)args.getArgument("arg7");
        double arg8 = (Double)args.getArgument("arg8");
        int arg9 = (Integer)args.getArgument("arg9");
        int arg10 = (Integer)args.getArgument("arg10");
        long arg11 = (Long)args.getArgument("arg11");
        long arg12 = (Long)args.getArgument("arg12");
        boolean arg13 = (Boolean)args.getArgument("arg13");
        String arg14 = (String)args.getArgument("arg14");

        System.out.println("argument 1 = " + arg1);
        System.out.println("argument 2 = " + arg2);
        System.out.println("argument 3 = " + arg3);
        System.out.println("argument 4 = " + arg4);
        System.out.println("argument 5 = " + arg5);
        System.out.println("argument 6 = " + arg6);
        System.out.println("argument 7 = " + arg7);
        System.out.println("argument 8 = " + arg8);
        System.out.println("argument 9 = " + arg9);
        System.out.println("argument 10 = " + arg10);
        System.out.println("argument 11 = " + arg11);
        System.out.println("argument 12 = " + arg12);
        System.out.println("argument 13 = " + arg13);
        System.out.println("argument 14 = " + arg14);
    }
}

