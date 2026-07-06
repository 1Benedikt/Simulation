#!/usr/bin/env bash
set -euo pipefail

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
INET_DIR="${INET_DIR:-/Users/bent/Desktop/Praktikum_Abgabe/Source/inet4.5}"
OMNETPP_DIR="${OMNETPP_DIR:-/Users/bent/Desktop/omnett/omnetpp-6.3.0}"
CONFIG="${CONFIG:-BaselineStatic}"
INI_FILE="${INI_FILE:-simulations/omnetpp.ini}"
MODE="${MODE:-auto}"
PROJECT_NAME="sdnproject"

usage() {
    cat <<EOF
Usage:
  ./sdn.sh build [debug|release]
  ./sdn.sh run [ConfigName] [debug|release]
  ./sdn.sh run-all [debug|release]
  ./sdn.sh clean

Environment variables:
  INET_DIR=$INET_DIR
  OMNETPP_DIR=$OMNETPP_DIR
  CONFIG=BaselineStatic
  MODE=debug|release|auto
  INI_FILE=simulations/omnetpp.ini

Examples:
  ./sdn.sh build debug
  ./sdn.sh run BaselineStatic debug
  ./sdn.sh run Dynamic debug
  ./sdn.sh run-all debug
EOF
}

source_omnetpp() {
    if command -v opp_makemake >/dev/null 2>&1 && command -v opp_run >/dev/null 2>&1; then
        return
    fi

    if [ -f "$OMNETPP_DIR/setenv" ]; then
        # shellcheck disable=SC1090
        source "$OMNETPP_DIR/setenv" >/dev/null
    fi

    if ! command -v opp_makemake >/dev/null 2>&1; then
        echo "ERROR: opp_makemake not found. Set OMNETPP_DIR or source OMNeT++ setenv first." >&2
        exit 1
    fi
}

lib_ext() {
    case "$(uname -s)" in
        Darwin) echo "dylib" ;;
        *) echo "so" ;;
    esac
}

# opp_makemake-generated Makefiles use $(file ...), added in GNU Make 4.0.
# macOS ships GNU Make 3.81 (last GPLv2 release) as /usr/bin/make, which
# silently no-ops that call, so the generated .last-copts prerequisite is
# never created and every .o target fails with "No rule to make target".
# Prefer a modern gmake (e.g. `brew install make`) when present.
MAKE_BIN="make"
if [ "$(uname -s)" = "Darwin" ] && command -v gmake >/dev/null 2>&1; then
    MAKE_BIN="gmake"
fi

detect_jobs() {
    if command -v nproc >/dev/null 2>&1; then
        nproc
    elif command -v sysctl >/dev/null 2>&1; then
        sysctl -n hw.ncpu
    else
        echo 2
    fi
}

detect_mode() {
    local requested="${1:-$MODE}"
    if [ "$requested" = "debug" ] || [ "$requested" = "release" ]; then
        echo "$requested"
        return
    fi

    local ext
    ext="$(lib_ext)"
    if find "$INET_DIR" -path "*/out/*debug*/src/libINET_dbg.${ext}" -o -path "*/src/libINET_dbg.${ext}" | head -n 1 | grep -q .; then
        echo "debug"
    elif find "$INET_DIR" -path "*/out/*release*/src/libINET.${ext}" -o -path "*/src/libINET.${ext}" | head -n 1 | grep -q .; then
        echo "release"
    else
        echo "release"
    fi
}

find_inet_lib_dir() {
    local mode="$1"
    local ext
    ext="$(lib_ext)"
    local libname="libINET.${ext}"
    [ "$mode" = "debug" ] && libname="libINET_dbg.${ext}"

    local found
    found="$(find "$INET_DIR" -path "*/out/*${mode}*/src/${libname}" -o -path "*/src/${libname}" | head -n 1 || true)"
    if [ -z "$found" ]; then
        echo "ERROR: Could not find $libname under $INET_DIR." >&2
        echo "Build INET first:" >&2
        echo "  cd $INET_DIR && source $OMNETPP_DIR/setenv && make makefiles && make MODE=$mode -j\$(nproc)" >&2
        exit 1
    fi

    dirname "$found"
}

find_inet_lib_file() {
    local mode="$1"
    local lib_dir="$2"
    local ext
    ext="$(lib_ext)"
    if [ "$mode" = "debug" ]; then
        echo "$lib_dir/libINET_dbg.${ext}"
    else
        echo "$lib_dir/libINET.${ext}"
    fi
}

