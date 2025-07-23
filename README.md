# 🔨 jmake++ — Simple C++ Build Tool

`jmake++` is a lightweight, zero-dependency build system for C++ projects, written in C++. It replaces complex systems like Make and CMake for small to mid-size projects with a single JSON configuration file and intuitive CLI.

**Version:** `1.7.3`  
**Platforms:** Windows ✅ & Linux ✅  
**Dependencies:** `g++`, `nlohmann/json`, and optionally `filio.h` (bundled or install separately)

---

## ⚡ Features

- 📦 Easy project config via `project.json`
- 🔁 Wildcard-based include path expansion (`./include/*`)
- 🔧 Build targets: Executables (`elf`) and shared libraries (`shared`)
- 📁 Header installer: install `.h`/`.hpp` files into your project
- 🧼 `clean` command to delete build artifacts
- 🆙 Automatic version tracking on successful builds
- ✅ Cross-platform: single `.exe` for Windows; CLI-native on Linux
- 🚀 Installer scripts (`installer.sh` / `installer.ps1`) for PATH setup

---
## patch notes:
- fix a bug where the built project would output a relative path if a relative path was given, now it uses `filio::extra::absolute_path`
## 🚀 Getting Started

### 📥 Installation

**On Linux:**
```bash
./installer.sh
```

**On Windows (PowerShell):**

```powershell
./installer.ps1
```

These scripts copy the `jmake` binary to a directory in your `PATH`.

###### or if you have an older version of jmake++:
```bash
git clone https://github.com/replit-user/jmakepp ./jmakepp
cd jmakepp
jmake build
---
```
however this will only work if the version of jmake++ you have supports the flags key in project.json

## 🔧 Commands

```bash
jmake new <path>      # Create a new project in the given directory
jmake build <version> # Build the project and update version in project.json
jmake install <path>  # Install .h/.hpp files to ./include
jmake clean           # Remove the ./build directory
jmake version         # Show jmake++ version
jmake help            # Show available commands
```

---

## 🛠 Example Workflow

```bash
jmake new myproject
cd myproject
jmake build 1.1.0
./build/example-1.1.0
```

You’ll see:

```bash
🚧 Running: g++ -o ./build/example-1.0 ./src/main.cpp -I./include
✅ Build successful -> ./build/example-1.0
🔄 Updated version to: 1.1.0
```

---

## 📄 project.json Format

```json
{
  "name": "example",
  "version": "1.0",
  "buildpath": "./build/",
  "includepaths": ["./include/*"],
  "srcpath": "./src/main.cpp",
  "type": "elf",
  "flags":""
}
```

### Fields

| Field | Type | Description |
| --- | --- | --- |
| `name` | `string` | Name of the output binary |
| `version` | `string` | Semantic version string |
| `buildpath` | `string` | Output path for compiled binary |
| `includepaths` | `array[string]` | List of include directories (supports `*`) |
| `srcpath` | `string` | Source file(s) (currently only one path) |
| `type` | `"elf"` or `"shared"` | Build target: executable or shared lib |
| `flags`| `string or list of strings`| `additional flags to add to g++`

---

## 📁 Directory Layout (after `jmake++ new`)

```pgsql
myproject/
├── project.json
├── include/
├── src/
│   └── main.cpp
└── build/           # Generated after build
```

---

## 🧰 Installing Headers

Install individual headers or full directories:

```bash
jmake install path/to/header.hpp
jmake install include/
```

Copies valid `.h` or `.hpp` files to your project's `./include/`.

---

## 🧼 Clean Build Artifacts

```bash
jmake clean
```

Removes the build directory completely.

---

## 🧱 Built With

-   [`nlohmann/json`](https://github.com/nlohmann/json) for JSON parsing
    
-   [`filio`](https://github.com/replit-user/filio) for cleaner I/O abstraction (optional)
    
-   C++17 and `<filesystem>` for cross-platform FS operations
    

---

## 📦 Example Build Output

```bash
g++ -o ./build/example-1.0 ./src/main.cpp -I./include
```

For shared libraries:

```bash
g++ -o ./build/libexample-1.0.so ./src/main.cpp -shared -fPIC -I./include
```

---

## 📌 Roadmap

empty for now

---

## 🧑‍💻 Author

Made by [replit-user](https://github.com/replit-user)

---

## 📄 License

MIT License — free to use, modify, and distribute.
