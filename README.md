# C++ Todo List App

This project is a simple todo list app written in C++.

It was created to practice writing web applications in C++ with TDD (Test Driven Development)

It uses Poco libraries for the web server and SQLite for the database.

## Dependencies

The easy way to see the dependencies is to look at the [.github/workflows/c-cpp.yml]() file.

- [Poco](https://pocoproject.org/) - for the web server
- [SQLite](https://www.sqlite.org/index.html) - for the database
- [spdlog](https://github.com/gabime/spdlog/) - for logging
- [backward-cpp](https://github.com/bombela/backward-cpp) - for stacktraces
- [doctest](https://github.com/doctest/doctest/) for testing

## Building

    mkdir build && cmake .. && make

## Nice features

If an exception happens, the server will log a stacktrace to the console.
