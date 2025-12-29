import base64
import json
import requests
import urllib3
from pathlib import Path

urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)

def read_lockfile(lockfile_path: str):
    text = Path(lockfile_path).read_text().strip()
    # LeagueClient:PID:PORT:PASSWORD:PROTO
    parts = text.split(":")
    if len(parts) != 5:
        raise RuntimeError(f"Unexpected lockfile format: {text}")
    _, _, port, password, proto = parts
    return proto, int(port), password

def get_current_summoner(lockfile_path: str):
    proto, port, password = read_lockfile(lockfile_path)

    auth_str = f"riot:{password}"
    auth_b64 = base64.b64encode(auth_str.encode()).decode()

    url = f"{proto}://127.0.0.1:{port}/lol-summoner/v1/current-summoner"
    headers = {
        "Authorization": f"Basic {auth_b64}"
    }

    resp = requests.get(url, headers=headers, verify=False)
    resp.raise_for_status()
    return resp.json()

if __name__ == "__main__":
    # TODO: 改成你自己的 lockfile 路径
    lockfile = "/Applications/League of Legends.app/Contents/LoL/lockfile"

    data = get_current_summoner(lockfile)
    print(json.dumps(data, indent=2, ensure_ascii=False))