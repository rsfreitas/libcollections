
package cplugin;

/**
 * An interface to set the plugin information, so a plugin manager
 * may known them.
 */
public interface CpluginEntryAPI {
    public String getName();
    public String getVersion();
    public String getAuthor();
    public String getDescription();
    public String getAPI();
}

