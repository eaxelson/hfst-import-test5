#Read commandline arguments
args <- commandArgs(TRUE)

# Turn on pdf device driver. Writing to file given as 3rd commandline arg.
pdf(file=args[3])

# Read the table with accuracies on the IRC test material given in the first 
# commandline arg.
irc_1st_table = read.table(args[1],header=TRUE)

# Read the table with accuracies on the SMS test material given in the second
# commandline arg.
sms_1st_table = read.table(args[2],header=TRUE)

# Always plot from 0.0 to 1.0 on y-axis
g_range <- range(0.0,1.0)

# Plot the IRC material using o-type ticks and filled line
plot(irc_1st_table,type="o",ylim=g_range,lty=2)

#Plot the SMS material using 0-typ ticks and dashed line
lines(sms_1st_table,type="o")

# Flush the pdf device into accuracy_first_guess.pdf and close the device.
dev.off()

