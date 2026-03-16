// DayZular Framework - Logger Service
// Standalone singleton logging service with log levels
// No external dependencies (replaces CF-based LoggingModule)

enum DayZularLogLevel
{
    Debug = 0,
    Info,
    Warn,
    Error,
    None
}

class DayZularLogger : DayZularService
{
    private static ref DayZularLogger s_Instance;
    
    private DayZularLogLevel m_LogLevel;
    private FileHandle m_FileHandle;
    private bool m_FileLoggingEnabled;
    private bool m_ConsoleOutputEnabled;
    private string m_LogPrefix;
    private ref DayZularLoggerSettings m_Settings;
    private float m_TimeSinceLastReload;
    private float m_ReloadInterval;
    
    void DayZularLogger()
    {
        m_LogLevel = DayZularLogLevel.Info; // Default to Info level
        m_FileLoggingEnabled = false;
        m_ConsoleOutputEnabled = false; // Disabled by default - enable via config for debugging
        m_LogPrefix = "[DayZular]";
        m_TimeSinceLastReload = 0;
        m_ReloadInterval = 60;
        
        LoadSettings();
    }
    
    void ~DayZularLogger()
    {
        if (m_FileHandle)
        {
            CloseFile(m_FileHandle);
        }
    }
    
    // Singleton access
    static DayZularLogger GetInstance()
    {
        if (!s_Instance)
        {
            s_Instance = new DayZularLogger();
        }
        return s_Instance;
    }
    
    // Configuration
    void SetLogLevel(DayZularLogLevel level)
    {
        m_LogLevel = level;
    }
    
    DayZularLogLevel GetLogLevel()
    {
        return m_LogLevel;
    }
    
    void SetPrefix(string prefix)
    {
        m_LogPrefix = prefix;
    }
    
    // Load settings from JSON config
    void LoadSettings()
    {
        m_Settings = DayZularLoggerSettings.Load();
        
        if (m_Settings)
        {
            m_LogLevel = m_Settings.logLevel;
            m_ReloadInterval = m_Settings.refreshRateInSeconds;
            m_ConsoleOutputEnabled = m_Settings.enableConsoleOutput;
            
            if (m_Settings.enableFileLogging && !m_FileLoggingEnabled)
            {
                EnableFileLogging();
            }
            else if (!m_Settings.enableFileLogging && m_FileLoggingEnabled)
            {
                DisableFileLogging();
            }
        }
    }
    
    // Update method for runtime config reload
    void Update(float deltaTime)
    {
        m_TimeSinceLastReload += deltaTime;
        
        if (m_TimeSinceLastReload >= m_ReloadInterval)
        {
            m_TimeSinceLastReload = 0;
            LoadSettings();
        }
    }
    
    // Enable file logging to $profile:DayZular/Logs/
    void EnableFileLogging()
    {
        if (m_FileLoggingEnabled)
            return;
        
        MakeDirectoryIfNotExists();
        
        string filePath = string.Format(DayZular_LOGGER_LOG_FILE, GenerateFullTimestamp());
        m_FileHandle = OpenFile(filePath, FileMode.WRITE);
        
        if (m_FileHandle)
        {
            FPrintln(m_FileHandle, "DayZular Log - Created: " + GenerateFullTimestamp());
            m_FileLoggingEnabled = true;
        }
    }
    
    void DisableFileLogging()
    {
        if (m_FileHandle)
        {
            CloseFile(m_FileHandle);
            m_FileHandle = null;
        }
        m_FileLoggingEnabled = false;
    }
    
    // Enable/disable console output
    void SetConsoleOutput(bool enabled)
    {
        m_ConsoleOutputEnabled = enabled;
    }
    
    // Log methods
    void LogDebug(string message)
    {
        Log(message, DayZularLogLevel.Debug);
    }
    
    void LogInfo(string message)
    {
        Log(message, DayZularLogLevel.Info);
    }
    
    void LogWarning(string message)
    {
        Log(message, DayZularLogLevel.Warn);
    }
    
    void LogError(string message)
    {
        Log(message, DayZularLogLevel.Error);
    }
    
    // Core log method
    void Log(string message, DayZularLogLevel level)
    {
        if (level < m_LogLevel)
            return;
        
        string formatted = m_LogPrefix + " " + GetLogLevelTag(level) + " " + message;
        
        // Write to console if enabled
        if (m_ConsoleOutputEnabled)
        {
            Print(formatted);
        }
        
        // Write to file if enabled
        if (m_FileLoggingEnabled && m_FileHandle)
        {
            string timestamp = GenerateShortTimeString();
            FPrintln(m_FileHandle, timestamp + " | " + GetLogLevelTag(level) + " | " + message);
        }
    }
    
    // Helpers
    private string GetLogLevelTag(DayZularLogLevel level)
    {
        switch (level)
        {
            case DayZularLogLevel.Debug:   return "[DEBUG]";
            case DayZularLogLevel.Info:    return "[INFO]";
            case DayZularLogLevel.Warn:    return "[WARN]";
            case DayZularLogLevel.Error:   return "[ERROR]";
        }
        return "";
    }
    
    private void MakeDirectoryIfNotExists()
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
    
    private string GenerateShortDateString()
    {
        int year, month, day;
        GetYearMonthDay(year, month, day);
        return "" + year + "_" + month + "_" + day;
    }
    
    private string GenerateShortTimeString()
    {
        int hour, minute, second;
        GetHourMinuteSecond(hour, minute, second);
        return "" + hour + "_" + minute + "_" + second;
    }
    
    private string GenerateFullTimestamp()
    {
        return GenerateShortDateString() + "-" + GenerateShortTimeString();
    }
    
    // Cleanup singleton on shutdown
    static void Destroy()
    {
        if (s_Instance)
        {
            delete s_Instance;
            s_Instance = null;
        }
    }
}

// Global convenience function
static DayZularLogger DZLog()
{
    return DayZularLogger.GetInstance();
}
