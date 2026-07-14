#include <cstdlib>
#include <iostream>
#include <QString>
#include <filesystem>
#include <fstream>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#ifdef _WIN32
  #include <windows.h>
#elif __APPLE__
  #include <mach-o/dyld.h>
#else
  #include <unistd.h>
#endif

class PATHS {
  public:
    std::string APPNAME = "VFW";
    std::string HOME;
    std::string execPath;
    bool isPortableMode;

    // root dirs
    std::filesystem::path assetsPath; 
    std::filesystem::path cachePath;
    std::filesystem::path configPath;
    std::filesystem::path tmpPath;

    // branch dirs
    std::string fontsDir;
    std::string themesDir;
    std::string currentThemeDir;
    std::string currentIconsDir;

    std::string ffprobeBinPath;
    std::string ffmpegBinPath;

    std::array<std::string, 8> essentialDirectories;

    PATHS() {
      defineRootDirs();
      defineBranchDirs();
      defineFFmpegBinPaths();
      populateEssentialDirectories();
      createMissingSystemDirs();
    }

  private:
    void defineRootDirs() {
      this->execPath = getExecutablePath();

      #ifdef _WIN32
        std::filesystem::path exeParentPath = std::filesystem::path(execPath).parent_path();
        this->isPortableMode = std::filesystem::exists(exeParentPath / "portable") || std::getenv("portableMode") != nullptr;

        if(isPortableMode) {
          std::cout<< "[ LOG ] launching Into Portable Mode\n";
          this->cachePath = exeParentPath / "cache";
          this->configPath = exeParentPath / "configs";
          this->assetsPath = exeParentPath / "assets";
          return;
        }

        HOME = std::getenv("USERPROFILE");
        const char* localAppDataEnv = std::getenv("LOCALAPPDATA");
        const char* appDataEnv = std::getenv("APPDATA");
        if (localAppDataEnv && appDataEnv) {
          std::filesystem::path localAppData(localAppDataEnv);
          std::filesystem::path appData(appDataEnv);
          
          this->cachePath = localAppData / APPNAME / "cache";
          this->configPath = appData / APPNAME / "configs";
          this->assetsPath = appData / APPNAME / "assets";
        }


      #elif __APPLE__
        HOME = std::getenv("HOME");
        if (!HOME.empty()) {
          std::filesystem::path home(HOME);
          this->cachePath = home / "Library" / APPNAME / "cache";
          this->configPath = home / "Library" / "Application Support" / APPNAME / "configs";
          this->assetsPath = home / "Library" / "Application Support" / APPNAME / "assets";
        }
      #else
        HOME = std::getenv("HOME");
        if (!HOME.empty()) {
          std::filesystem::path home(HOME);
          this->cachePath = home / ".cache" / APPNAME;
          this->configPath = home / ".config" / APPNAME;
          this->assetsPath = home / ".local" / "share" / APPNAME;
        }
      #endif
      this->tmpPath = std::filesystem::temp_directory_path() / std::filesystem::path(APPNAME);
    }

    void defineBranchDirs() {
      fontsDir = (assetsPath / "fonts").string();
      themesDir = (assetsPath / "themes").string();
      
      std::string currentThemeName = getCurrentThemeName();
      
      std::filesystem::path currentTheme(themesDir);
      currentThemeDir = (currentTheme / currentThemeName).string();
      currentIconsDir = (assetsPath / "icons" / currentThemeName).string();
    }

    void defineFFmpegBinPaths() {
      #ifdef _WIN32
        ffprobeBinPath = "C:\\Program Files\\ffmpeg\\bin\\ffprobe.exe";
        ffmpegBinPath  = "C:\\Program Files\\ffmpeg\\bin\\ffmpeg.exe";
      #else
        ffprobeBinPath = "/usr/bin/ffprobe";
        ffmpegBinPath  = "/usr/bin/ffmpeg";
      #endif
    }

    void populateEssentialDirectories() {
      essentialDirectories = {
        configPath.string(),
        assetsPath.string(),
        cachePath.string(),
        tmpPath.string(),
        themesDir,
        fontsDir,
        currentThemeDir,
        currentIconsDir,
      };
    }

    void createMissingSystemDirs() {
      for (const std::string& dirPath : this->essentialDirectories) {
        if (!dirPath.empty()) {
          bool created = std::filesystem::create_directories(dirPath);
          if (created)
            std::cerr << "[ WARNING ] Missing directory created: " << dirPath << "\n";
        }
      }
    }
    
    std::string getCurrentThemeName() {
      std::filesystem::path configFile = configPath / "theme";
      std::ifstream themefile(configFile);
      std::string theme = "light";
      if (themefile) {
        getline(themefile, theme);
      }
      return theme;
    }

    std::string getExecutablePath() {
      std::string executablePath;
      
      const char* appimage_root = std::getenv("VFW_ROOT");
      if (appimage_root != nullptr) {
        return std::string(appimage_root);
      }
      
      #ifdef _WIN32
        wchar_t path[PATH_MAX];
        GetModuleFileNameW(NULL, path, sizeof(path) / sizeof(wchar_t));
        std::wstring ws(path);
        executablePath = std::string(ws.begin(), ws.end());
      #elif __APPLE__
        char path[PATH_MAX];
        uint32_t size = sizeof(path);
        if (_NSGetExecutablePath(path, &size) == 0) {
          executablePath = path;
        }
      #else
        char path[PATH_MAX];
        ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
        if (len != -1) {
          path[len] = '\0';
          executablePath = path;
        }
      #endif
      
      return executablePath;
    }
};


extern std::vector<std::string> supportedMediaFormats;
extern std::vector<std::string> supportedSubtitlesFormats;


