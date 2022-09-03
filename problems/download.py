#!/usr/bin/env python3

import requests

for i in range(1, 26):
    url = "https://cdn.robovinci.xyz/imageframes/%d.png" % i
    filename = "%d.png" % i
    print("downloading: " + url)
    resp = requests.get(url)
    with open(filename, 'wb') as localFile:
        localFile.write(resp.content)
