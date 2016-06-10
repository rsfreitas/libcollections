
package cplugin;

import cplugin.CpluginType;

public class CpluginObject {
    private Object value;
    private CpluginType type;
    private String function_name;
    private int caller_id;

    public CpluginObject(CpluginType type, Object value, String function_name,
                         int caller_id)
    {
        this.type = type;
        this.value = value;
        this.function_name = function_name;
        this.caller_id = caller_id;
    }

    public Object getValue() {
        return value;
    }

    public CpluginType getType() {
        return type;
    }
}

