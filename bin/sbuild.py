import os
import sys
import json
import shutil
import glob
import subprocess
from pathlib import Path

# Utility to run a system command and print it
def run_cmd(cmd):
    print(f"🚧 Running: {cmd}")
    result = subprocess.run(cmd, shell=True)
    return result.returncode

# Load JSON config from file
def load_project_config():
    try:
        with open("project.json", 'r') as file:
            return json.load(file)
    except FileNotFoundError:
        raise RuntimeError("project.json not found.")

# Expand wildcard include paths
def expand_includes(raw_paths):
    expanded = []
    for path in raw_paths:
        if '*' in path:
            # Find base directory before wildcard
            base = path.split('*')[0].rstrip('/')
            # Get all directories in base path
            for entry in glob.glob(os.path.join(base, '*')):
                if os.path.isdir(entry):
                    expanded.append(f"-I{entry}")
        else:
            if os.path.isdir(path):
                expanded.append(f"-I{path}")
            else:
                print(f"⚠️ Warning: include path '{path}' is not a directory. Skipping.")
    return expanded

# Build project
def build(new_version):
    config = load_project_config()
    
    buildpath = config["buildpath"]
    src = config["srcpath"]
    build_type = config["type"]
    includepaths = config.get("includepaths", ["./include/*"])
    version = config["version"]
    
    # Create build directory if needed
    Path(buildpath).parent.mkdir(parents=True, exist_ok=True)
    
    # Base compile command
    output_name = f"{buildpath}-{version}"
    command = f"g++ -o {output_name} {src}"
    
    # Add type-specific flags
    if build_type == "shared":
        command += " -shared -fPIC"
    elif build_type != "elf":
        print(f"❌ invalid type: {build_type}")
        return
    
    # Add expanded include paths
    includes = expand_includes(includepaths)
    command += " " + " ".join(includes)
    
    # Execute build command
    result = run_cmd(command)
    if result != 0:
        print("❌ Build failed.")
    else:
        print(f"✅ Build successful -> {output_name}")
        # Update version on success
        config["version"] = new_version
        with open("project.json", 'w') as out:
            json.dump(config, out, indent=4)
        print(f"🔄 Updated version to: {new_version}")

# Create new project directory
def create_new_project(path):
    os.makedirs(f"{path}/src", exist_ok=True)
    os.makedirs(f"{path}/include", exist_ok=True)
    
    # Create project config
    project_config = {
        "buildpath": "./build/output",
        "includepaths": ["./include/*"],
        "srcpath": "./src/main.cpp",
        "version": "1.0",
        "type": "elf"
    }
    
    with open(f"{path}/project.json", 'w') as proj:
        json.dump(project_config, proj, indent=4)
    
    # Create sample source file
    with open(f"{path}/src/main.cpp", 'w') as maincpp:
        maincpp.write(f'#include <iostream>\nint main() {{\n    std::cout << "Hello from {path}!\\n";\n    return 0;\n}}\n')
    
    print(f"✅ Project created at: {path}")

# Install header files to ./include/
def install_headers(from_path):
    os.makedirs("include", exist_ok=True)
    source = Path(from_path).resolve()
    
    if not source.exists():
        print("❌ Error: source path does not exist.")
        return
    
    # Single file installation
    if source.is_file():
        if source.suffix in (".h", ".hpp"):
            shutil.copy(source, f"include/{source.name}")
            print(f"✅ Installed header: {source.name}")
        else:
            print(f"❌ Not a header file: {source}")
        return
    
    # Directory installation
    for file in source.rglob('*'):
        if file.is_file() and file.suffix in (".h", ".hpp"):
            rel_path = file.relative_to(source)
            dest = Path("include") / rel_path
            dest.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy(file, dest)
    
    print("✅ Headers installed to ./include")

# Main CLI dispatcher
def main():
    if len(sys.argv) < 2:
        print("Usage: mybuild [build|new|install|help] [optional args]")
        return
    
    cmd = sys.argv[1]
    
    try:
        if cmd == "build":
            if len(sys.argv) < 3:
                print("Usage: mybuild build <new_version>")
                return
            build(sys.argv[2])
        elif cmd == "new":
            if len(sys.argv) < 3:
                print("Usage: mybuild new <path>")
                return
            create_new_project(sys.argv[2])
        elif cmd == "install":
            if len(sys.argv) < 3:
                print("Usage: mybuild install <path>")
                return
            install_headers(sys.argv[2])
        elif cmd == "help":
            print("Commands:\n"
                  "  new <path>      - Creates new project\n"
                  "  build <version> - Builds project and updates version\n"
                  "  install <path>  - Installs headers from path\n"
                  "  help            - Shows this message")
        else:
            print(f"❌ Unknown command: {cmd}")
    except Exception as e:
        print(f"❌ Exception: {str(e)}")

if __name__ == "__main__":
    main()