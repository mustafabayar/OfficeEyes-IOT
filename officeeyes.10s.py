#!/usr/bin/env python
# coding=utf-8
#
# <bitbar.title>OfficeEyes PingPong Status</bitbar.title>
# <bitbar.version>v2.0</bitbar.version>
# <bitbar.author>mustafabayar</bitbar.author>
# <bitbar.author.github>mustafabayar</bitbar.author.github>
# <bitbar.desc>Tells you whether the pingpong table is free or not</bitbar.desc>
# <bitbar.dependencies>python</bitbar.dependencies>
#
# by mbcoder

import urllib
import json

free = False

url = 'https://office-eyes.herokuapp.com/bitbar'

try:
    r = urllib.urlopen(url).read()
except IOError:
    print("Server loading...")
    exit(1)

json_data = json.loads(r)

try:
    free = json_data['free']
except TypeError:
    print("Converting Data...")
    exit(1)

if free is True:
    print(':tennis:')
else:
    print(':no_entry:')
    
