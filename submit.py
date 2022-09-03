#!/usr/bin/env python3

import requests
import os
import json
import hashlib

NPROBLEMS = 31
STATUS_FILENAME = "submitterStatus.json"

def main():
    with open(STATUS_FILENAME, "rb") as f:
        status = json.load(f)

    auth = open("apiKey2", "rb").read().decode('utf-8').strip()
    print(auth)
    for i in range(1, NPROBLEMS):
        key = "task%d" % i
        filename = "solutions/best/%d.txt" % i
        if not os.path.exists(filename):
            filename = "solutions/best/empty.txt"
        with open(filename, "rb") as f:
            filedigest = hashlib.md5(open(filename, "rb").read()).hexdigest()
            if filedigest == status[key].get("submission_digest", 0):
                continue
            resp = requests.post("https://robovinci.xyz/api/problems/%d" % i, files=[('file', f)], headers={"Authorization": "Bearer %s" % auth})
            print(resp.text)
            if resp.text != "Limit exceeded":
                taskStatus = resp.json()
                if key not in status:
                    status[key] = {}
                status[key]["submission_id"] = taskStatus["submission_id"]
                status[key]["submission_digest"] = filedigest
                status[key]["submission_status"] = {}

    for i in range(1, NPROBLEMS):
        key = "task%d" % i
        if key in status and len(status[key]["submission_status"]) == 0:
            resp = requests.get("https://robovinci.xyz/api/submissions/%d" % status[key]["submission_id"], headers={"Authorization": "Bearer %s" % auth})
            print(resp.text)
            if resp.text != "Limit exceeded":
                status[key]["submission_status"] = resp.json()

    with open(STATUS_FILENAME, "w") as f:
        f.write(json.dumps(status, indent=4))


if __name__ == "__main__":
    main()
