
package cplugin;

/**
 * This sequence must follow the enum located in the <cl/cl_type.h> file
 * from libcollections.
 */
public enum CpluginType {
    CL_VOID(0),
    CL_CHAR(1),
    CL_UCHAR(2),
    CL_INT(3),
    CL_UINT(4),
    CL_SINT(5),
    CL_USINT(6),
    CL_FLOAT(7),
    CL_DOUBLE(8),
    CL_LONG(9),
    CL_ULONG(10),
    CL_LLONG(11),
    CL_ULLONG(12),
    CL_POINTER(13),
    CL_STRING(14),      /* 'char *' strings */
    CL_BOOLEAN(15),
    CL_CSTRING(16);      /* collections strings */

    private int value;

    CpluginType(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }
}

