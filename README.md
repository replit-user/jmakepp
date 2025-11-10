# 🔨 jmake++ — Simple C++ Build Tool

`jmake++` is a lightweight, zero-dependency build system for C++ projects, written in C++. It replaces complex systems like Make and CMake for small to mid-size projects with a single JSON configuration file and intuitive CLI.

**Version:** `1.8.5`
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
double click the .deb file


**On windows**
double click the .MSI file

THE INSTALLER SCRIPTS FROM NOW ON WILL BE OUTDATED PLEASE NOTE THIS BUT THEY SHOULD STILL WORK JUST WILL NO LONGER RECIVE UPDATES

###### or if you have an older version of jmake++:
```bash
git clone https://github.com/replit-user/jmakepp ./jmakepp
cd jmakepp
jmakepp build
---
```
however this will only work if the version of jmake++ you have supports the flags key in project.json most newer versions should

## 🔧 Commands

```bash
jmakepp new <path>      # Create a new project in the given directory
jmakepp build <version> # Build the project and update version in project.json
jmakepp install <path>  # Install .h/.hpp files to ./include
jmakepp clean           # Remove the ./build directory
jmakepp version         # Show jmake++ version
jmakepp help            # Show available commands
jmakepp update          # update the script but for some reason doesn't work
```

---

## 🛠 Example Workflow

```bash
jmakepp new myproject
cd myproject
jmakepp build 1.1.0
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
  "flags":"",
  "platforms":[
    "windows",
    "linux"
  ]
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
|`platforms`|`list of strings, either windows or linux`|`the platofrms to compile with`|
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

Responsible Sharing License do most things freely and give credit if republishing


#### patch notes

- 1.8.5
  - fixed a bug where it would always include a certain library even if that library wasn't included in config 