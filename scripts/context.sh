
# set command for context
commands="top ifconfig lsusb lspci free df mount"

# set label and option for each command
cmd_label_top="Processes"
cmd_option_top="-bn 1"
cmd_label_ifconfig="Network"
cmd_option_ifconfig=
cmd_label_lsusb="USB devices"
cmd_option_lsusb=
cmd_label_lspci="PCI Devices"
cmd_option_lspci=
cmd_label_free="Memory"
cmd_option_free="-m"
cmd_label_df="Disks"
cmd_option_df="-m"
cmd_label_mount="Mount"
cmd_option_mount=

# Display crashreport
while read line; do
	echo $line
done

# Display context
for command in $commands; do
	label=$(eval echo \$cmd_label_$command)
	option=$(eval echo \$cmd_option_$command)

	[ -z "$label" ] && label=$command

	if which $command &> /dev/null; then
		echo "#### $label #####"
		$command $option
		echo
	fi
done 

