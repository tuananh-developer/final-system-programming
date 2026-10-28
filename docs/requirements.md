# Requirements

## Functional Requirements

- FR-01: Copy a regular file to a new destination.
- FR-02: Copy binary and text file contents correctly.
- FR-03: Copy a directory recursively.
- FR-04: Preserve the directory hierarchy.
- FR-05: Automatically determine whether the source is a file or directory.
- FR-06: Report errors for missing sources and failed I/O operations.
- FR-07: Reject existing destinations instead of overwriting them.
- FR-08: Reject symbolic links and unsupported special files.

## Non-functional Requirements

- NFR-01: The program must be implemented in C.
- NFR-02: The program must run on Linux.
- NFR-03: Use POSIX file and directory APIs.
- NFR-04: Copy data in bounded-size blocks.
- NFR-05: Close file descriptors and directory streams after use.
- NFR-06: Compile with CMake and GCC.
