#!/usr/bin/env python3

import requests

NIMAGES = 31

for i in range(1, NIMAGES):
    url = "https://cdn.robovinci.xyz/imageframes/%d.png" % i
    filename = "%d.png" % i
    print("downloading: " + url)
    resp = requests.get(url)
    with open(filename, 'wb') as localFile:
        localFile.write(resp.content)

for i in range(26, NIMAGES):
    url = "https://cdn.robovinci.xyz/imageframes/%d.initial.json" % i
    filename = "%d.initial.json" % i
    print("downloading: " + url)
    resp = requests.get(url)
    with open(filename, 'wb') as localFile:
        localFile.write(resp.content)
