# Contributor Guidelines

## Advice for new contributors

Start small. The PRs most likely to be merged are the ones that make small,
easily reviewed changes with clear and specific intentions. See below for more
[guidelines on pull requests](#pull-requests).

It's a good idea to gauge interest in your intended work by finding the current issue
for it or creating a new one yourself. You can use also that issue as a place to signal
your intentions and get feedback from the users most likely to appreciate your changes.

You're most likely to have your pull request accepted easily if it addresses bugs already
in the [Next Steps project](https://github.com/despair86/loki-pager/projects/1),
especially if they are near the top of the Backlog column. Those are what we'll be looking
at next, so it would be great if you helped us out!

Once you've spent a little bit of time planning your solution, it's a good idea to go
back to the issue and talk about your approach. We'd be happy to provide feedback. [An
ounce of prevention, as they say!](https://www.goodreads.com/quotes/247269-an-ounce-of-prevention-is-worth-a-pound-of-cure)

## Developer Setup

First, you'll need a C89 compiler. We make use of some C99 features, but should be optional soon enough. 
We've tested the following C compilation systems:

- Microsoft C v15.00.30729+
- GNU CC v8.3+
- Apple LLVM C v8.0.1
- Oracle ProWorks (the x86 compiler) 12.1 or later
- Oracle SPARCWorks 12.1+

While these are the compilers currently used, pretty much any conforming C89/99 implementation should work,
as we make no use of any vendor specific extensions, only platform-specific extensions when warranted.

Then you need `git`, if you don't have that yet: https://git-scm.com/

### macOS

1.  Install the [Xcode Command-Line Tools](http://osxdaily.com/2014/02/12/install-command-line-tools-mac-os-x/).

### Windows

1.  **Windows 7 only:**
    * Install Microsoft .NET Framework 3.5:
      (See Programs and Features/Optional Features)
    * Install Windows SDK version 7.0: https://developer.microsoft.com/en-us/windows/downloads/sdk-archive
2.  Install CMake from https://cmake.org/. 32-bit build is recommended even in 64-bit installs if core memory is at a premium on your site.
3.  Install cURL from https://curl.haxx.se, we use it only to download the Netscape Navigator root cert bundle, manually grab from: https://curl.haxx.se/ca/cacert.pem and save as `rootcerts.pem`

If you are developing on an older version of Windows, you should probably get the last version of Microsoft C available for your system, plus the Windows Server 2003 R2 SDK.

### Linux

1.  Pick your favorite package manager.
1.  Install `gcc`
1.  Install `g++`
1.  Install `make`
1.  Install `cmake`
1.  Install `curl`

### UCB UNIX (OpenBSD, NetBSD, FreeBSD, etc)

The C compilation system should already be available (try executing `cc`), optionally install Ninja, and make sure CMake and cURRL are available.

### Solaris 2.x

Install Oracle Workshop from https://www.oracle.com/technetwork/server-storage/developerstudio/overview/index.html, or
install CMake and GCC or Clang from IPS. For SVR4 packages, the [UNC SunSITE](http://ibiblio.org) may have GCC or Clang packages for v2.5-2.10 available.

## Development host setup (All platforms)

### Build dependencies

* [CMake](https://cmake.org/) 2.8.4 or higher
* [LCOV *2](http://ltp.sourceforge.net/coverage/lcov.php)

Most of these dependencies are required just for the unit test suite and
development of the library itself. When integrating into actual applications,
you should not need anything beyond CMake. Alternatively, you may integrate
the code using a build system of your choice.
Items marked with *1 are required for tests, with *2 are additionally required for code coverage.

```
git clone https://github.com/despair86/loki-pager.git
cd loki-pager
```

### Setting up a fresh source tree

    $ mkdir build
    $ cd build
    $ cmake -DCMAKE_BUILD_TYPE=Debug ..
    $ make

### Running the unit tests

    $ cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=1 ..
    $ cd tests
    $ make
    $ cd ..
    $ ctest

### Creating the code coverage report

    $ cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=1 -DCOVERAGE=1 ..
    $ make coverage

The generated code coverage report can be found in:
`/path/to/libsignal-protocol-c/build/coverage`

### Eclipse project setup

CMake provides a tutorial on Eclipse project setup here:
https://cmake.org/Wiki/CMake:Eclipse_UNIX_Tutorial

It is recommended to follow the more manual "Option 2," since the Eclipse
project generator built into CMake tends to be outdated and leads you toward
a very awkward and occasionally broken project configuration.

### Protocol Buffers compiler

This project uses serialization code based on [Protocol Buffers](https://github.com/google/protobuf).
Since the official library does not support C, the [protobuf-c](https://github.com/protobuf-c/protobuf-c)
generator is used instead. For the sake of convenience, the generated code and its dependencies are
included in the source tree. The generated code can be regenerated at any time by installing the two
mentioned packages and running "make" in the "protobuf/" subdirectory.

## Target platforms

CMake toolchain files have been included from the following sources:

* [iOS](https://code.google.com/archive/p/ios-cmake)
* [BlackBerry 10](https://github.com/blackberry/OGRE/blob/master/src/CMake/toolchain/blackberry.toolchain.cmake)

## Additional storage profiles

Since there is no registration for Loki Messenger, you can create as many accounts as you
can public keys. To test the P2P functionality on the same machine, however, requries
that each client binds their message server to a different port.

You can use the following command to start a client bound to a different port.

```
pager --bind-port=[port]
```
# Making changes

So you're in the process of preparing that pull request. Here's how to make that go
smoothly.

## Tests

Please write tests! Our testing framework is
[Check](https://libcheck.github.io/check).

The easiest way to run all tests at once is `make test`. Additionally, you can handwrite unit tests, by
making them return 0 for success, and anything else if it failed.

## Pull requests

So you wanna make a pull request? Please observe the following guidelines.

<!-- TODO:
* Please do not submit pull requests for translation fixes. Anyone can update
  the translations in
  [Transifex](https://www.transifex.com/projects/p/signal-desktop).
-->

* Never use plain strings right in the source code - pull them from `messages.json`!
  You **only** need to modify the default locale
  [`_locales/en/messages.json`](_locales/en/messages.json).
  * We should probably use `catgets` or `gettext`, since we're not shackled by the limitations of
    a JS interpreter environment.
  <!-- TODO:
    Other locales are generated automatically based on that file and then periodically
    uploaded to Transifex for translation. -->
* [Rebase](https://nathanleclaire.com/blog/2014/09/14/dont-be-scared-of-git-rebase/) your
  changes on the latest `development` branch, resolving any conflicts.
  This ensures that your changes will merge cleanly when you open your PR.
* Be sure to add and run tests!
* Make sure the diff between our master and your branch contains only the
  minimal set of changes needed to implement your feature or bugfix. This will
  make it easier for the person reviewing your code to approve the changes.
  Please do not submit a PR with commented out code or unfinished features.
* Avoid meaningless or too-granular commits. If your branch contains commits like
  the lines of "Oops, reverted this change" or "Just experimenting, will
  delete this later", please [squash or rebase those changes away](https://robots.thoughtbot.com/git-interactive-rebase-squash-amend-rewriting-history).
* Don't have too few commits. If you have a complicated or long lived feature
  branch, it may make sense to break the changes up into logical atomic chunks
  to aid in the review process.
* Provide a well written and nicely formatted commit message. See [this
  link](http://chris.beams.io/posts/git-commit/)
  for some tips on formatting. As far as content, try to include in your
  summary
  1.  What you changed
  2.  Why this change was made (including git issue # if appropriate)
  3.  Any relevant technical details or motivations for your implementation
      choices that may be helpful to someone reviewing or auditing the commit
      history in the future. When in doubt, err on the side of a longer
      commit message.

Above all, spend some time with the repository. Follow the pull request template added to
your pull request description automatically. Take a look at recent approved pull requests,
see how they did things.

## Testing Production Builds

To test changes to the build system, build a release using `make` or `ninja`, which should kick off CMake if the build system changed.

Then, run the tests using the `test` target depending on your make app (`[make|ninja] test`)

<!-- TODO:
## Translations

To pull the latest translations, follow these steps:

1.  Download Transifex client:
    https://docs.transifex.com/client/installing-the-client
2.  Create Transifex account: https://transifex.com
3.  Generate API token: https://www.transifex.com/user/settings/api/
4.  Create `~/.transifexrc` configuration:
    https://docs.transifex.com/client/client-configuration#-transifexrc
5.  Run `yarn grunt tx`. -->
