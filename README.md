# mkdebcontrol

Emits a Debian control file (DEBIAN/control) on stdout from a template
control file, command-line options and a staging directory containing
software to be packaged.  Its main purpose is the automatic generation
of dependencies when creating a Debian software package.

## Build
flex and bison are required.  If you don't have them:
```
apt install flex
apt install bison
```
Then it's just:
```
./configure
gmake package
```

## Install
```gmake package``` will create a Debian package which can be install with ```dpkg```.  For example,
```
dpkg -i mkdebcontrol\_1.0.0\_amd64.deb
```

### Usage
See the manpage for usage.
