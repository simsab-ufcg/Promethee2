#http://ggplot2.tidyverse.org/reference/#section-scales

library("ggplot2")
library("scales")
library("data.table")
library("stringi")

args = commandArgs(trailingOnly=TRUE)

cpu_usage_path = args[1]
mem_usage_path = args[2]
disk_usage_path = args[3]
proctimes_path = args[4]

cpu_usage = read.table(sep = ",", cpu_usage_path, header=TRUE)
mem_usage = read.table(sep = ",", mem_usage_path, header=TRUE)
disk_usage = read.table(sep = ",", disk_usage_path, header=TRUE)
proctimes = read.table(sep = " ", proctimes_path, header=FALSE)

# Cada proctime com label deve indicar o fim de o processamento desse label no script
proctimes = subset(proctimes, stri_length(V2) > 0)

cpu_min_TS = min(cpu_usage$TIMESTAMP)
mem_min_TS = min(mem_usage$TIMESTAMP)
disk_min_TS = min(disk_usage$TIMESTAMP)

cpu_usage$IDLE = 100 - cpu_usage$IDLE

mem_usage$TOTAL = (mem_usage$USED/mem_usage$TOTAL)*100

cpu_usage$TIMESTAMP = cpu_usage$TIMESTAMP - cpu_min_TS
mem_usage$TIMESTAMP = mem_usage$TIMESTAMP - mem_min_TS
disk_usage$TIMESTAMP = disk_usage$TIMESTAMP - disk_min_TS

cpu_usage = cpu_usage[c("TIMESTAMP", "IDLE", "GNICE")]
mem_usage = mem_usage[c("TIMESTAMP", "TOTAL", "BUFFER.CACHE")]
disk_read_usage = disk_usage[c("TIMESTAMP", "KB_READ.S", "KB_WRITE.S")]#TIMESTAMP, KB/S, TYPE(READ/WRITE)
disk_write_usage = disk_usage[c("TIMESTAMP", "KB_READ.S", "KB_WRITE.S")]

disk_write_usage$"KB_READ.S" = disk_write_usage$"KB_WRITE.S"

setnames(cpu_usage, c("TIMESTAMP", "USAGE", "TYPE"))
setnames(mem_usage, c("TIMESTAMP", "USAGE", "TYPE"))
setnames(disk_read_usage, c("TIMESTAMP", "MB.S", "TYPE"))
setnames(disk_write_usage, c("TIMESTAMP", "MB.S", "TYPE"))

cpu_usage$TYPE = 'CPU'
mem_usage$TYPE = 'MEM'
disk_read_usage$TYPE = 'READ'
disk_write_usage$TYPE = 'WRITE'

data_cpu_mem = rbind(cpu_usage, mem_usage)
data_disk = rbind(disk_write_usage, disk_read_usage)

data_disk$TYPE = as.character(data_disk$TYPE)
data_disk$TYPE = factor(data_disk$TYPE, levels = unique(data_disk$TYPE))

data_disk$MB.S = data_disk$MB.S/1024

med_proctimes = 0
previous = -300
for(i in 1:dim(proctimes)[1])
{
	med_proctimes[i] = (previous + proctimes$V1[i])/2.00
	previous = proctimes$V1[i]
}


pplot = ggplot(cpu_usage, aes(x=TIMESTAMP, y=USAGE, group=TYPE, colour=TYPE)) +
	geom_line(size=0.3) + xlab("TIME (s)") + 
	ylab("USAGE (%)") + 
	geom_vline(xintercept = proctimes$V1, linetype=2, size=0.15) +
	annotate("text", x = med_proctimes, y=0.0, label = proctimes$V2) + 
	scale_colour_manual(values=c("#FF7777")) + 
	theme_bw() + 
	guides(fill=guide_legend(title=NULL)) + 
	theme(legend.title=element_blank())

ggsave("usage_cpu.png", pplot, width=14)


pplot = ggplot(mem_usage, aes(x=TIMESTAMP, y=USAGE, group=TYPE, colour=TYPE)) + 
	geom_line(size=0.3) + xlab("TIME (s)") + 
	ylab("USAGE (%)") + 
	geom_vline(xintercept = proctimes$V1, linetype=2, size=0.15) + 
	annotate("text", x = med_proctimes, y=0.0, label = proctimes$V2) + 
	scale_colour_manual(values=c("#0066CC")) + 
	theme_bw() + 
	guides(fill=guide_legend(title=NULL)) + 
	theme(legend.title=element_blank())

ggsave("usage_mem.png", pplot, width=14)


pplot = ggplot(data_cpu_mem, aes(x=TIMESTAMP, y=USAGE, color=TYPE)) + 
	geom_line(size=0.3) + 
	xlab("TIME (s)") + 
	ylab("USAGE (%)") + 
	geom_vline(xintercept = proctimes$V1, linetype=2, size=0.15) + 
	annotate("text", x = med_proctimes, y=0.0, label = proctimes$V2) + 
	scale_colour_manual(values=c("#FF7777", "#0066CC")) + 
	theme_bw() + 
	guides(fill=guide_legend(title=NULL)) + 
	theme(legend.title=element_blank())

ggsave("usage_cpu_mem.png", pplot, width=14)


pplot = ggplot(data_disk, aes(x=TIMESTAMP, y=MB.S, color=TYPE)) + 
	geom_line(size=0.3) + 
	xlab("TIME (s)") + 
	ylab("MB/S") + 
	geom_vline(xintercept = proctimes$V1, linetype=2, size=0.15) + 
	annotate("text", x = med_proctimes, y=-10.0, label = proctimes$V2) + 
	scale_colour_manual(values=c("#FF7777", "#0066CC")) + 
	theme_bw() + 
	guides(fill=guide_legend(title=NULL)) + 
	theme(legend.title=element_blank())

ggsave("usage_disk.png", pplot, width=14)