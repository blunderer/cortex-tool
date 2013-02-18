#!/bin/sh

CORTEX_OUPTUT_DIR=/var/log/cortex
HANDLER="|/usr/local/bin/cortex_wrapper.sh $CORTEX_OUPTUT_DIR/report_%e_%p.log"

[ -f /etc/default/cortex.conf ] && source /etc/default/cortex.conf

case $1 in
	"start")
		mkdir -p $CORTEX_OUPTUT_DIR

		if [ -f /bin/busybox ]; then
			# enable coredump for busybox 
			touch /.init_enable_core
		fi

		# install the cortex coredump handler
		echo "$HANDLER" > /proc/sys/kernel/core_pattern
	;;

	"stop")
		if [ -f /bin/busybox ]; then
			# disable coredump for busybox 
			rm /.init_enable_core
		fi

		# uninstall the cortex coredump handler
		echo "core" > /proc/sys/kernel/core_pattern
	;;
esac

