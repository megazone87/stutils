#!/bin/sh

add=0
while true; do
  case $1 in
    -x) shift
        case $1 in
          *[!0-9]* | "") echo "$0: major version not specified" >&2;
                         exit 2 ;;
                      *) major=$1; shift ;;
        esac ;;
    -y) shift
        case $1 in
          *[!0-9]* | "") echo "$0: minor version not specified" >&2;
                         exit 2 ;;
                      *) minor=$1; shift ;;
        esac ;;
    -z) shift
        case $1 in
          *[!0-9]* | "") echo "$0: patch version not specified" >&2;
                         exit 2 ;;
                      *) patch=$1; shift ;;
        esac ;;
    +x) if [ "$add" -ne 0 ]; then
          echo "$0: can only specify one of +x, +y or +z" >&2
          exit 3
        fi
        add=1; shift ;;
    +y) if [ "$add" -ne 0 ]; then
          echo "$0: can only specify one of +x, +y or +z" >&2
          exit 3
        fi
        add=2; shift ;;
    +z) if [ "$add" -ne 0 ]; then
          echo "$0: can only specify one of +x, +y or +z" >&2
          exit 3
        fi
        add=3; shift ;;
    -s) add=-1; break ;;
    -h) echo "Usage: $0 [-x major] [-y minor] [-z patch] [+x|+y|+z] [-s | -h ]" >&2
        exit 0;;
    -*) echo "$0: Unrecognized option $1" >&2
        exit 1;;
    *) break ;;
  esac
done

if [ "$add" -eq 0 ]; then # default to add patch
  add=3
fi

cur_version=`grep AC_INIT configure.ac | awk -F',' '{print $2}'`
cur_major=`echo $cur_version | awk -F'.' '{print $1}'`
cur_minor=`echo $cur_version | awk -F'.' '{print $2}'`
cur_patch=`echo $cur_version | awk -F'.' '{print $3}'`

echo "Current version: $cur_major.$cur_minor.$cur_patch"

case $add in
 -1) exit 0 ;;
  1) cur_major=$(($cur_major + 1)); cur_minor=0; cur_patch=0 ;;
  2) cur_minor=$(($cur_minor + 1)); cur_patch=0 ;;
  3) cur_patch=$(($cur_patch + 1)) ;;
esac

major=${major:-$cur_major}
minor=${minor:-$cur_minor}
patch=${patch:-$cur_patch}

echo "Releasing version: $major.$minor.$patch..."

echo "Updating configure..."
awk -v major=$major -v minor=$minor -v patch=$patch \
    '{if ($1 ~ /^AC_INIT/) { \
        $2 = major"."minor"."patch","; \
      } \
      print $0; \
     }' configure.ac > /tmp/configure.ac.$$ || exit 1
mv /tmp/configure.ac.$$ configure.ac
(
set -x
autoreconf || exit 1
./configure > /dev/null|| exit 1
) || exit 1

echo "Updating header..."
awk -v major=$major -v minor=$minor -v patch=$patch \
    '{if ($1 == "#define") {\
        if ($2 == "ST_VERSION_MAJOR") { \
          $3 = "\""major"\""; \
        } else if ($2 == "ST_VERSION_MINOR") { \
          $3 = "\""minor"\""; \
        } else if ($2 == "ST_VERSION_PATCH") { \
          $3 = "\""patch"\""; \
        } \
      } \
      print $0; \
     }' src/st_macro.h > /tmp/st_macro.h.$$
mv /tmp/st_macro.h.$$ src/st_macro.h
(
set -x
make > /dev/null || exit 1
make check > /dev/null || exit 1
) || exit 1

echo "Updating README..."
awk -v major=$major -v minor=$minor -v patch=$patch \
    '{if ($1 == "#") {\
        if ($2 == "stutils") { \
          $3 = "(v"major"."minor"."patch")"; \
        } \
      } else if ($1 == "$") { \
        gsub(/stutils-[0-9]+\.[0-9]+\.[0-9]+/, \
             "stutils-"major"."minor"."patch); \
        gsub(/releases\/download\/v[0-9]+.[0-9]+.[0-9]+\//, \
             "releases/download/v"major"."minor"."patch"/");
      } \
      print $0; \
     }' README.md > /tmp/README.md.$$
mv /tmp/README.md.$$ README.md
(
set -x
make dist > /dev/null || exit 1
make distcheck > /dev/null || exit 1
) || exit 1

echo "Tagging repo..."
git commit -am"version bump" || exit 1
git tag "v$major.$minor.$patch" || exit 1
git push || exit 1
git push --tags || exit 1

echo "Releasing on github..."
github-release release \
        --user wantee \
        --repo stutils \
        --tag v$major.$minor.$patch || exit 1

github-release upload \
        --user wantee \
        --repo stutils \
        --tag v$major.$minor.$patch \
        --name "stutils-$major.$minor.$patch.tar.gz" \
        --label "Ready-to-install tarball" \
        --file "stutils-$major.$minor.$patch.tar.gz" || exit 1

echo "Finish releasing."
