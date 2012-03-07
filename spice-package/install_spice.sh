#!/bin/bash

# Script to install qemu spice.

txtred=$(tput setaf 1)    # Red
txtgrn=$(tput setaf 2)    # Green
txtylw=$(tput setaf 3)    # Yellow
txtblu=$(tput setaf 4)    # Blue
txtmag=$(tput setaf 5)    # Magenta
txtcyn=$(tput setaf 6)    # Cyan
txtrst=$(tput sgr0)       # Text reset

# This function echoes coloured messages.
echo_coloured()
{
    local _loop="1"
    local _options=""
    local _colour="${1}"
    shift 1

    until [ -z "${_loop}" ] ; do
  case "${1}" in
    '-n'|'-e'|'-E'|'--help'|'--version')
      _options="${1} $_options"
      shift 1
      ;;
    *)
      _loop=
      ;;
  esac
    done

    echo ${_options} "${_colour}${*}${txtrst}"
}

# This function echoes success messages.
echo_success()
{
    echo_coloured ${txtgrn} ${*}
}

# This function echoes debug messages.
echo_debug()
{
    echo_coloured ${txtcyn} ${*}
}

# This function echoes info messages.
echo_info()
{
    echo_coloured ${txtylw} ${*}
}

# This function echoes error messages.
echo_error()
{
    echo_coloured ${txtred} ${*}
}

LOG_DIR=`pwd`/"build_log"

# This function shows script usage.
usage()
{
    echo_error "Usage:"
    echo_error "sudo bash install_spice.sh -f <packages_list_file> -d <source directory> | -h"
    echo_error "-d <src directory path> :Source directory path."
    echo_error "-f <packages_list_file> :Packages list file."
    echo_error "-t <tmp directory path> :Temporary directory to use"
    echo_error "-h                      :Print this help message."
    echo ""
    echo_error "*Note: Run this script with admin privileges."
}

# Get system core count.
get_cpu_count()
{
    cpu_count=`grep 'processor' /proc/cpuinfo | wc -l`
    cpu_count=$(( $cpu_count + 1 ))
    echo $cpu_count
    return 0
}

# Runs command
# param 1: Package name 
# param 2: Log file name
# param 3: State name (dependency, configure, install)
# param 4: Command to run if any.
run_command()
{

    [ -z "${3}" ] && { \
        echo_error 'Please provide package installation state' ; \
        exit 3 ; }

    case "${3}" in
        'dependency')
            begin_message="Installing dependency for ${1}."
            error_message="Unable to install dependency of ${1}."
            exit_val=5
            ;;
        'configure')
            begin_message="Configuring ${1}."
            error_message="Unable to configure ${1}."
            exit_val=6
            ;;
        'install')
            begin_message="Installing ${1}."
            error_message="Unable to install ${1}."
            exit_val=7
            ;;
        *)
            echo_error "Unknown installation state."
            exit 3
            ;; 
    esac
    
    if [ ! -z "${4}" ] 
    then
        echo_info "${begin_message}" 
        eval "${4}" &>"${2}" || { echo_error "${error_message}" ; \
            echo_error "For errors look in ${2}." ; exit ${exit_val} ; }
        echo_success "Done."
    fi
}

# This function install package build dependency.
# param 1: line from package list file.
install_package_build_dependency()
{
    [ -z "${1}" ] && { echo_error "package info is not given." ; exit 3 ; }
    
    dependency_list=`echo "${1}" | awk -F '|' '{print $3}'`  
    pkg_name=`echo "${1}" | awk -F '|' '{print $1}'`
    log_file="${LOG_DIR}/${pkg_name}_dep.log"
    
    if [ ! -z "${dependency_list}" ] 
    then
        command="apt-get --force-yes -y install ${dependency_list}"
        run_command "${pkg_name}" "${log_file}" "dependency" "${command}"
    fi
}

# This function fires the configure command if specified in package info line
# param 1: line from package list file.
configure_package()
{
    [ -z "${1}" ] && { echo_error "package info is not given." ; exit 3 ; }

    config_cmd=`echo "$1" | awk -F '|' '{print $4}'`
    pkg_name=`echo "${1}" | awk -F '|' '{print $1}'`
    log_file="${LOG_DIR}/${pkg_name}_config.log"

    run_command "${pkg_name}" "${log_file}" "configure" "${config_cmd}"
    
}

# This function make command if specified in package info line
# param 1: line from package list file.
build_and_install_package()
{
    [ -z "${1}" ] && { echo_error "package info is not given." ; exit 3 ; }

    pkg_name=`echo "${1}" | awk -F '|' '{print $1}'`
    install_cmd=`echo "${1}" | awk -F '|' '{print $5}'`
    log_file="${LOG_DIR}/${pkg_name}_build.log"
    
    run_command "${pkg_name}" "${log_file}" "install" "${install_cmd}"
}

install_package()
{
    package_list_file="${1}"
    src_dir="${2}"
    cur_dir=`pwd`

    [ -f "${package_list_file}" ] || { \
        echo_error "Unable to find $package_list_file file." ; \
        exit 2 ; } 
    
    [ -d "${src_dir}" ] || { \
        echo_error "Unable to find $src_dir directory." ; \
        exit 2 ; }

    while read line
    do
        comment_line=`echo $line | grep ^#`
        [  ! -z "$comment_line" ] && { continue ; }

        pkg_name=`echo $line | awk -F '|' '{print $1}'`
        [ -z "$pkg_name" ] && { continue ; }

        pkg_dir=`echo $line | awk -F '|' '{print $2}'`          

        [ -z "${pkg_dir}" ] && { \
            echo_error "Package directory is not mentioned in conf file." ; \
            echo_error "Exiting ..." ; \
            exit 3 ; }

        [ -d "${src_dir}/${pkg_dir}" ] || { \
            echo_error "Unable to find $pkg_dir inside $src_dir." ; \
            exit 4 ; }

        cd "${src_dir}/${pkg_dir}"
        
        [ ! -d "${LOG_DIR}" ] && mkdir -p "${LOG_DIR}"

        install_package_build_dependency "${line}"
        configure_package "${line}"
        build_and_install_package "${line}"

        cd "${cur_dir}" 1>/dev/null
    done <"${package_list_file}"
}

############################ Main Script ##################################

package_file=
src_dir=
tmp_dir=

while getopts "d:f:t:h" SWITCH ; do

    case $SWITCH in
    'f') package_file="${OPTARG}" 
        ;;
    't') tmp_dir="${OPTARG}"
        ;;
    'd') src_dir="${OPTARG}"
        ;;
    * | 'h') usage
       exit 192
        ;;
    esac
done   

[ -z "${package_file}" ] && { usage ; exit 192 ; }
[ -z "${src_dir}" ] && { usage ; exit 192 ; }
[ -z "${tmp_dir}" ] && { usage ; exit 192 ; }

export TMP="${tmp_dir}"
mkdir -p "$TMP"
install_package "${package_file}" "${src_dir}"