ensure_inet_generated_headers() {
    if [ ! -f "$INET_DIR/src/inet/opp_defines.h" ]; then
        echo "ERROR: $INET_DIR/src/inet/opp_defines.h is missing." >&2
        echo "Run:" >&2
        echo "  cd $INET_DIR && source $OMNETPP_DIR/setenv && make makefiles && make MODE=$(detect_mode) -j\$(nproc)" >&2
        exit 1
    fi
}

build_project() {
    local mode="$1"
    local lib_dir
    lib_dir="$(find_inet_lib_dir "$mode")"
    ensure_inet_generated_headers

    # Generate the Makefile inside src/ (not the project root) so that
    # PROJECTRELATIVE_PATH is non-empty. opp_makemake leaves it blank when run
    # from the project root with --deep, which produces a Makefile with
    # doubled slashes in object paths that GNU Make (3.81, as shipped on
    # macOS) fails to match against its own pattern rules ("No rule to make
    # target"). Building from src/ also matches this project's own
    # .oppbuildspec (used by the OMNeT++ IDE), so the executable ends up in
    # the same place (src/sdnproject[_dbg]) whether built via this script or
    # via the IDE.
    cd "$PROJECT_DIR/src"

    opp_makemake -f --deep -O out \
        -o "$PROJECT_NAME" \
        -DINET_IMPORT \
        -I"$INET_DIR/src" \
        -L"$lib_dir" \
        '-lINET$(D)'

    "$MAKE_BIN" MODE="$mode" -j"$(detect_jobs)"
}

find_executable() {
    local mode="$1"
    local name="$PROJECT_NAME"
    [ "$mode" = "debug" ] && name="${PROJECT_NAME}_dbg"

    if [ -x "$PROJECT_DIR/src/$name" ]; then
        echo "$PROJECT_DIR/src/$name"
        return
    fi

    local found
    found="$(find "$PROJECT_DIR" -type f -name "$name" -perm -111 | head -n 1 || true)"
    if [ -z "$found" ]; then
        echo "ERROR: Could not find executable $name. Run ./sdn.sh build $mode first." >&2
        exit 1
    fi
    echo "$found"
}

run_project() {
    local config="$1"
    local mode="$2"
    local lib_dir lib_file exe

    lib_dir="$(find_inet_lib_dir "$mode")"
    lib_file="$(find_inet_lib_file "$mode" "$lib_dir")"
    exe="$(find_executable "$mode")"

    cd "$PROJECT_DIR"
    "$exe" \
        -l "$lib_file" \
        -n ".:src:simulations:$INET_DIR/src" \
        "$INI_FILE" \
        -c "$config"
}

run_all() {
    local mode="$1"
    local configs=(
        BaselineStatic
        Dynamic
        LowLoad
        HighLoad
        HotspotTraffic
        LinkFailure
    )

    for config in "${configs[@]}"; do
        echo "Running $config"
        run_project "$config" "$mode"
    done
}

clean_project() {
    cd "$PROJECT_DIR/src"
    if [ -f Makefile ]; then
        "$MAKE_BIN" clean || true
    fi
    rm -f "$PROJECT_NAME" "${PROJECT_NAME}_dbg"
    rm -rf "$PROJECT_DIR/out"
}

main() {
    local command="${1:-}"
    case "$command" in
        build)
            source_omnetpp
            build_project "$(detect_mode "${2:-$MODE}")"
            ;;
        run)
            source_omnetpp
            local config="${2:-$CONFIG}"
            local mode
            mode="$(detect_mode "${3:-$MODE}")"
            if [ ! -x "$(find_executable "$mode" 2>/dev/null || true)" ]; then
                build_project "$mode"
            fi
            run_project "$config" "$mode"
            ;;
        run-all)
            source_omnetpp
            local mode
            mode="$(detect_mode "${2:-$MODE}")"
            if [ ! -x "$(find_executable "$mode" 2>/dev/null || true)" ]; then
                build_project "$mode"
            fi
            run_all "$mode"
            ;;
        clean)
            clean_project
            ;;
        ""|-h|--help|help)
            usage
            ;;
        *)
            echo "ERROR: Unknown command '$command'" >&2
            usage
            exit 1
            ;;
    esac
}

main "$@"
