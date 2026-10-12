import pathlib,os,subprocess,json
r=pathlib.Path('/home/auzieman/Projects/E-projects/media-player');c=r/'build/out/prototype/drop-candidate'
f=pathlib.Path('/tmp/ejamz-drop-fixture');f.mkdir(exist_ok=True);(f/'nested').mkdir(exist_ok=True)
for name in ['A space.MP3','nested/b.xm','ignored.txt','.hidden.mod']: (f/name).write_bytes(b'fixture')
(f/'cycle').symlink_to(f,target_is_directory=True) if not (f/'cycle').exists() else None
single=pathlib.Path('/tmp/ejamz-single.med');single.write_bytes(b'fixture')
env=dict(os.environ,ELM_ENGINE='software_x11',ELM_ACCEL='none',EJAMZ_THEME=str(c/'player.edj'))
env['EJAMZ_VERIFY_DROP']='# test\r\n'+f.as_uri()+'\r\n'+single.as_uri()+'\r\nhttps://example.com/no.mp3\r\nfile://remote/tmp/no.mp3\r\n'
p=subprocess.run([str(c/'ejamz')],env=env,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=30)
(r/'build/out/prototype/drop-verification.log').write_bytes(p.stdout)
print(p.returncode,p.stdout.decode()[-2000:]);assert p.returncode==0 and b'VERIFY DROP: 3 tracks' in p.stdout
del env['EJAMZ_VERIFY_DROP'];env['EJAMZ_VERIFY']='1'
queue=json.loads((r/'build/out/prototype/verification-queue.json').read_text())
p=subprocess.run([str(c/'ejamz'),*queue],env=env,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=60)
(r/'build/out/prototype/drop-build-playback-verification.log').write_bytes(p.stdout)
print(p.returncode,p.stdout.decode()[-2200:]);assert p.returncode==0 and b'VERIFY COMPLETE: 5 files; Flat=PASS' in p.stdout
