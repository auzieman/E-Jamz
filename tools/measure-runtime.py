"""Short target-host CPU/RSS samples; audio services are reported separately."""
import argparse
import json
import os
from pathlib import Path
import time

parser = argparse.ArgumentParser()
parser.add_argument('pid', type=int)
parser.add_argument('state')
parser.add_argument('output')
parser.add_argument('--seconds', type=float, default=5)
args = parser.parse_args()
assert 1 <= args.seconds <= 60
def read(pid):
    p = Path('/proc') / str(pid)
    stat = (p / 'stat').read_text().rsplit(')', 1)[1].split()
    rows = (p / 'status').read_text().splitlines()
    rss = next((int(row.split()[1]) for row in rows if row.startswith('VmRSS:')), 0)
    return {'ticks': int(stat[11]) + int(stat[12]), 'rss_kib': rss}
uid = (Path('/proc') / str(args.pid)).stat().st_uid
pids = {args.pid: 'player'}
pending=[args.pid]
while pending:
    parent=pending.pop()
    try:
        for value in (Path('/proc')/str(parent)/'task'/str(parent)/'children').read_text().split():
            child=int(value)
            if child not in pids:
                pids[child]='player child';pending.append(child)
    except OSError:
        pass
for p in Path('/proc').iterdir():
    if not p.name.isdigit():
        continue
    try:
        name = (p / 'comm').read_text().strip()
        if p.stat().st_uid == uid and name in ('pipewire', 'pipewire-pulse', 'wireplumber'):
            pids[int(p.name)] = name
    except (OSError, ValueError):
        pass
before = {pid: read(pid) for pid in pids}
start = time.monotonic(); time.sleep(args.seconds); elapsed = time.monotonic() - start
results = []
for pid, name in pids.items():
    try:
        after = read(pid)
        cpu = (after['ticks'] - before[pid]['ticks']) / os.sysconf('SC_CLK_TCK') / elapsed * 100
        results.append({'pid': pid, 'name': name, 'cpu_percent_one_core': round(cpu, 2), 'rss_kib': after['rss_kib']})
    except OSError:
        results.append({'pid': pid, 'name': name, 'status': 'exited during sample'})
result = {'state': args.state, 'seconds': round(elapsed, 3), 'processes': results,
          'limits': 'short snapshot; player ticks include threads; descendants sampled separately; shared audio services may include other clients; no underrun or sustained-growth acceptance'}
Path(args.output).write_text(json.dumps(result, indent=2) + '\n')
print(json.dumps(result))
