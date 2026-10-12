from pathlib import Path
import tempfile,subprocess,os,json
r=Path(__file__).resolve().parents[2];o=r/'build/out/prototype';c=o/'ej011-final';test=Path(tempfile.mkdtemp(prefix='playlist-test-',dir=o))
queue=json.loads((o/'verification-queue.json').read_text())+[str(sorted(Path('/mnt/seagate-readonly/Music/Daft Punk/TRON_ Legacy Reconfigured').glob('*.m4a'))[0])]
env=dict(os.environ,ELM_ENGINE='software_x11',ELM_ACCEL='none',EJAMZ_THEME=str(c/'player.edj'),EJAMZ_VERIFY='1',EJAMZ_VERIFY_IO=str(test/'filtered.m3u8'))
p=subprocess.run([str(c/'ejamz'),*queue],env=env,capture_output=True,timeout=90);(o/'ej011-integrated.log').write_bytes(p.stdout+p.stderr);print(p.returncode,p.stdout.decode(),p.stderr.decode(),flush=True)
assert p.returncode==0 and b'VERIFY COMPLETE: 6 files; Flat=PASS' in p.stdout
assert (test/'filtered.m3u8').read_text().splitlines()==['#EXTM3U',queue[1]]
# Independently authored BOM/CRLF playlist, relative path, URI spaces, missing and remote entries.
(test/'track.s3m').symlink_to(queue[0]);fixture=test/'ordered.m3u8'
fixture.write_bytes(('\ufeff#EXTM3U\r\ntrack.s3m\r\n'+Path(queue[1]).as_uri()+'\r\nhttps://example.com/radio\r\nmissing.mp3\r\n').encode())
env.pop('EJAMZ_VERIFY');env.pop('EJAMZ_VERIFY_IO');env['EJAMZ_VERIFY_DROP']=fixture.as_uri()+'\r\n'
p=subprocess.run([str(c/'ejamz')],env=env,capture_output=True,timeout=30);(o/'ej011-import.log').write_bytes(p.stdout+p.stderr);print(p.returncode,p.stdout.decode(),flush=True)
assert p.returncode==0 and b'VERIFY DROP: 2 tracks / 2 skipped' in p.stdout
assert [x.removeprefix('VERIFY IMPORT: ') for x in p.stdout.decode().splitlines() if x.startswith('VERIFY IMPORT: ')]==[str(Path(queue[0]).resolve()),str(Path(queue[1]).resolve())]
print('PLAYLIST ORDER / RELATIVE PATH / URI / BOM / SKIPS PASS',flush=True)
(o/'ej011-test-folder.txt').write_text(str(test)+'\n')
