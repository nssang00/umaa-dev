"""Print bounded official generated UMAA engine type layout for implementation verification."""
import pathlib, re, zipfile
root=pathlib.Path('/tmp/umaa-official/idl')
archive=pathlib.Path('/tmp/umaa-6.0-idl.zip')
with zipfile.ZipFile(archive) as z:
    selected = [n for n in z.namelist() if (
        'EngineControl' in n or
        ('Ack' in n and n.endswith('.idl')) or
        ('CommandStatus' in n and n.endswith('.idl'))
    )]
    print('ENGINE_ARCHIVE_ENTRIES',len(selected))
    for n in selected[:90]: print('ENGINE_ENTRY',n)
base=pathlib.Path('/tmp/umaa-official/engine-type-support')
for path in sorted(base.rglob('*.h')):
    if not any(t in path.name for t in ('EngineCommandType','EngineCommandStatusType','CommandAck','CommandStatus','CommandType','Measurements')):
        continue
    lines=path.read_text(errors='replace').splitlines()
    print('HEADER',path.name,len(lines))
    for i,line in enumerate(lines):
        if re.search(r'(typedef struct|typedef enum|#define .*Topic|^} .*;|\b(sessionID|source|destination|timeStamp|status|command|commandState|ack|reason|rpm|engine|shaft)\b)',line,re.I):
            print('H',i,line.strip()[:180])
