<img
  src="https://github.com/Sqydev/randix/blob/master/showcase_screenshot.png"
  width="450"
  align="left"
/>

<h1 align="center">Randix</h1>

![GitHub top language](https://img.shields.io/github/languages/top/Sqydev/randix)
![Last Commit](https://img.shields.io/github/last-commit/Sqydev/randix)
![Repo size](https://img.shields.io/github/repo-size/Sqydev/randix)
![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/Sqydev/randix)
![Stars](https://img.shields.io/github/stars/Sqydev/randix)
![GitHub forks](https://img.shields.io/github/forks/Sqydev/randix)
![GitHub License](https://img.shields.io/github/license/Sqydev/randix)
![GitHub Release](https://img.shields.io/github/v/release/Sqydev/randix)
![GitHub Tag](https://img.shields.io/github/v/tag/Sqydev/randix)
![GitHub Issues](https://img.shields.io/github/issues/Sqydev/randix)
![GitHub Downloads (all assets, all releases)](https://img.shields.io/github/downloads/Sqydev/randix/total)
![GitHub Downloads (all assets, latest release)](https://img.shields.io/github/downloads/Sqydev/randix/latest/total)


<br clear="left">

<h2 align="left">What is this?</h2>
Randix is basically a random character filler for your terminal. You can think of it like the Matrix effect, but instead of characters falling down the console, they appear all over and "swarm" your terminal.

<h2 align="left">Installation</h2>

<a href="https://repology.org/project/referencing/versions">
    <img src="https://repology.org/badge/vertical-allrepos/randix.svg?columns=2" alt="Packaging status" align="right">
</a>

If your distribution’s package manager is listed on the right, you can install randix directly:

```sh
sudo [YOUR-PACKAGE-MANAGER] [INSTALL-ARG] randix
```

If your distribution is not listed, you can build and install randix manually using `make`.

First, clone the repository:

```sh
git clone https://github.com/Sqydev/randix.git
```

or download a source archive for a specific version from
GitHub Releases.

Then go to the project directory:

```sh
cd path/to/randix
```

Build the project:

```sh
make release
```

Finally, install it:

```sh
sudo make install PREFIX=[Directory where your bin/ is]
```

For 99% of systems it will just be:

```sh
sudo make install PREFIX=/usr
```

<h2 align="left">Usage</h2>

Usage is pretty easy. You just do:
```sh
randix [options]
```

For options explaining just get help"
```sh
randix -h
```

You can also build randix from a specific released version
by downloading the source archive from GitHub Releases
instead of cloning the repository.

<h2 align="left">Building</h2>

Randix uses a custom Makefile with multiple build profiles and libc targets.

Below is a simplified overview of the main build targets:
```sh
# Release build
make release

# Development builds
make local-build
make san-build
make check-build

# Docker-based builds
make docker-[profile]-[libc]
```

And here are all the profiles, libs, and all:
Profiles: local, san, check  
Libc targets: glibc, musl, static-musl
