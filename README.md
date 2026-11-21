# zrnutils

**zrnutils** is a collection of development tools, libraries, APIs, and example projects for creating applications for **ZareboonOS**.

The repository contains everything needed to develop, build, and experiment with ZareboonOS userspace applications.

## Repository Structure

```text
zrn/
├── api/          # Application APIs and libraries
├── projects/     # Applications and example projects
├── target/       # Build output and target files
├── zake          # Build system
└── zrn.ld        # Linker script
```

## API

The `api/` directory contains the APIs available to ZareboonOS applications.

Each API version has its own headers, source files, and compiled library.

Example:

```text
api/
└── api_1/
    ├── inc/          # API headers
    ├── src/          # API implementation
    ├── libapi.a      # Static library
    ├── README.md     # API documentation
    └── recompiler.sh # Library rebuild script
```

## Build System

zrnutils uses **zake**, its build tool, to manage project compilation.

The repository also includes `zrn.ld`, a linker script used when linking ZareboonOS applications.

## Getting Started

1. Clone the repository.
2. Make sure the required compiler and build tools are installed.
3. Explore the `api/` directory to learn about the available system APIs.
4. Check individual project directories for examples.
5. Use `zake` to build projects.

## Purpose

zrnutils aims to provide a simple and consistent development environment for ZareboonOS.

Rather than relying on a large external toolchain or general-purpose application frameworks, it provides a dedicated set of tools and libraries for the ZareboonOS ecosystem.

---

**Part of the ZareboonOS project.**
