#!/usr/bin/env python3

import requests

NPROBLEMS = int(open("../nprob", "r").read().strip()) + 1

for i in range(1, NPROBLEMS):
    url = "https://cdn.robovinci.xyz/imageframes/%d.png" % i
    filename = "%d.png" % i
    print("downloading: " + url)
    resp = requests.get(url)
    with open(filename, 'wb') as localFile:
        localFile.write(resp.content)

for i in range(1, NPROBLEMS):
    url = "https://cdn.robovinci.xyz/imageframes/%d.json" % i
    filename = "%d.json" % i
    print("downloading: " + url)
    resp = requests.get(url)
    with open(filename, 'wb') as localFile:
        localFile.write(resp.content)

for i in range(26, NPROBLEMS):
    url = "https://cdn.robovinci.xyz/imageframes/%d.initial.json" % i
    filename = "%d.initial.json" % i
    print("downloading: " + url)
    resp = requests.get(url)
    with open(filename, 'wb') as localFile:
        localFile.write(resp.content)

for i in range(36, NPROBLEMS):
    url = "https://cdn.robovinci.xyz/sourcepngs/%d.source.png" % i
    filename = "%d.source.png" % i
    print("downloading: " + url)
    resp = requests.get(url)
    with open(filename, 'wb') as localFile:
        localFile.write(resp.content)

for i in range(36, NPROBLEMS):
    url = "https://cdn.robovinci.xyz/sourcepngs/%d.source.json" % i
    filename = "%d.source.json" % i
    print("downloading: " + url)
    resp = requests.get(url)
    with open(filename, 'wb') as localFile:
        localFile.write(resp.content)
