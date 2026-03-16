// DayZular Framework - Logger Settings
// JSON-based configuration for logging system

class DayZularLoggerSettings
{
    int logLevel = 1; // Default to Info (0=Debug, 1=Info, 2=Warn, 3=Error, 4=None)
    int refreshRateInSeconds = 60;
    bool enableFileLogging = false; // Disabled by default for production
    bool enableConsoleOutput = false; // Disabled by default for production

    void MakeDirectoryIfNotExists()
    {
        if (!FileExist(DayZular_ROOT_FOLDER))
            MakeDirectory(DayZular_ROOT_FOLDER);

        if (!FileExist(DayZular_LOG_FOLDER))
            MakeDirectory(DayZular_LOG_FOLDER);

        if (!FileExist(DayZular_LOGGER_CONFIG_DIR))
            MakeDirectory(DayZular_LOGGER_CONFIG_DIR);
        
        if (!FileExist(DayZular_LOGGER_LOG_DIR))
            MakeDirectory(DayZular_LOGGER_LOG_DIR);
    }

    void Save()
    {
        MakeDirectoryIfNotExists();
        JsonFileLoader<DayZularLoggerSettings>.JsonSaveFile(DayZular_LOGGER_CONFIG_FILE, this);
    }

    static ref DayZularLoggerSettings Load()
    {
        DayZularLoggerSettings settings = new DayZularLoggerSettings();

        settings.MakeDirectoryIfNotExists();

        if (FileExist(DayZular_LOGGER_CONFIG_FILE))
        {
            JsonFileLoader<DayZularLoggerSettings>.JsonLoadFile(DayZular_LOGGER_CONFIG_FILE, settings);
            return settings;
        }

        settings.Save();
        return settings;
    }
}
