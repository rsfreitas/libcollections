
package cplugin;

import cplugin.CpluginType;

public class Cplugin {
    private int caller_id;

    Cplugin(int caller_id) {
        this.caller_id = caller_id;
    }

    public int getCallerId() {
        return caller_id;
    }

    public String getCurrentFunctionName() {
        return Thread.currentThread().getStackTrace()[0].getMethodName();
    }
}

