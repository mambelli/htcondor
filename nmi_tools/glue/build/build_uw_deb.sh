#!/bin/bash
set -e
if [[ $VERBOSE ]]; then
  set -x
fi

# build_uw_deb

usage () {
  echo "usage: [VERBOSE=1] $(basename "$0")"
  echo
  echo "Environment:"
  echo "  VERBOSE=1                         Show all commands run by script"
  echo
  exit $1
}

fail () { echo "$@" >&2; exit 1; }

top_dir=$PWD
[[ $dest_dir ]] || dest_dir=$PWD

check_version_string () {
  [[ ${!1} =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]] || fail "Bad ${1//_/ }: '${!1}'"
}

# get the version and build id
condor_build_id=$(<BUILD-ID)
condor_version=$(echo condor-*.tgz | sed -e s/^condor-// -e s/.tgz$//)

[[ $condor_version ]] || fail "Condor version string not found"
check_version_string  condor_version

# Do everything in a temp dir that will go away on errors or end of script
tmpd=$(mktemp -d "$PWD/.tmpXXXXXX")
trap 'rm -rf "$tmpd"' EXIT

cd "$tmpd"

# Unpack the official tarball
mv ../condor-${condor_version}.tgz ./condor_${condor_version}.orig.tar.gz
tar xfpz condor_${condor_version}.orig.tar.gz
cd condor-${condor_version}

if $(grep -qi stretch /etc/os-release); then
    dist='stretch'
elif $(grep -qi buster /etc/os-release); then
    dist='buster'
elif $(grep -qi xenial /etc/os-release); then
    dist='xenial'
elif $(grep -qi bionic /etc/os-release); then
    dist='bionic'
else
    dist='unstable'
fi
echo "Distribution is $dist"

# copy srpm files from condor sources into the SOURCES directory
cp -pr build/packaging/new-debian debian

# Nightly build changelog
dch --distribution $dist --newversion "$condor_version-0.$condor_build_id" "Nightly build"

# Final release changelog
#dch --release --distribution $dist ignored

dpkg-buildpackage -uc -us

cd ..

mv *.changes *.dsc *.debian.tar.xz *.orig.tar.gz *.deb "$dest_dir"
ls -lh "$dest_dir"

