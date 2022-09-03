#!/usr/bin/env python3

import requests
import os

NPROBLEMS = 31

def main():
    auth = open("apiKey2", "rb").read().decode('utf-8').strip()
    print(auth)
    for i in range(1, NPROBLEMS):
        filename = "solutions/best/%d.txt" % i
        if not os.path.exists(filename):
            filename = "solutions/best/empty.txt"
        with open(filename, "rb") as f:
            resp = requests.post("https://robovinci.xyz/api/problems/%d" % i, files=[('file', f)], headers={"Authorization": "Bearer %s" % auth})
            print(resp.text)


if __name__ == "__main__":
    main()
