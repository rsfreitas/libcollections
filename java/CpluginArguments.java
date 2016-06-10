
package cplugin;

import cplugin.Argument;
import java.util.ArrayList;

public class CpluginArguments {
    private ArrayList<Argument> args;

    CpluginArguments() {
        args = new ArrayList<>();
    }

    public void addArgument(String name, Object value) {
        args.add(new Argument(name, value));
    }

    public void addArgument(String name, byte value) {
        args.add(new Argument(name, value));
    }

    public void addArgument(String name, short value) {
        args.add(new Argument(name, value));
    }

    public void addArgument(String name, char  value) {
        args.add(new Argument(name, value));
    }

    public void addArgument(String name, int value) {
        args.add(new Argument(name, value));
    }

    public void addArgument(String name, long value) {
        args.add(new Argument(name, value));
    }

    public void addArgument(String name, float value) {
        args.add(new Argument(name, value));
    }

    public void addArgument(String name, double value) {
        args.add(new Argument(name, value));
    }

    public Object getArgument(String name) {
        int p = args.indexOf(new Argument(name, null));

        if (p < 0)
            return null;

        Argument a = args.get(p);

        return a.getValue();
    }
}

