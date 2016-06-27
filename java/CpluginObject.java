
package cplugin;

import cplugin.CpluginType;

public class CpluginObject {
    private Object value;
    private CpluginType type;

    public CpluginObject(CpluginType type, Object value)
    {
        this.type = type;
        this.value = value;
    }

    public Object getValue_as_Object() {
        return value;
    }

    public byte getValue_as_byte() {
        return (Byte)value;
    }

    public short getValue_as_short() {
        return (Short)value;
    }

    public char getValue_as_char() {
        return (Character)value;
    }

    public int getValue_as_int() {
        return (Integer)value;
    }

    public long getValue_as_long() {
        return (Long)value;
    }

    public float getValue_as_float() {
        return (Float)value;
    }

    public double getValue_as_double() {
        return (Double)value;
    }

    public boolean getValue_as_boolean() {
        return (Boolean)value;
    }

    public CpluginType getType() {
        return type;
    }
}

