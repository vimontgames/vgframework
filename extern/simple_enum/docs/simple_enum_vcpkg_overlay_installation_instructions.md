
# Installation Instructions for simple_enum with vcpkg Overlay Ports

This guide provides instructions on how to install the `simple_enum` library using vcpkg with an overlay port directly from a repository. Follow these steps to integrate `simple_enum` into your C++ project.

## Prerequisites

- Ensure you have Git installed on your system to clone repositories.
- Make sure you have vcpkg installed. If not, refer to the [official vcpkg documentation](https://github.com/microsoft/vcpkg) for installation instructions.

## Step 1: Clone the Repository

First, clone the repository containing the `simple_enum` overlay port.

cloning master
```bash
git clone https://github.com/arturbac/simple_enum.git path/to/your/local/overlay-ports-directory
```

cloning specific version ex v.5.7

```bash
git clone --branch v0.5.7 --single-branch https://github.com/arturbac/simple_enum.git path/to/your/local/overlay-ports-directory
```
 or downloading and unpacking

```bash
wget https://github.com/arturbac/simple_enum/archive/refs/tags/v0.5.7.tar.gz -O simple_enum_v0.5.7.tar.gz
tar -xzvf simple_enum_v0.5.7.tar.gz -C path/to/your/local/overlay-ports-directory
```

## Step 2: Install simple_enum Using vcpkg

With the overlay port for `simple_enum` cloned to your local machine, you can now install the library using vcpkg. Use the `--overlay-ports` option to specify the path to your overlay ports directory.

```bash
vcpkg install simple_enum --overlay-ports=path/to/your/local/overlay-ports-directory
```

## Step 3: Integrate simple_enum into Your Project

After installation, use `simple_enum` in your project by including the header files in your source code. Ensure your project's build system is configured to use the vcpkg toolchain file for dependency resolution.

## Conclusion

For further details on using vcpkg in your projects, refer to the [vcpkg documentation](https://github.com/microsoft/vcpkg).

