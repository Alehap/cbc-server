#!/usr/bin/env python
# coding: utf-8

# this script process all zh-Hans pages and add new pages for en with content
# NeedTranslationDefault

import os, os.path

def main():
    zhlist = os.listdir(".")
    enlist = os.listdir("en")
    for filename in zhlist:
        if os.path.isdir(filename):
            continue
        if filename not in enlist:
            cmd = "cp %s %s" % ("en/NeedTranslationDefault", filename)
            print cmd
            os.system(cmd)
        else:
            firstline = open(filename).readline().strip()
            # overwrite those which do not have summary
            if firstline == "#summary":
                cmd = "cp %s %s" % ("en/NeedTranslationDefault", filename)
                print cmd
                os.system(cmd)

if __name__ == "__main__":
    main()

