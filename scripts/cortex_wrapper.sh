#!/bin/sh

# Report output name
OUPUTNAME="$1"

# Examples for cortex chain commands
# Simply gzip the output
COMMAND_GZ="gzip > $OUPUTNAME.gz"

# Add some userland context info and gzip all
COMMAND_CTX_GZ="context.sh | gzip > $OUPUTNAME.gz"

# Do the processing
cortex -e "$COMMAND_GZ"

