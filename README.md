# 🔨 jmake++ — Simple C++ Build Tool

`jmake++` is a lightweight, zero-dependency build system for C++ projects, written in C++. It replaces complex systems like Make and CMake for small to mid-size projects with a single JSON configuration file and intuitive CLI.

**Version:** `2.0.4`
**Platforms:** Windows ✅ & Linux ✅  & MacOS ⚠️(needs to build from source, not tested)
**Dependencies:** `g++`, `nlohmann/json` (bundled), `filio` (bundled), and C++17 standard library

---

## ⚡ Features

- 📦 Easy project config via `project.json`
- 🔁 Wildcard-based include path expansion (`./include/*`)
- 🔧 Build targets: Executables (`elf`) and shared libraries (`shared`)
- 📁 Header installer: install `.h`/`.hpp` files into your project
- 🧼 `clean` command to delete build artifacts
- 🆙 Automatic version tracking on successful builds
- ✅ Cross-platform: single `.exe` for Windows; CLI-native on Linux
- 🚀 Installers(`jmakepp_<version>_amd64.deb` and `jmakepp_<version>_amd64.msi`) in the installers directory

---

## 🚀 Getting Started

### 📥 Installation

Download the prebuilt binary for your platform from the [repo](https://github.com/replit-user/jmakepp/bin) page, or build from source:

```bash
git clone https://github.com/replit-user/jmakepp ./jmakepp
cd jmakepp
./jmakepp_linux build 2.0.4  # On Linux
# or
jmakepp_windows.exe build 2.0.4  # On Windows
```

Add the binary to your PATH for system-wide access using the provided .deb and .msi packages

## 🔧 Commands

```bash
jmakepp new <path>      # Create a new project in the given directory
jmakepp build <version> # Build the project and update version in project.json
jmakepp install <path>  # Install .h/.hpp files to ./include
jmakepp clean           # Remove the ./build directory
jmakepp version         # Show jmake++ version
jmakepp help            # Show available commands
jmakepp update          # update the script
jmakepp run             # build the program and run the binary
```

---

## 🛠 Example Workflow

```bash
jmakepp new myproject
cd myproject
jmakepp build 1.1.0
./build/myproject-1.1.0
```

You'll see:

```bash
📦 Starting compilation
✅ Build successful -> ./build/myproject-1.1.0
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
  "flags":"",
  "platforms":[
    "windows",
    "linux"
  ],
  "c":false,
  "override binary name":false,
  "binary name":""
}
```

### Fields

| Field | Type | Description |
| --- | --- | --- |
| `name` | `string` | Name of the output binary |
| `version` | `string` | Semantic version string |
| `buildpath` | `string` | Output path for compiled binary |
| `includepaths` | `array[string]` | List of include directories (supports `*`) |
| `srcpath` | `array[string]` | Source file(s) |
| `type` | `"elf"` or `"shared"` | Build target: executable or shared lib |
| `flags`| `string or list of strings`| `additional flags to add to g++`
|`platforms`|`list of strings, either windows or linux`|`the platofrms to compile with`|
|`override binary name`|`boolean`|`wheather or not to override the default binary naming`|
|`binary name`|`string`|`if obverriding the binary name, tha binary name to use`

---

## 📁 Directory Layout (after `jmakepp new`)

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
jmakepp install path/to/header.hpp
jmakepp install include/
```

Copies valid `.h` or `.hpp` files to your project's `./include/`.

---

## 🧼 Clean Build Artifacts

```bash
jmakepp clean
```

Removes the build directory completely.

---

## 🧱 Built With

-   [`nlohmann/json`](https://github.com/nlohmann/json) for JSON parsing
    
-   [`filio`](https://github.com/replit-user/filio) for cleaner I/O abstraction (optional)
    
-   C++17 and `<filesystem>` for cross-platform FS operations

-   jmakepp itself!
    

---

## 📦 Example Build Output

```bash
g++ -std=c++17 -O2 -o ./build/myproject-1.0 ./src/main.cpp -I./include
```

For shared libraries:

```bash
g++ -std=c++17 -O2 -o ./build/libmyproject-1.0.so ./src/main.cpp -shared -fPIC -I./include
```

---

## 📌 Latest Changes (v2.0.4)

fixed bugs
  - fixed a permission issue after installing
  - added a build.sh to easily build the project from source as a utility tool

## ❗ Known Limitations

- macOS support is not currently maintained (C) support was added but is not actively tested
- Only tested on Linux (x86_64) and windows x86_64

---

## 🧑‍💻 Author

Made by [replit-user](https://github.com/replit-user)

---

## 📄 License

Responsible Sharing License do most things freely and give credit if republishing


## 🔖 Version History
- **2.0.5** - refactor code
- **2.0.4** - minor bigfixes
- **2.0.3** - fix a bug where the old binary wouldn't get removed
- **2.0.2** - add run command
- **2.0.1** - a small bugfix
- **2.0.0** - added multithreading and multiple soirce files in srcpath
- **1.9.0** - added a binary name override option in project.json and remove installer.ps1/installer.sh
- **1.8.8** - Previous release
- **1.8.7** - Fixed update mechanism
- **1.8.6** - Added C language support and experimental macOS support
