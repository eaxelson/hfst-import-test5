#Read commandline arguments
args <- commandArgs(TRUE)

# Turn on pdf device driver. Writing to file given as 3rd commandline arg.
pdf(file=args[2])

# Read the table with accuracies for different training data set sizes.
table = read.table(args[1],header=TRUE)

# Plot from 0.5 to 1.0 on y axis.
g_range <- range(0.5,0.8)

# Plot the graph
plot(table,type="o",ylim=g_range)

# Flush the pdf device into target file.
dev.off()
