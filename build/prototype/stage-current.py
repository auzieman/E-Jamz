import tarfile,hashlib,json,subprocess
from pathlib import Path
r=Path(__file__).resolve().parents[2];p=r.parents[1]/'BlackKnightController/pipelines/e-native-prototype-build'
with tarfile.open(p/'source.tgz','w:gz') as t:
 for f in ['src/ui/prototype.c','src/ui/theme-palette.h','src/ui/player.edc','build/prototype/build.sh','build/prototype/alpine-public.rsa.pub','assets/skins/blue-steel/assets/button.png','assets/fonts/Doto-variable.ttf','assets/fonts/OFL.txt','assets/fonts/provenance.json']:t.add(r/f,arcname=f)
h=hashlib.sha256((p/'source.tgz').read_bytes()).hexdigest();m={'baseline':'63c043a','sha256':h,'remote_bundle':f'/var/lib/bkc/e-native-inputs/{h}.tgz','project':'E-Jamz','scope':'EJ-013 attached animated settings drawer'}
(p/'source.json').write_text(json.dumps(m,indent=2)+'\n')
subprocess.run(['flatpak-spawn','--host','ssh','root@192.168.1.15',f'cat > {m["remote_bundle"]}'],input=(p/'source.tgz').read_bytes(),check=True)
for c in ['blackknight_bkc.1.ls8r9k54l3utbld0vx6fx9suy','blackknight_bkc-worker.1.w44wnuc1oyhbsq3uifu2q207n','blackknight_bkc-slow-worker.1.ey17ssi4ew9psfiscu82ajrc9']:
 subprocess.run(['flatpak-spawn','--host','docker','--context','bkc-edge-swarm','exec','-i',c,'sh','-c','cat > /app/runtime/pipelines/e-native-prototype-build/source.json'],input=(p/'source.json').read_bytes(),check=True)
subprocess.run(['flatpak-spawn','--host','docker','--context','bkc-edge-swarm','exec','blackknight_bkc.1.ls8r9k54l3utbld0vx6fx9suy','python','/app/bkc_cli.py','trigger-pipeline','--pipeline-id','e-native-prototype-build','--notes',m['scope']],check=True)
