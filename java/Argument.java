
package cplugin;

public class Argument {
    private String name;
    private Object value;

    Argument(String name, Object value) {
        this.name = name;
        this.value = value;
    }

    public Object getValue() {
        return value;
    }

    public String getName() {
        return name;
    }

    @Override
    public boolean equals(Object o) {
        if ((o instanceof Argument) == false)
            return false;

        Argument p = (Argument)o;

        return this.name.equals(p.getName());
    }
}

