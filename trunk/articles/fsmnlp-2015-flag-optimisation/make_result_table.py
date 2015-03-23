#! /usr/bin/env python3

import re
import os

MINUTES = "min"
SECONDS = "sec"

FIN = 0
KAL = 1
SME = 2

LANGUAGES = ["Finnish", "Greenlandic", "Northern Sami"]
LANGUAGE_IDS = ["fin", "kal", "sme"]

def parse(time_str, unit):
    mo = re.match(".*([0-9]+)m([0-9]+[.][0-9]+)s.*", time_str)

    minutes = float(mo.group(1))
    seconds = float(mo.group(2))

    if unit == SECONDS:
        return 60 * minutes + seconds
    else:
        return minutes + seconds / 60

def get_line_count(fn):
    lines = open(fn).read().split('\n')
    return len(lines)

def get_times(fn):
    lines = open(fn).read().split('\n')
    return list(filter(lambda l: l.find("user\t") == 0, lines))

class SubTable:
    def __init__(self, language, fst_id):
        self.language_name = LANGUAGES[language]
        lan_id = LANGUAGE_IDS[language]
        
        self.binary_size = os.path.getsize("%s/hfst_bin/%s.hfst.%s" % 
                                           (lan_id, lan_id, fst_id))
        
        input_count = get_line_count("data/%s_data.txt" % lan_id)
        compile_time_str = get_times("compile_%s_%s" % (lan_id, fst_id))[0]
        load_time_str, lookup_time_str = \
            get_times("lookup_%s_%s" % (lan_id, fst_id))

        self.binary_size /= 1024.0**2

        self.compile_time = parse(compile_time_str, MINUTES)

        load_time    = parse(load_time_str, SECONDS)
        lookup_time  = parse(lookup_time_str, SECONDS)
        input_count  = input_count
        self.lookup_speed = input_count / (lookup_time - load_time)
        self.lookup_speed /= 1000

class LanguageTable:
    def __init__(self, language):
        self.language_name = LANGUAGES[language]

        self.st1 = SubTable(language, "1")
        self.st2 = SubTable(language, "2")
        self.st3 = SubTable(language, "3")

    def __str__(self):
        hline = "\hline\n"
        end_hline = "\hline"

        lan_header = "\multicolumn{4}{c}{{\\sc %s}}\\\\\n" % self.language_name

        method_header = "               & None & H-M  & H-M + Cond. \\\\\n"

        compile_line  = "Compile (min)  & %4u & %4u & %4u        \\\\\n" % \
        (int(self.st1.compile_time), 
         int(self.st2.compile_time), 
         int(self.st3.compile_time))

        size_line = "Size (MB)      & %4.0f & %4.0f & %4.0f        \\\\\n" % \
        (self.st1.binary_size, self.st2.binary_size, self.st3.binary_size)

        lookup_line = "Lookup (kw/s)  & %4u & %4u & %4u        \\\\\n" % \
        (int(self.st1.lookup_speed), 
         int(self.st2.lookup_speed), 
         int(self.st3.lookup_speed))

        return (lan_header + 
                hline + 
                method_header + 
                hline + 
                compile_line +
                size_line + 
                lookup_line + 
                end_hline)

class Table:
    def __init__(self, languages):
        self.lan_tabs = [ LanguageTable(lan) for lan in languages ]

    def __str__(self):
        header = "\\begin{tabular}{lccc}\n"
        lan_strs = [ str(lt) for lt in self.lan_tabs ]
        empty_line = " & & & \\\\\n"
        footer = "\\end{tabular}"

        return (header + 
                empty_line.join(lan_strs) +
                "\n" +
                footer)

table = Table([FIN, KAL, SME])

print(table)
