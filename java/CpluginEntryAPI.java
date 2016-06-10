
package cplugin;

/**
 * An interface to set the plugin informations, so a plugin manager
 * may known them.
 */
public interface CpluginEntryAPI {
    public String getName();
    public String getVersion();
    public String getCreator();
    public String getDescription();
    public String getAPI();
}

