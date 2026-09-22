#pragma once

#ifdef _WIN32
    inline bool is_windows = true;
    inline bool is_macos = false;
#elif __APPLE__
    inline bool is_windows = false;
    inline bool is_macos = true;
#else
    inline bool is_windows = false;
    inline bool is_macos = false;
#endif